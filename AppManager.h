//
// Created by Tiziano on 05.03.2026.
//

#ifndef SUDOKUSOLVER_APPMANAGER_H
#define SUDOKUSOLVER_APPMANAGER_H

#include <QPointer>
#include "mainwindow.h"
#include "startupdialog.h"

class AppManager : public QObject
{
	Q_OBJECT

public:
	explicit AppManager(QObject *parent = nullptr);

	void start();

	void showModeSelection();

private slots:
	void onMainWindowClosed();
	void onModeSelected();
	void onModeSelectionCancelled();

private:
	void createMainWindow(const int & mode);
	void cleanup();

	QPointer<MainWindow> m_mainWindow;
	QPointer<StartupDialog> m_startupDialog;
	bool m_isStartingUp;
};


#endif //SUDOKUSOLVER_APPMANAGER_H