#include "mainwindow.h"
#include "solverworker.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QMessageBox>
#include <QResizeEvent>
#include <QThread>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , solver(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("SudokuSolver");
    setMinimumSize(600, 700);
    resize(600, 700);

    font = QFont("Arial", 22, QFont::Bold);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    dim = 9;

    solver = new SudokuSolverAlgorithm(dim);

    cells.resize(dim);
    for(unsigned short i=0; i<dim; ++i) cells[i].resize(dim);

    gridWidget = setupGrid(dim);
    optionPanel = setupOptionsPanel(dim);

    // Griglia sopra (4 parti), Opzioni sotto (2 parti)
    mainLayout->addWidget(gridWidget, 4);
    mainLayout->addWidget(optionPanel, 2);

    // Usiamo QTimer per forzare un aggiornamento del layout all'avvio
    QTimer::singleShot(0, this, [this](){
        // Invece di chiamare resizeGrid, lanciamo un evento di resize
        // così la logica centralizzata in resizeEvent viene eseguita.
        auto *event = new QResizeEvent(this->size(), QSize());
        QCoreApplication::postEvent(this, event);
    });
}

MainWindow::~MainWindow()
{
    delete solver;
    delete ui;
}

// Ensure background thread is stopped on window close
void MainWindow::closeEvent(QCloseEvent* event)
{
    if (solverThread) {
        // Request the worker thread event loop to stop and wait for it
        if (solverThread->isRunning()) {
            solverThread->quit();
            solverThread->wait();
        }
        solverThread->deleteLater();
        solverThread = nullptr;
    }
    QMainWindow::closeEvent(event);
}

// === Overridden Qt events ===
// --- QUESTA È LA FUNZIONE CHIAVE PER L'ALLINEAMENTO ---
void MainWindow::resizeEvent(QResizeEvent *event)  {
    QMainWindow::resizeEvent(event);
    if (!gridWidget || !optionPanel) return;

    // 1. Calcoliamo la geometria quadrata basandoci SOLO sulla griglia (Master)
    QSize gSize = gridWidget->size();
    int lato = qMin(gSize.width(), gSize.height());

    // Calcoliamo i margini necessari per centrare il quadrato
    int margineX = (gSize.width() - lato) / 2;
    int margineY = (gSize.height() - lato) / 2;

    // 2. Applichiamo i margini alla GRIGLIA (per farla quadrata)
    if (gridWidget->layout()) {
        gridWidget->layout()->setContentsMargins(margineX, margineY, margineX, margineY);
    }

    // 3. Applichiamo gli STESSI margini orizzontali al PANNELLO OPZIONI
    // In questo modo, il pannello sotto inizia e finisce esattamente dove finisce la griglia sopra.
    // I margini verticali (10, 10) li teniamo fissi per estetica.
    if (optionPanel->layout()) {
        optionPanel->layout()->setContentsMargins(margineX, 10, margineX, 10);
    }
}

// --- Funzione per tracciare la cella selezionata ---
bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::FocusIn) {
        if (auto *edit = qobject_cast<QLineEdit*>(watched)) {
            selectedCell = edit;
        }
        return false;
    }

    if (event->type() == QEvent::KeyPress) {

        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        QLineEdit* edit = qobject_cast<QLineEdit*>(watched);

        if (!edit) return false;

        // Quando la cella riceve il focus (es. click o tab), la ricordiamo
        if (event->type() == QEvent::FocusIn) {
            selectedCell = edit;
            return QMainWindow::eventFilter(watched, event); // lascia comportamento di default
        }

        // Quando la cella viene cliccata con il mouse, impostiamo selectedCell (ridondante con FocusIn)
        if (event->type() == QEvent::MouseButtonPress) {
            selectedCell = edit;
            return QMainWindow::eventFilter(watched, event);
        }

        int r = edit->property("row").toInt();
        int c = edit->property("col").toInt();

        selectedCell = edit;

        switch (keyEvent->key()) {

        case Qt::Key_Left:
            if (c > 0){
                selectedCell = cells[r][c-1];
                cells[r][c-1]->setFocus();
            }
            return true;

        case Qt::Key_Right:
            if (c < dim-1){
                cells[r][c+1]->setFocus();
                selectedCell = cells[r][c+1];
            }
            return true;

        case Qt::Key_Up:
            if (r > 0){
                cells[r-1][c]->setFocus();
                selectedCell = cells[r-1][c];
            }
            return true;

        case Qt::Key_Down:
            if (r < dim-1){
                cells[r+1][c]->setFocus();
                selectedCell = cells[r+1][c];
            }
            return true;

        default:
            return false;
        }
    }

    return false;
}

