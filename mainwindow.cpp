#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QMessageBox>
#include <QResizeEvent> // Necessario per usare QResizeEvent nel costruttore

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , solver(nullptr)
{
    ui->setupUi(this);

    setWindowTitle(tr("Sudoku"));
    setMinimumSize(600, 700);
    resize(600, 700);

    font = QFont("Arial", 20, QFont::Bold);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    dim = 9;

    solver = new SudokuSolverAlgorithm(dim);

    cells.resize(dim);
    for(int i=0; i<dim; ++i) cells[i].resize(dim);

    gridWidget = setupGrid(dim);
    optionPanel = setupOptionsPanel(dim);

    // Griglia sopra (4 parti), Opzioni sotto (2 parti)
    mainLayout->addWidget(gridWidget, 4);
    mainLayout->addWidget(optionPanel, 2);

    // Usiamo QTimer per forzare un aggiornamento del layout all'avvio
    QTimer::singleShot(0, this, [this](){
        // Invece di chiamare resizeGrid, lanciamo un evento di resize
        // così la logica centralizzata in resizeEvent viene eseguita.
        QResizeEvent *event = new QResizeEvent(this->size(), QSize());
        QCoreApplication::postEvent(this, event);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
            if (solver) {
                // Controllo per evitare crash se il testo è vuoto o solver non inizializzato
                // (Anche se all'inizio è vuoto, è buona norma)
                unsigned short val = cell->text().toUShort();
                if(val > 0) solver->insert(val, row, col);
            }
        }
    }
    return gridPanel;
}

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

QWidget* MainWindow::setupNumberPad(const unsigned short &size)
{
    QWidget* padContainer = new QWidget();
    padContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QGridLayout* padLayout = new QGridLayout(padContainer);
    padLayout->setSpacing(0);
    padLayout->setContentsMargins(0, 0, 0, 0);

    for (unsigned short i = 0; i < size; ++i) {
        padLayout->setRowStretch(i, 1);
        padLayout->setColumnStretch(i, 1);
    }

    for (unsigned short i = 1; i <= size; ++i) {
        QPushButton *numBtn = new QPushButton(QString::number(i));
        numBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        numBtn->setMinimumSize(30, 30);
        numBtn->setFont(font);
        padLayout->addWidget(numBtn, (i-1)/3, (i-1)%3);
    }
    return padContainer;
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

QWidget* MainWindow::setupButtons(){
    QWidget* btnContainer = new QWidget();

    QVBoxLayout* btnLayout = new QVBoxLayout(btnContainer);

    // RIMOSSO: btnLayout->setAlignment(Qt::AlignCenter);
    // Togliendo l'allineamento forzato, i pulsanti si espanderanno orizzontalmente
    // per riempire tutto lo spazio concesso dal layout padre (che è limitato dai margini).

    QPushButton* reset = new QPushButton("Ripristina");
    reset->setFont(QFont("Arial", 14));
    reset->setMinimumHeight(50);
    reset->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); // Espanditi in larghezza

    btnLayout->addWidget(reset);
    connect(reset, &QPushButton::clicked, this, &MainWindow::askResetCells);

    QPushButton* solve = new QPushButton("Risolvi");
    solve->setFont(QFont("Arial", 14));
    solve->setMinimumHeight(50);
    solve->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    btnLayout->addWidget(solve);
    connect(solve, &QPushButton::clicked, this, &MainWindow::askSolve);

    return btnContainer;
}

void MainWindow::askResetCells(){
    QMessageBox::StandardButton reply = QMessageBox::warning(this, "Attenzione",
                                                             "Ripristinare tutta la griglia?",
                                                             QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        resetCells();
    }
}

void MainWindow::resetCells(){
    for (unsigned short i = 0; i < dim; i++){
        for (unsigned short j = 0; j < dim; j++) {
            if(cells[i][j]) {
                cells[i][j]->clear();
            }
        }
    }
}

void MainWindow::askSolve(){
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Risolvi",
                                                              "Risolvere il sudoku?",
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Error");
        msgBox.setText("To be implemented!");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.button(QMessageBox::Ok)->setText("Abort");
        msgBox.exec();
        // abort(); // Commentato perché abort() chiude brutalmente il programma
    }
}
