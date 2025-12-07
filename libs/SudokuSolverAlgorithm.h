#ifndef SUDOKUSOLVERALGORITHM_LIBRARY_H
#define SUDOKUSOLVERALGORITHM_LIBRARY_H

class SudokuSolverAlgorithm {
private:
	unsigned short dimension;
	unsigned short blockSize; // Es: 3 per un sudoku 9x9
	unsigned short **grid;    // Usiamo una sola griglia per semplicità e velocità

public:
	explicit SudokuSolverAlgorithm(const unsigned short &);
	~SudokuSolverAlgorithm();

	// Inserisce un valore iniziale (fisso)
	void insert(const unsigned short &, const unsigned short &, const unsigned short &) const;

	// Avvia la risoluzione
	bool solve();

	// Stampa per debug (opzionale)
	void printGrid() const;

private:
	// Funzione ricorsiva interna ottimizzata
	bool solveRecursive(unsigned short, unsigned short);

	// Controlla se è sicuro mettere n in r,c
	[[nodiscard]] bool isSafe(const unsigned short &, const unsigned short &, const unsigned short &) const;
};

#endif // SUDOKUSOLVERALGORITHM_LIBRARY_H