#include "SudokuSolverAlgorithm.h"

SudokuSolverAlgorithm::SudokuSolverAlgorithm(const unsigned short & dim) {
    this->dimension = dim;
    // Calcoliamo la dimensione del blocco (es. sqrt(9) = 3)
    this->blockSize = static_cast<unsigned short>(std::sqrt(dim));

    grid = new unsigned short*[dimension];
    for (unsigned short i = 0; i < dimension; i++) {
        grid[i] = new unsigned short[dimension];
        // FIX 1: Inizializzazione a 0 (vuoto)
        for (unsigned short j = 0; j < dimension; j++) {
            grid[i][j] = 0;
        }
    }
}

SudokuSolverAlgorithm::~SudokuSolverAlgorithm() {
    for (unsigned short i = 0; i < dimension; ++i) {
        delete [] grid[i];
    }
    delete [] grid;
}

void SudokuSolverAlgorithm::insert(const unsigned short & value, const unsigned short & row, const unsigned short & column) {
    if (row < dimension && column < dimension && value > 0 && value <= dimension) {
        grid[row][column] = value;
    }
}

bool SudokuSolverAlgorithm::isSafe(const unsigned short & row, const unsigned short & col, const unsigned short & num) const {
    // Controlla se il numero esiste già nella riga (escludendo la cella corrente)
    for (unsigned short x = 0; x < dimension; x++) {
        if (x != col && grid[row][x] == num) return false;
    }

    // Controlla se il numero esiste già nella colonna (escludendo la cella corrente)
    for (unsigned short x = 0; x < dimension; x++) {
        if (x != row && grid[x][col] == num) return false;
    }

    // Controlla il blocco (quadrato)
    unsigned short startRow = row - (row % blockSize);
    unsigned short startCol = col - (col % blockSize);

    for (unsigned short i = 0; i < blockSize; i++) {
        for (unsigned short j = 0; j < blockSize; j++) {
            unsigned short currRow = startRow + i;
            unsigned short currCol = startCol + j;
            if (!(currRow == row && currCol == col) && grid[currRow][currCol] == num) {
                return false;
            }
        }
    }

    return true;
}

bool SudokuSolverAlgorithm::solve() {
    // Wrapper pubblico per avviare la ricorsione dalla prima cella

    //printGrid();

    if (!checkAll()) 
        return false;

    return solveRecursive(0, 0);
}

bool SudokuSolverAlgorithm::solveRecursive(unsigned short row, unsigned short column) {
    // Caso base: abbiamo superato l'ultima riga
    if (row == dimension) {
        return true;
    }

    // Se siamo alla fine di una riga, passa alla successiva
    if (column == dimension) {
        return solveRecursive(row + 1, 0);
    }

    // Se la cella è già piena, passa alla prossima
    if (grid[row][column] != 0) {
        return solveRecursive(row, column + 1);
    }

    // Prova tutti i numeri possibili
    for (unsigned short num = 1; num <= dimension; num++) {
        if (isSafe(row, column, num)) {
            grid[row][column] = num;

            pushCoord(row, column);

            if (solveRecursive(row, column + 1)) {
                return true;
            }

            // Backtrack
            grid[row][column] = 0;
        }
    }

    return false;
}

void SudokuSolverAlgorithm::printGrid() const {
    for (unsigned short i = 0; i < dimension; i++) {
        for (unsigned short j = 0; j < dimension; j++) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void SudokuSolverAlgorithm::clean(const unsigned short & row, const unsigned short & col) {
    if (row < dimension && col < dimension)
        grid[row][col] = 0;
}

void SudokuSolverAlgorithm::clean() {
    for (unsigned short i = 0; i < dimension; i++) {
        for (unsigned short j = 0; j<dimension; j++)
            grid[i][j] = 0;
    }
}

unsigned short SudokuSolverAlgorithm::get(const unsigned short & row, const unsigned short & column) const {
    if (row < dimension && column < dimension)
        return grid[row][column];
    return 0;
}

bool SudokuSolverAlgorithm::checkAll() const {
    for (unsigned short i = 0; i < dimension; i++)
        for (unsigned short j = 0; j < dimension; j++)
            if (grid[i][j])
                if (! isSafe(i, j, grid[i][j])) {
                    printf("Errore alla riga %d colonna %d valore %d", i, j, grid[i][j]);
                    return false;
                }

    return true;
}

size_t SudokuSolverAlgorithm::coordsSize() const {
    std::lock_guard<std::mutex> guard(coordsMutex);
    return coords.size();
}

std::pair<unsigned short, unsigned short> SudokuSolverAlgorithm::coordAt(size_t i) const {
    std::lock_guard<std::mutex> guard(coordsMutex);
    return coords.at(i);
}

void SudokuSolverAlgorithm::pushCoord(unsigned short r, unsigned short c) {
    std::lock_guard<std::mutex> guard(coordsMutex);
    coords.emplace_back(r, c);
}