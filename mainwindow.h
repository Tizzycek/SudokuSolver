#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>


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

    QFont font;

    QWidget*gridWidget;
    QWidget*optionPanel;
    QWidget*numberPad;

    QWidget*setupGrid(const unsigned short&);

    template <class T>
    void resizeGrid(T&);

    QWidget*setupOptionsPanel(const unsigned short&);
    QWidget*setupNumberPad(const unsigned short &);
};

#include "resizer.tpp"

#endif // MAINWINDOW_H
