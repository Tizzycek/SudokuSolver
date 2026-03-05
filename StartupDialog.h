//
// Created by Tiziano on 05.03.2026.
//

#ifndef SUDOKUSOLVER_STARTUPDIALOG_H
#define SUDOKUSOLVER_STARTUPDIALOG_H

#include <QDialog>

class QButtonGroup;

class StartupDialog : public QDialog
{
	Q_OBJECT

public:
	explicit StartupDialog(QWidget *parent = nullptr);
	int getSelectedMode() const;

private:
	QButtonGroup *radioGroup;
};

#endif //SUDOKUSOLVER_STARTUPDIALOG_H