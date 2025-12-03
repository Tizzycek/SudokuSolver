#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("Sudoku"));
    setMinimumSize(500, 600);
    resize(500, 600);

    font = QFont("Arial", 20, QFont::Bold);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    // Rimuoviamo i margini del layout principale per sfruttare tutto lo spazio
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    unsigned short dim = 9;
    gridWidget = setupGrid(dim);
    optionPanel = setupOptionsPanel(dim);

    mainLayout->addWidget(gridWidget, 4);
    mainLayout->addWidget(optionPanel, 1);

    QTimer::singleShot(0, this, [this](){
        this->resizeGrid(*gridWidget);
        this->resizeGrid(*numberPad);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget*MainWindow::setupGrid(const unsigned short &size){
    QWidget*gridPanel = new QWidget(this);
    QGridLayout*sudokuGrid = new QGridLayout(gridPanel);

    sudokuGrid->setSpacing(0);
    sudokuGrid->setContentsMargins(0, 0, 0, 0);
    //sudokuGrid->setContentsMargins(20, 20, 20, 20);
    //sudokuGrid->setHorizontalSpacing(10);
    //sudokuGrid->setVerticalSpacing(10);

    for (unsigned short i = 0; i < size; ++i) {
        sudokuGrid->setRowStretch(i, 1);
        sudokuGrid->setColumnStretch(i, 1);
    }

    QRegularExpression rx("^[1-9]$");
    QValidator *validator = new QRegularExpressionValidator(rx);

    for (unsigned short row = 0; row < size; row++){
        for (unsigned short col = 0; col <  size; col++){
            QLineEdit *cell = new QLineEdit;

            cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            cell->setMinimumSize(30, 30);

            cell->setAlignment(Qt::AlignCenter);
            cell->setMaxLength(1);
            cell->setFont(font);
            cell->setValidator(validator);


            //int temp[size][size];

            QString style = "QLineEdit { "
                            "background-color: white; "
                            "border: 1px solid #c0c0c0; "
                            "color: #333; "
                            "}"
                            "QLineEdit:focus {"
                            "    border: 2px solid #0078d4;"
                            "}";

            if (col % 3 == 2)
                style.append("QLineEdit { border-right: 2px solid black; }");
            if (row % 3 == 2)
                style.append("QLineEdit { border-bottom: 2px solid black; }");

            cell->setStyleSheet(style);
            sudokuGrid->addWidget(cell, row, col);
            // IMPLEMENTARE E CONTROLLARE
            //temp[row][col] = cell->text().toInt();
        }
    }

    return gridPanel;
}

void MainWindow::resizeEvent(QResizeEvent *event)  {
    // Chiamiamo l'implementazione base per assicurarci che il widget centrale sia aggiornato
    QMainWindow::resizeEvent(event);

    if (!gridWidget || !numberPad) return;

    // Otteniamo le dimensioni attuali dell'area di lavoro
    resizeGrid(*gridWidget);
    resizeGrid(*numberPad);
}

// Cambiamo il tipo di ritorno da QGridLayout* a QWidget*
QWidget* MainWindow::setupNumberPad(const unsigned short &size)
{
    // Creiamo un widget che farà da contenitore (Wrapper)
    QWidget* padContainer = new QWidget();

    // Creiamo il layout e lo assegniamo al contenitore
    QGridLayout* padLayout = new QGridLayout(padContainer);

    padLayout->setSpacing(0);
    // Fondamentale: margini a 0 così resizeGrid ha il controllo totale
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

        // Aggiungiamo al layout (che è dentro padContainer)
        padLayout->addWidget(numBtn, (i-1)/3, (i-1)%3);
    }

    // Restituiamo il contenitore, non il layout
    return padContainer;
}

QWidget* MainWindow::setupOptionsPanel(const unsigned short &size){
    QWidget* lowPanel = new QWidget(this);
    QVBoxLayout* lowLayout = new QVBoxLayout(lowPanel);

    // Allineamento e margini per il pannello opzioni
    lowLayout->setContentsMargins(0, 10, 0, 10);
    lowLayout->setAlignment(Qt::AlignCenter);

    // numberPad ora è un QWidget*, creato dalla funzione modificata
    numberPad = setupNumberPad(size);

    // Usiamo addWidget invece di addLayout
    lowLayout->addWidget(numberPad);

    return lowPanel;
}
