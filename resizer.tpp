#ifndef RESIZER_TPP
#define RESIZER_TPP

#include "mainwindow.h"

template <class T>
void MainWindow::resizeGrid(T &widget)
{
    QSize size = widget.size();

    // Troviamo il lato più corto (es. se la finestra è 800x600, il lato è 600)
    int lato = qMin(size.width(), size.height());

    // Calcoliamo quanto spazio vuoto lasciare ai lati per centrare il quadrato
    int margineX = (size.width() - lato) / 2;
    int margineY = (size.height() - lato) / 2;

    // Applichiamo questi margini al layout.
    // Aggiungiamo +10 pixel extra per non far toccare mai il bordo finestra
    QLayout* layout = widget.layout();
    if (layout) {
        // Applichiamo i margini SOLO alla griglia interna.
        // Il pannello delle opzioni sotto non verrà toccato!
        layout->setContentsMargins(margineX, margineY, margineX, margineY);
    }
}

#endif
