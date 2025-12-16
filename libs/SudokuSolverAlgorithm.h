#ifndef SUDOKUSOLVERALGORITHM_LIBRARY_H
#define SUDOKUSOLVERALGORITHM_LIBRARY_H

#include <vector>
#include <cmath>
#include <iostream>
#include <mutex>

/**
 * @file SudokuSolverAlgorithm.h
 * @brief Public API for the shared library that solves Sudoku puzzles.
 *
 * This header declares the `SudokuSolverAlgorithm` class, a simple backtracking
 * Sudoku solver designed to be used from the UI thread or a worker thread.
 *
 * Key characteristics:
 * - Supports square Sudoku of size `dimension x dimension` where `dimension`
 *   is typically 9, and the sub-block size is `sqrt(dimension)` (e.g. 3 for 9x9).
 * - Provides basic input methods (`insert`, `clean`), validity checks (`isSafe`),
 *   a synchronous solver entry point (`solve`), and read-back utilities (`get`).
 * - Exposes a small, thread-safe progress buffer (`coords`, guarded by a mutex)
 *   to mirror incremental placements while solving on a background thread.
 *
 * Usage notes:
 * - Create an instance with the desired dimension, populate initial clues with
 *   `insert`, then call `solve()`. After completion, read values via `get()`.
 * - When used from multiple threads, only the progress methods (`coordsSize`,
 *   `coordAt`, `pushCoord`) are thread-safe. Other methods should be called
 *   in a controlled context (e.g., single worker thread) while the UI only
 *   reads progress.
 */

class SudokuSolverAlgorithm {
private:
    /** Overall puzzle dimension (e.g., 9 for a 9x9 Sudoku). */
    unsigned short dimension;
    /** Sub-square side length, computed as `sqrt(dimension)` (e.g., 3 for 9x9). */
    unsigned short blockSize;
    /**
     * Backing grid, allocated as a `dimension x dimension` matrix of unsigned short.
     * Values are 0 for empty cells, 1..dimension for filled cells.
     */
    unsigned short **grid;

    public:
    /**
     * @brief Constructs a solver for a square Sudoku of the given size.
     * @param dimension Grid size (e.g., 9 for 9x9). Must be a perfect square to
     *        derive an integer sub-block size. Values are not validated here.
     *
     * The constructor allocates and zero-initializes the internal grid and
     * computes the `blockSize` as `sqrt(dimension)`.
     */
    explicit SudokuSolverAlgorithm(const unsigned short & dimension);

    /**
     * @brief Frees the allocated grid and internal resources.
     */
    ~SudokuSolverAlgorithm();
	
 /**
  * @brief Inserts a value at the given cell as an initial (fixed) clue.
  * @param value Number in the range [1, dimension]. Values outside range are ignored.
  * @param row Zero-based row index (0..dimension-1).
  * @param column Zero-based column index (0..dimension-1).
  *
  * If `row`/`column` are out of bounds or `value` is 0/out of range, the call
  * has no effect. No validity checks against Sudoku rules are performed here.
  */
 void insert(const unsigned short & value, const unsigned short & row, const unsigned short & column);

 /**
  * @brief Attempts to solve the puzzle using backtracking.
  * @return true if a complete solution is found; false otherwise (e.g., invalid setup).
  *
  * This is a synchronous call that explores the search space depth-first.
  * It assumes the current grid contains the initial clues and empty cells (0).
  */
 bool solve();

 /**
  * @brief Prints the grid to stdout for debugging purposes.
  */
 void printGrid() const;

 /**
  * @brief Clears a single cell (sets it to 0).
  * @param row Zero-based row index.
  * @param column Zero-based column index.
  */
 void clean(const unsigned short & row, const unsigned short & column);

 /**
  * @brief Clears the entire grid (sets all cells to 0).
  */
 void clean();

    /**
     * @brief Checks whether placing a number at the given position is valid.
     * @param row Zero-based row index.
     * @param col Zero-based column index.
     * @param num Candidate number (1..dimension).
     * @return true if Sudoku constraints are satisfied in row, column, and sub-block.
     */
    [[nodiscard]] bool isSafe(const unsigned short & row, const unsigned short & col, const unsigned short & num) const;

 /**
  * @brief Reads the value at a cell.
  * @param row Zero-based row index.
  * @param column Zero-based column index.
  * @return The value in [1, dimension], or 0 if empty/out of bounds.
  */
 [[nodiscard]] unsigned short get(const unsigned short & row, const unsigned short & column) const;

 /**
     * @name Solver progress (thread-safe)
     * Methods that expose incremental placements performed by the solver.
     * These are protected by an internal mutex and can be safely called
     * from another thread (e.g., the UI) while the solver runs.
     */
    ///@{
    /** @brief Number of recorded coordinate updates. */
    size_t coordsSize() const;
    /**
     * @brief Returns the i-th recorded coordinate (row, col).
     * @param i Zero-based index into the progress buffer, in range [0, coordsSize()).
     */
    std::pair<unsigned short, unsigned short> coordAt(size_t i) const;
    ///@}

    /**
     * @brief Appends a coordinate to the progress buffer (thread-safe).
     * @param r Row index.
     * @param c Column index.
     */
    void pushCoord(unsigned short r, unsigned short c);


private:
    /**
     * @brief Validates the current grid state against Sudoku constraints.
     * @return false if any preset value violates rules; true otherwise.
     */
    [[nodiscard]] bool checkAll() const;

    /**
     * @brief Core backtracking routine.
     * @param row Current row being explored.
     * @param column Current column being explored.
     * @return true if a solution is found from this state; false to backtrack.
     */
    bool solveRecursive(unsigned short row, unsigned short column);

 /** Mutex protecting access to the `coords` progress buffer. */
    mutable std::mutex coordsMutex;
    /** FIFO list of coordinates set during solving, for UI progress display. */
    std::vector<std::pair<unsigned short, unsigned short>> coords;
	
    /** Non-copyable to avoid double-free of the internal grid. */
    SudokuSolverAlgorithm(const SudokuSolverAlgorithm&) = delete;
    /** Non-copyable to avoid double-free of the internal grid. */
    SudokuSolverAlgorithm& operator=(const SudokuSolverAlgorithm&) = delete;
};


#endif // SUDOKUSOLVERALGORITHM_LIBRARY_H
