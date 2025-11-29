#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Sudoku");
    setMinimumSize(800, 600);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(setupGrid(9));
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
    sudokuGrid->setContentsMargins(20, 20, 20, 20);

    for (unsigned short row = 0; row < size; row++){
        for (unsigned short col = 0; col <  size; col++){
            QLineEdit *cell = new QLineEdit;
            cell->setAlignment(Qt::AlignCenter);
            cell->setMaxLength(1);
            cell->setFixedSize(40, 40);
            int temp[size][size];

            cell->setStyleSheet(
                    "QLineEdit {"
                    "    border: 1px solid #ccc;"
                    "    font-size: 18px;"
                    "    font-weight: bold;"
                    "    border-radius: 8px;"
                    //"    background-color: white;"
                    "}"
                    "QLineEdit:focus {"
                    //"    background-color: #e6f3ff;"
                    "    border: 2px solid #0078d4;"
                    "}"
                );

            sudokuGrid->addWidget(cell, row, col);
            // IMPLEMENTARE E CONTROLLARE
            temp[row][col] = cell->text().toInt();
        }
    }

    return gridPanel;
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
