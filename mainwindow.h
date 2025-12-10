#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QVector>
#include <qlineedit.h>
#include "libs/SudokuSolverAlgorithm.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;
    QWidget*centralWidget;
    QVBoxLayout*mainLayout;
    QLineEdit* currentCell = nullptr;

    QThread* solverThread = nullptr;    // puntatore al thread del solver
    unsigned long lastCoordsSize = 0;             // ultima dimensione letta di coords

    QVector<QVector<QLineEdit*>> cells;
    SudokuSolverAlgorithm*solver;

    unsigned short dim;

    QFont font;

    QWidget*gridWidget;
    QWidget*optionPanel;
    QWidget*numberPad;

    QWidget*setupGrid(const unsigned short&);

    template <class T>
    void resizeGrid(T&);

    QWidget*setupOptionsPanel(const unsigned short&);
    QWidget*setupNumberPad(const unsigned short &);
    QWidget*setupButtons();

    void askResetCells();
    void resetCells();
    void askSolve();
    void solveSequence();
    void startProgressMonitor();

private slots:
    void handleCellInput(int row, int col, const QString &text);

};

#include "resizer.tpp"

#endif // MAINWINDOW_H
