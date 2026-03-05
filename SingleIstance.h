//
// Created by Tiziano on 05.03.2026.
//

#ifndef SUDOKUSOLVER_SINGLEISTANCE_H
#define SUDOKUSOLVER_SINGLEISTANCE_H

#include <QSharedMemory>
#include <QSystemSemaphore>
#include <QWidget>

class SingleInstance : public QObject
{
	Q_OBJECT

public:
	explicit SingleInstance(const QString &key, QWidget *mainWindow = nullptr, QObject *parent = nullptr);
	~SingleInstance() override;

	[[nodiscard]] bool isPrimaryInstance() const { return m_isPrimary; }

	static void activateWindow(QWidget *window);

private:
	QString m_key;
	QSharedMemory m_sharedMemory;
	QSystemSemaphore m_semaphore;
	QWidget *m_mainWindow;
	bool m_isPrimary;
};


#endif //SUDOKUSOLVER_SINGLEISTANCE_H