// === Slots ===
// --- GESTIONE INPUT DA TASTIERA ---
void MainWindow::handleCellInput(unsigned short row, unsigned short col, const QString &text) {
    unsigned short val = text.isEmpty() ? 0 : text.toUShort();

    if (val == 0) {
        solver->clean(row, col);
    } else {
        if (solver->isSafe(row, col, val)){
            solver->insert(val, row, col);
            cells[row][col]->setStyleSheet(cells[row][col]->styleSheet() + "color: black;"); // Colore per input utente
        } else {
            QMessageBox::warning(this, tr("Errore"), QString(tr("Il numero %1 non è valido in posizione (%2, %3).")).arg(val).arg(row+1).arg(col+1));
            cells[row][col]->clear();
        }
    }
}

// === UI setup helpers ===
QWidget* MainWindow::setupGrid(const unsigned short &size){
    QWidget* gridPanel = new QWidget(this);
    QGridLayout* sudokuGrid = new QGridLayout(gridPanel);

    sudokuGrid->setSpacing(0);
    sudokuGrid->setContentsMargins(0, 0, 0, 0);

    for (unsigned short i = 0; i < size; ++i) {
        sudokuGrid->setRowStretch(i, 1);
        sudokuGrid->setColumnStretch(i, 1);
    }

    QRegularExpression rx("^[1-9]$");
    QValidator *validator = new QRegularExpressionValidator(rx, this);

    for (unsigned short row = 0; row < size; row++){
        for (unsigned short col = 0; col < size; col++){
            QLineEdit *cell = new QLineEdit;

            cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cell->setMinimumSize(30, 30);
            cell->setAlignment(Qt::AlignCenter);
            cell->setMaxLength(1);
            cell->setFont(font);
            cell->setValidator(validator);

            cell->setProperty("row", row);
            cell->setProperty("col", col);

            // --- CONNESSIONE PER INPUT DA TASTIERA ---
            // Quando il testo è modificato (dopo la validazione), chiamiamo la funzione di gestione
            connect(cell, &QLineEdit::textEdited, this, [this, row, col](const QString &text) {
                this->handleCellInput(row, col, text);
            });

            // --- CONNESSIONE PER TRACCIARE LA SELEZIONE ---
            // Quando la cella ottiene il focus
            connect(cell, &QLineEdit::editingFinished, this, [](){
                // Usiamo editingFinished come un modo per reagire dopo che la cella perde il focus,
                // ma per tracciare la selezione usiamo focusInEvent se fosse disponibile.
                // Per la semplicità di un QLineEdit, basta tracciare quale ha il focus.
            });

            connect(cell, &QLineEdit::textChanged, this, [this, row, col](const QString &text){});
            // Per tracciare la cella selezionata, usiamo un Event Filter (vedi nota sotto)
            // o sovrascriviamo l'evento, ma qui usiamo l'event filter più semplice.
            cell->installEventFilter(this);

            QString style = "QLineEdit { "
                            "background-color: white; "
                            "border: 1px solid #c0c0c0; "
                            "color: #333; "
                            "}"
                            "QLineEdit:focus {"
                            "    border: 2px solid #0078d4;"
                            "    background-color: #e3f2fd;"
                            "}";

            if (col % 3 == 2) style.append("QLineEdit { border-right: 2px solid black; }");
            if (row % 3 == 2) style.append("QLineEdit { border-bottom: 2px solid black; }");

            cell->setStyleSheet(style);
            sudokuGrid->addWidget(cell, row, col);

            cells[row][col] = cell;

            solver->insert(cell->text().toUShort(), row, col);
        }
    }
    return gridPanel;
}

