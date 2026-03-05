//
// Created by Tiziano on 05.03.2026.
//

#include "SingleIstance.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

SingleInstance::SingleInstance(const QString &key, QWidget *mainWindow, QObject *parent)
	: QObject(parent)
	, m_key(key)
	, m_sharedMemory(key)
	, m_semaphore(key + "_semaphore", 1)
	, m_mainWindow(mainWindow)
	, m_isPrimary(false)
{
	m_semaphore.acquire();

	if (m_sharedMemory.attach()) {
		// Esiste già un'istanza
		m_semaphore.release();
		m_isPrimary = false;

		// Attiva la finestra esistente
		if (m_mainWindow) {
			activateWindow(m_mainWindow);
		}
	} else {
		// Prima istanza
		if (m_sharedMemory.create(1)) {
			m_isPrimary = true;
		}
		m_semaphore.release();
	}
}

SingleInstance::~SingleInstance()
{
	if (m_isPrimary) {
		m_semaphore.acquire();
		m_sharedMemory.detach();
		m_semaphore.release();
	}
}

void SingleInstance::activateWindow(QWidget *window)
{
	if (!window) return;

	// Porta la finestra in primo piano
	window->raise();
	window->activateWindow();

	// Se minimizzata, ripristina
	if (window->isMinimized()) {
		window->showNormal();
	}

#ifdef Q_OS_WIN
	// Su Windows, forziamo il focus
	HWND hwnd = reinterpret_cast<HWND>(window->winId());
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);
#endif

	qDebug() << "Finestra attivata";
}