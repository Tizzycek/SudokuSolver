#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVector>
#include <QLineEdit>
#include "libs/SudokuSolverAlgorithm.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Main application window for the Sudoku solver.
 *
 * This class builds and manages the Sudoku board UI, the number pad and
 * action buttons, and coordinates interactions with the underlying
 * SudokuSolverAlgorithm running on a worker thread. User input is
 * validated and propagated to the solver; solve progress is periodically
 * reflected back to the UI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window and sets up all UI components.
     * @param parent Optional parent widget.
     */
    MainWindow(QWidget *parent = nullptr);
    /**
     * @brief Destroys the window and releases owned resources.
     */
    ~MainWindow();

protected:
    /**
     * @brief Keeps the grid perfectly square and aligns the options panel with it.
     *
     * Called automatically by Qt on window resize; adjusts layout margins so the
     * Sudoku grid remains square and the options panel below shares the same
     * horizontal margins.
     * @param event Resize event delivered by Qt.
     */
    void resizeEvent(QResizeEvent *event) override;
    /**
     * @brief Ensures background solver thread is terminated before the window closes.
     */
    void closeEvent(QCloseEvent* event) override;

private slots:
    // Slots
    /**
     * @brief Handles text edits in a grid cell.
     *
     * Validates and applies the value to the solver. If invalid, resets the
     * cell and informs the user.
     * @param row Zero-based row index of the edited cell.
     * @param col Zero-based column index of the edited cell.
     * @param text The text entered by the user (validated to a single digit 1–9 or empty).
     */
    void handleCellInput(unsigned short row, unsigned short col, const QString &text);

private:
    // UI setup helpers
    /**
     * @brief Creates the Sudoku grid widget and populates it with editable cells.
     * @param size Grid dimension (typically 9 for a 9x9 Sudoku).
     * @return Newly created widget containing the grid layout.
     */
    QWidget* setupGrid(const unsigned short&);
    /**
     * @brief Creates the lower options panel containing the number pad and action buttons.
     * @param size Grid dimension, used to build the number pad.
     * @return Newly created widget containing the options panel.
     */
    QWidget* setupOptionsPanel(const unsigned short&);
    /**
     * @brief Builds the number pad used to input digits with mouse/touch.
     * @param size Grid dimension; determines the range of numbers displayed.
     * @return A widget containing the number pad.
     */
    QWidget* setupNumberPad(const unsigned short &);
    /**
     * @brief Creates the action buttons (Reset, Solve).
     * @return A widget containing the action buttons arranged vertically.
     */
    QWidget* setupButtons();

    template <class T>
    /**
     * @brief Resizes the grid cells proportionally to keep the board readable.
     *
     * The concrete implementation lives in resizer.tpp.
     * @tparam T A Qt layout/container type holding the grid.
     * @param layout The layout/container instance to resize.
     */
    void resizeGrid(T&);

    // Event handlers and actions
    /**
     * @brief Filters key/mouse events on grid cells to support navigation and selection tracking.
     * @param watched The object being watched.
     * @param event The event being filtered.
     * @return true if the event is handled; otherwise false to continue default processing.
     */
    bool eventFilter(QObject *watched, QEvent *event) override;
    /**
     * @brief Prompts the user to confirm resetting the entire grid.
     */
    void askResetCells();
    /**
     * @brief Clears all cells and resets the solver state.
     */
    void resetCells();
    /**
     * @brief Prompts the user to start solving the current puzzle.
     */
    void askSolve();
    /**
     * @brief Launches the solver on a worker thread and locks the grid while solving.
     *
     * On completion, updates the grid with the solved values, unlocks the UI, and
     * informs the user about the outcome.
     */
    void solveSequence();
    /**
     * @brief Periodically reflects solver progress into the UI while the worker thread runs.
     */
    void startProgressMonitor();
    /**
     * @brief Inserts a digit into the currently selected cell via number pad.
     * @param val Digit value in the range [1, dim].
     */
    void handleNumberPadInput(unsigned short val);

    // Member variables — UI elements
    /** UI generated helper (from Qt Designer). */
    Ui::MainWindow *ui;
    /** Central container widget for the window. */
    QWidget* centralWidget;
    /** Top-level vertical layout stacking grid and options panel. */
    QVBoxLayout* mainLayout;
    /** Container for the Sudoku grid. */
    QWidget* gridWidget;
    /** Container for the lower options. */
    QWidget* optionPanel;
    /** Container for the number pad. */
    QWidget* numberPad;
    /** Layout for the number pad (owned by numberPad). */
    QGridLayout* numberPadLayout;
    /** Currently focused/selected cell (nullptr if none). */
    QLineEdit* selectedCell = nullptr;

    // Member variables — grid/state
    /** 2D array of pointers to the grid cells. */
    QVector<QVector<QLineEdit*>> cells;
    /** Grid dimension (e.g., 9 for 9x9). */
    unsigned short dim;
    /** Base font used by grid cells. */
    QFont font;

    // Member variables — solver/threading
    /** Pointer to the solver algorithm (owned). */
    SudokuSolverAlgorithm* solver;
    /** Pointer to the solver worker thread (nullptr when idle). */
    QThread* solverThread = nullptr;          // puntatore al thread del solver
    /** Last applied progress size used to incrementally mirror solver updates. */
    unsigned long lastCoordsSize = 0;         // ultima dimensione letta di coords

};

#include "resizer.tpp"

#endif // MAINWINDOW_H
