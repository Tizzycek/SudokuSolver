#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

/*int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "SudokuSolver_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();
}*/

#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

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
        a.installTranslator(&translator);
    } else {
        qDebug() << "⚠ Nessuna traduzione trovata, usando inglese/italiano di default";
    }

    // 4. Avvia l'applicazione
    MainWindow w;
    w.show();

    return a.exec();
}