QWidget* MainWindow::setupOptionsPanel(const unsigned short &size){
    QWidget* lowPanel = new QWidget(this);

    QHBoxLayout* lowLayout = new QHBoxLayout(lowPanel);

    // I margini verranno sovrascritti da resizeEvent, ma ne impostiamo uno base
    lowLayout->setContentsMargins(10, 10, 10, 10);
    lowLayout->setSpacing(20);

    numberPad = setupNumberPad(size);

    // Stretch 1 per il tastierino
    lowLayout->addWidget(numberPad, 1);

    QWidget* options = setupButtons();

    // Stretch 1 per i bottoni
    lowLayout->addWidget(options, 1);

    return lowPanel;
}

QWidget* MainWindow::setupNumberPad(const unsigned short &size)
{
    QWidget* padContainer = new QWidget();
    padContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGridLayout* padLayout = new QGridLayout(padContainer);
    padLayout->setSpacing(0);
    padLayout->setContentsMargins(0, 0, 0, 0);

    for (unsigned short i = 0; i < std::sqrt(size); ++i) {
        padLayout->setRowStretch(i, 1);
        padLayout->setColumnStretch(i, 1);
    }

    for (unsigned short i = 1; i <= size; ++i) {
        QPushButton *numBtn = new QPushButton(QString::number(i));
        connect(numBtn, &QPushButton::clicked, this, [this, i]() {
            this->handleNumberPadInput(i);
        });
        numBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        numBtn->setMinimumSize(30, 30);
        //numBtn->setFont(font);
        numBtn->setFont(QFont("Arial", 25, QFont::Bold));
        padLayout->addWidget(numBtn, (i-1)/3, (i-1)%3);
    }
    return padContainer;
}

QWidget* MainWindow::setupButtons(){
    QWidget* btnContainer = new QWidget();

    QVBoxLayout* btnLayout = new QVBoxLayout(btnContainer);

    QPushButton* reset = new QPushButton(tr("Ripristina"));
    reset->setFont(QFont("Arial", 14));
    reset->setMinimumHeight(50);
    reset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Espanditi in larghezza

    btnLayout->addWidget(reset);
    connect(reset, &QPushButton::clicked, this, &MainWindow::askResetCells);

    QPushButton* solve = new QPushButton(tr("Risolvi"));
    solve->setFont(QFont("Arial", 14));
    solve->setMinimumHeight(50);
    solve->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    btnLayout->addWidget(solve);
    connect(solve, &QPushButton::clicked, this, &MainWindow::askSolve);

    return btnContainer;
}

// === Actions / handlers ===
void MainWindow::askResetCells(){
    QMessageBox::StandardButton reply = QMessageBox::warning(this, tr("Attenzione"),
                                                             tr("Ripristinare tutta la griglia?"),
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        resetCells();
    }
}

void MainWindow::resetCells(){

    for (unsigned short i = 0; i < dim; i++){
        for (unsigned short j = 0; j < dim; j++) {
            QString style = "QLineEdit { "
                            "background-color: white; "
                            "border: 1px solid #c0c0c0; "
                            "color: #333; "
                            "}"
                            "QLineEdit:focus {"
                            "    border: 2px solid #0078d4;"
                            "    background-color: #e3f2fd;"
                            "}";
            if(cells[i][j]) {
                cells[i][j]->clear();
                if (j % 3 == 2) style.append("QLineEdit { border-right: 2px solid black; }");
                if (i % 3 == 2) style.append("QLineEdit { border-bottom: 2px solid black; }");
                cells[i][j]->setStyleSheet(style);
                if (cells[i][j]->isReadOnly())
                    cells[i][j]->setReadOnly(false);
            }
        }
    }
    solver->clean();
}

void MainWindow::askSolve(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, tr("Risolvi"),
                                                              tr("Risolvere il sudoku?"),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        solveSequence();
    }
}

