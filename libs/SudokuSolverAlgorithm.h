#ifndef SUDOKUSOLVERALGORITHM_LIBRARY_H
#define SUDOKUSOLVERALGORITHM_LIBRARY_H

#include <vector>
#include <cmath>
#include <iostream>
#include <mutex>

class SudokuSolverAlgorithm {
private:
	unsigned short dimension;
	unsigned short blockSize; // Es: 3 per un sudoku 9x9
	unsigned short **grid;    // Usiamo una sola griglia per semplicità e velocità
	
	public:
	explicit SudokuSolverAlgorithm(const unsigned short &);
	~SudokuSolverAlgorithm();
	
	// Inserisce un valore iniziale (fisso)
	void insert(const unsigned short & value, const unsigned short & row, const unsigned short & column);

	// Avvia la risoluzione
	bool solve();

	// Stampa per debug (opzionale)
	void printGrid() const;

	void clean(const unsigned short & row, const unsigned short & column);
	void clean();

    // Controlla se è sicuro mettere n in r, c
    [[nodiscard]] bool isSafe(const unsigned short & row, const unsigned short & col, const unsigned short & num) const;

	[[nodiscard]] unsigned short get(const unsigned short & row, const unsigned short & column) const;

	// Funzioni thread-safe per leggere coords
    size_t coordsSize() const;
    std::pair<unsigned short, unsigned short> coordAt(size_t i) const;

    // Funzione thread-safe per aggiungere
    void pushCoord(unsigned short r, unsigned short c);


private:
	// Funzione ricorsiva interna ottimizzata
	[[nodiscard]] bool checkAll() const;
	bool solveRecursive(unsigned short row, unsigned short column);

	mutable std::mutex coordsMutex;
    std::vector<std::pair<unsigned short, unsigned short>> coords;
	
	SudokuSolverAlgorithm(const SudokuSolverAlgorithm&) = delete;
	SudokuSolverAlgorithm& operator=(const SudokuSolverAlgorithm&) = delete;
};


#endif // SUDOKUSOLVERALGORITHM_LIBRARY_H
