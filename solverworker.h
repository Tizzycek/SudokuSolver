#ifndef SOLVERWORKER_H
#define SOLVERWORKER_H

#pragma once
#include <QObject>

class SudokuSolverAlgorithm;

class SolverWorker : public QObject {
    Q_OBJECT
public:
    SolverWorker(SudokuSolverAlgorithm* solver);

public slots:
    void run();  // eseguito nel thread

signals:
    void finished(bool result);

private:
    SudokuSolverAlgorithm* solver;
};


#endif // SOLVERWORKER_H