void MainWindow::solveSequence()
{
    QString finalStyle = "QLineEdit { "
        "background-color: #0ABAB5; "
        "border: 1px solid #c0c0c0; "
        "color: #333; "
        "}"
        "QLineEdit:focus {"
        "    border: 2px solid #0078d4;"
        "    background-color: #e3f2fd;"
        "}";
    // Prepare progress tracking for a new run
    lastCoordsSize = 0;
    if (solver) solver->clearProgress();

    for (unsigned short i = 0; i < dim; i++)
        for (unsigned short j = 0; j < dim; j++)
            cells[i][j]->setReadOnly(true);

    QThread* thread = new QThread;
    SolverWorker* worker = new SolverWorker(solver);

    worker->moveToThread(thread);

    // Ensure cleanup regardless of success path
    connect(thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    connect(thread, &QThread::started, worker, &SolverWorker::run);

    // quando la solve termina
    connect(worker, &SolverWorker::finished,
            this, [=](bool ok){
                qDebug() << "Solver terminato, risultato:" << ok;

                // aggiorna griglia completa
                for (unsigned short r = 0; r < dim; ++r){
                    for (unsigned short c = 0; c < dim; ++c){
                        cells[r][c]->setText(QString::number(solver->get(r, c)));
                        //cells[r][c]->setStyleSheet(finalStyle);
                    }
                }

                // sblocca GUI
                // si sblocca solo dopo reset

                // pulizia thread/worker
                thread->quit();
                thread->wait();

                worker->deleteLater();
                thread->deleteLater();

                // reset stato in MainWindow
                solverThread = nullptr;
                lastCoordsSize = 0;

                if (ok)
                    QMessageBox::information(this, tr("Completato"), tr("Sudoku risolto"));
                else
                    QMessageBox::critical(this, tr("Errore"), tr("Il sudoku non è stato risolto"));
            });

    // salva il thread nel membro e avvialo
    solverThread = thread;
    thread->start();

    // avvia monitor (usa lastCoordsSize membro, viene azzerato all'inizio)
    startProgressMonitor();
}

void MainWindow::startProgressMonitor()
{
    // usa il membro lastCoordsSize (non statico), così si resetta tra esecuzioni
    QTimer* timer = new QTimer(this);

    /*QString solvedColor = "QLineEdit { "
                          "background-color: #FFD16E; "
                          "border: 1px solid #c0c0c0; "
                          "color: #333; "
                          "}"
                          "QLineEdit:focus {"
                          "    border: 2px solid #0078d4;"
                          "    background-color: #e3f2fd;"
                          "}";*/

    connect(timer, &QTimer::timeout, this, [this, timer/*, solvedColor*/]() {
        size_t size = solver->coordsSize();


        if (size > lastCoordsSize) {
            for (unsigned short k = lastCoordsSize; k < size; ++k) {
                auto [r, c] = solver->coordAt(k);
                unsigned short v = solver->get(r, c);
                cells[r][c]->setText(QString::number(v));
                //cells[r][c]->setStyleSheet(solvedColor);
                qDebug() << "Aggiornata cella:" << r << c << " = " << v;
            }
            lastCoordsSize = size;
        }

        // Se il thread è nullo o non è in esecuzione, fermiamo il timer
        if (!solverThread || !solverThread->isRunning()) {
            timer->stop();
            timer->deleteLater();
        }
    });

    timer->start(100); // ogni 100 ms (regola come preferisci)
}

void MainWindow::handleNumberPadInput(unsigned short val)
{
    if (!selectedCell) return;

    unsigned short row = selectedCell->property("row").toInt();
    unsigned short col = selectedCell->property("col").toInt();

    if (!solver->isSafe(row, col, val)) {
        QMessageBox::warning(this, tr("Errore"),
                             tr("Il numero %1 non è valido in posizione (%2, %3).")
                                 .arg(val).arg(row+1).arg(col+1));
        return;
    }

    selectedCell->setText(QString::number(val));
    solver->insert(val, row, col);

    // opzionale: focus avanti
    int nc = col + 1;
    int nr = row;
    if (nc >= dim) { nc = 0; nr++; }
    if (nr < dim) cells[nr][nc]->setFocus();
}

