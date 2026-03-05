#include "mainwindow.h"

#include <QLocale>
#include <QApplication>
#include <QTranslator>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

#include "AppManager.h"
#include "SingleIstance.h"

void installLanguage(const QApplication*a) {
    // Nome dell'applicazione (deve coincidere con i file .qm)
    const QString appName = "SudokuSolver";

    QTranslator translator;
    bool translationLoaded = false;

    // 1. Prendi le lingue preferite dal sistema
    const QStringList uiLanguages = QLocale::system().uiLanguages();

    // 2. Cerca nei file .qm ESTERNI (non in risorse!)
    for (const QString &localeName : uiLanguages) {
        // Pulisce la stringa locale (es: "it_IT" o "it-IT" → "it_IT")
        QString locale = QLocale(localeName).name();

        // Costruisce il nome del file
        QString qmFile = QString("%1_%2.qm").arg(appName).arg(locale);

        // Cerca in diversi percorsi (IN ORDINE DI PRIORITÀ):

        // A) Directory dell'eseguibile/translations
        QString appDir = QCoreApplication::applicationDirPath();
        QString path1 = appDir + "/translations/" + qmFile;

        // B) Directory dell'eseguibile
        QString path2 = appDir + "/" + qmFile;

        // C) Directory standard per app data
        QString path3 = QStandardPaths::locate(
            QStandardPaths::AppDataLocation,
            "translations/" + qmFile
        );

        qDebug() << "Cercando traduzione per:" << locale;
        qDebug() << "  Percorso 1:" << path1;
        qDebug() << "  Percorso 2:" << path2;

        // Prova a caricare in ordine
        if (QFile::exists(path1) && translator.load(path1)) {
            qDebug() << "✓ Traduzione caricata da:" << path1;
            translationLoaded = true;
            break;
        }
        else if (QFile::exists(path2) && translator.load(path2)) {
            qDebug() << "✓ Traduzione caricata da:" << path2;
            translationLoaded = true;
            break;
        }
        else if (!path3.isEmpty() && translator.load(path3)) {
            qDebug() << "✓ Traduzione caricata da:" << path3;
            translationLoaded = true;
            break;
        }

        // Prova anche solo con il codice lingua (es: "it" invece di "it_IT")
        if (locale.contains('_')) {
            QString shortLocale = locale.split('_').first();
            QString shortQmFile = QString("%1_%2.qm").arg(appName).arg(shortLocale);
            QString shortPath1 = appDir + "/translations/" + shortQmFile;

            if (QFile::exists(shortPath1) && translator.load(shortPath1)) {
                qDebug() << "✓ Traduzione caricata (solo lingua):" << shortPath1;
                translationLoaded = true;
                break;
            }
        }
    }

    // 3. Installa il traduttore se trovato
    if (translationLoaded) {
        a->installTranslator(&translator);
    } else {
        qDebug() << "⚠ Nessuna traduzione trovata, usando inglese/italiano di default";
    }
}

int main(int argc, char *argv[])
{
    const QApplication a(argc, argv);

    QString appId = "it.tizzycek.sudokusolver";
    a.setApplicationName("SudokuSolver");
    a.setApplicationVersion("1.0");

    installLanguage(&a);

    SingleInstance instance(appId);
    if (!instance.isPrimaryInstance())
        return 0;

    AppManager manager;
    manager.start();

    return QApplication::exec();
}
