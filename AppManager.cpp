//
// Created by Tiziano on 05.03.2026.
//

#include "AppManager.h"

#include <QApplication>

AppManager::AppManager(QObject *parent) : QObject(parent), m_isStartingUp(true) {

}

void AppManager::start() {
	showModeSelection();
}

/*
 * Crea il primo dialog, lo mostra e associa i pulsanti OK/annulla
 */
void AppManager::showModeSelection() {
	cleanup();

	m_startupDialog = new StartupDialog();

	connect(m_startupDialog, &QDialog::accept, this, &AppManager::onModeSelected);
	connect(m_startupDialog, &QDialog::rejected, this, &AppManager::onModeSelectionCancelled);

	m_startupDialog->show();
}

/*
 * Dialog OK: lo elimina e chiama il crea finestra col suo risultato
 */
void AppManager::onModeSelected() {
	int mode = m_startupDialog->getSelectedMode();

	m_startupDialog->deleteLater();
	m_startupDialog = nullptr;

	createMainWindow(mode);
}

/*
 Dialog annulla: lo elimina ed esce se l'applicazione è in fase di avvio
 */
void AppManager::onModeSelectionCancelled() {
	m_startupDialog->deleteLater();
	m_startupDialog = nullptr;

	if (m_isStartingUp)
		QApplication::quit();
}

/*
 * Crea la nuova finestra. Bisogna garantire che le finestre vengano distrutte correttamente.
 * Viene inizializzata e mostrata.
 * Lo startingup diventa falso.
 */
void AppManager::createMainWindow(const int & mode) {
	m_mainWindow = new MainWindow();
	m_mainWindow->setAttribute(Qt::WA_DeleteOnClose);

	connect(m_mainWindow, &QMainWindow::destroyed, this, &AppManager::onMainWindowClosed);

	m_mainWindow->initializeForMode(mode);
	m_mainWindow->show();

	m_isStartingUp = false;
}

void AppManager::onMainWindowClosed() {
	m_mainWindow = nullptr;
}

/*
 Pulizia: chiudi, elimina e metti null
 */
void AppManager::cleanup() {
	if (m_mainWindow) {
		m_mainWindow->close();
		m_mainWindow->deleteLater();
		m_mainWindow = nullptr;
	}
}
