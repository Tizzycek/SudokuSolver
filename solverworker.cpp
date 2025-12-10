#include "solverworker.h"
#include "libs/SudokuSolverAlgorithm.h"

SolverWorker::SolverWorker(SudokuSolverAlgorithm* s)
    : solver(s)
{
}

void SolverWorker::run()
{
    bool ok = solver->solve();
    emit finished(ok);
}
