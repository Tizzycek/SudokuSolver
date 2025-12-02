#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>
#include <QRegularExpressionValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("Sudoku"));
    setMinimumSize(500, 500);
    resize(500, 500);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    gridWidget = setupGrid(9);

    mainLayout->addWidget(gridWidget);
    mainLayout->addWidget(setupOptionsPanel());
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget*MainWindow::setupGrid(unsigned short size){
    QWidget*gridPanel = new QWidget(this);
    QGridLayout*sudokuGrid = new QGridLayout(gridPanel);

    sudokuGrid->setSpacing(0);
    //sudokuGrid->setContentsMargins(20, 20, 20, 20);
    //sudokuGrid->setHorizontalSpacing(10);
    //sudokuGrid->setVerticalSpacing(10);

    for (int i = 0; i < 9; ++i) {
        sudokuGrid->setRowStretch(i, 1);
        sudokuGrid->setColumnStretch(i, 1);
    }

    QFont font("Arial", 20, QFont::Bold);

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


            int temp[size][size];

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
            temp[row][col] = cell->text().toInt();
        }
    }

    return gridPanel;
}

void MainWindow::resizeEvent(QResizeEvent *event)  {
    // Chiamiamo l'implementazione base per assicurarci che il widget centrale sia aggiornato
    QMainWindow::resizeEvent(event);

    if (!gridWidget) return;

    // Otteniamo le dimensioni attuali dell'area di lavoro
    QSize size = gridWidget->size();

    // Troviamo il lato più corto (es. se la finestra è 800x600, il lato è 600)
    int lato = qMin(size.width(), size.height());

    // Calcoliamo quanto spazio vuoto lasciare ai lati per centrare il quadrato
    int margineX = (size.width() - lato) / 2;
    int margineY = (size.height() - lato) / 2;

    // Applichiamo questi margini al layout.
    // Aggiungiamo +10 pixel extra per non far toccare mai il bordo finestra
    mainLayout->setContentsMargins(margineX + 10, margineY + 10, margineX + 10, margineY + 10);
}

QWidget*MainWindow::setupOptionsPanel(){
    QWidget*lowPanel = new QWidget(this);
    QVBoxLayout*lowLayout = new QVBoxLayout(lowPanel);

    QGridLayout *numberPad = new QGridLayout;
    for (int i = 1; i <= 9; ++i) {
        QPushButton *numBtn = new QPushButton(QString::number(i));
        numBtn->setFixedSize(40, 40);
        numberPad->addWidget(numBtn, (i-1)/3, (i-1)%3);
    }

    lowLayout->addLayout(numberPad);

    return lowPanel;
}
