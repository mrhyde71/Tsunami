#include "mainwindow.h"

#include <QApplication>
#include <QSplashScreen>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QStandardPaths>

#include <mutex> // needed for std::call_once, std::once_flag

#include <src/tsumanager.h>
#include <src/settingswindow.h>
#include <src/updatemanager.h>

#include <src/tsuCrawler/tsuprovider.h>
#include <src/runGuard/runguard.h>


QString getDefaultLogDirName()
{
    // return default directory to use to store the log files.
    // Even if currently Tsunami should be used by one user at time (=not allowed multiple instances launched by different users)
    // I strongly suggest to use a directory belonging to the launching user (this for reasons related to privacy: also names of
    // downloaded files can be logged)


#if defined(Q_OS_WIN32)

// proposal: return the User's AppData folder
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#else
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation);
#endif

    // for the moment keep current behaviour
    // return QString();

#elif defined(Q_OS_MAC)
    // Impose a folder for log files
    // On MacOS usually there is ~/Library/Logs/APPNAME, others use also ~/Library/Application Support/APPNAME
    // I've chosen the first because MacOS has a log viewer which looks for log files just in ~/Library/Logs/
    return (QString("%0/Library/Logs/Tsunami").arg(QDir::home().path()));

#elif defined(Q_OS_LINUX)
    // Proposal: return ~/.Tsunami
    return (QString("%0/.Tsunami").arg(QDir::home().path()));

    // for the moment keep current behaviour
    // return QString();

#else
    // in all other cases return empty string (caller in this case will use the current directory)
    return QString();
#endif
}

void initLog(QString& log_full_path)
{
    static const QString DEFAULT_LOG_NAME("Tsunami.log");

    QFile logFile(DEFAULT_LOG_NAME);

    QString logDirName = getDefaultLogDirName();
    if (!logDirName.isEmpty()) {
        // make sure that dir exists, if not let's create it
        QDir logDir(logDirName);
        if (!logDir.exists()) {
            logDir.mkpath(".");
        }
        logFile.setFileName(logDir.filePath(DEFAULT_LOG_NAME));
    }

    log_full_path = logFile.fileName();
}


void logMessageHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    static std::once_flag gotLogFileName{};
    static QString        logFileName{};

    std::call_once(gotLogFileName, initLog, logFileName);


    QString txt;

    QString txtContext;
    QString funcName = QString::fromUtf8(context.function);
    QString message(msg);

    // strip out all chars after '(' like '(class QWidget *)' from function name
    funcName = funcName.left(funcName.indexOf("("));

    // strip out all chars like 'void __cdecl ' before function name
    funcName = funcName.right(funcName.length() - funcName.lastIndexOf(" ") - 1);

    // strip out double quotes around string message (if present)
    if (message.left(1) == "\"") {
        message = message.right(message.length() - 1);

        if (message.right(1) == "\"") {
            message = message.left(message.length() - 1);
        }
    }

    txtContext = QString("%0::%1").arg(funcName).arg(context.line);

    //if (QLibraryInfo::isDebugBuild()) {
    //    txtContext = QString("%0::%1::%2").arg(context.file).arg(context.function).arg(context.line);
    //}

    QString date = QDateTime::currentDateTime().toString(Qt::SystemLocaleDate);

    QSettings settings(qApp->property("iniFilePath").toString(), QSettings::IniFormat);
    int debugLevel = settings.value("Debug/Level", 1).toInt();

    bool shouldLog = false;
    switch (debugLevel) {
    case 0: // debug
        shouldLog = true;
        break;
    case 1: // info
        if (type != QtDebugMsg)
            shouldLog = true;
        break;
    case 2: // warning
        if (type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg)
            shouldLog = true;
        break;
    case 3: // critical
        if (type == QtCriticalMsg || type == QtFatalMsg)
            shouldLog = true;
        break;
    case 4: // fatal
        if (type == QtFatalMsg)
            shouldLog = true;
        break;
    default:
        // info
        shouldLog = true;
        break;
    }

    switch (type) {
    case QtDebugMsg:
        txt = "Debug   ";
        break;
    case QtInfoMsg:
        txt = "Info    ";
        break;
    case QtWarningMsg:
        txt = "Warning ";
        break;
    case QtCriticalMsg:
        txt = "Critical";
        break;
    case QtFatalMsg:
        txt = "Fatal   ";
        break;
    }

    if (shouldLog) {
        txt = QString("%0 - %1 - %2 -> %3").arg(date).arg(txt).arg(txtContext).arg(message);
        
        QFile outFile(logFileName);

        outFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
        QTextStream ts(&outFile);
        // it is not rare to have UTF-8 characters in file nmaes, so try to use UTF-8
        ts.setCodec("UTF-8");
        ts << txt << endl;
        QTextStream out(stdout);
        // it is not rare to have UTF-8 characters in file nmaes, so try to use UTF-8
        out.setCodec("UTF-8");
        out << txt << endl;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // exit if already running
    runGuard guard("singleTsunamiInstance");
    if ( !guard.tryToRun() ) {
        QMessageBox msgBox;
        msgBox.setText("Tsunami is already running");
        msgBox.exec();
        return 0;
    }

    qRegisterMetaType<tsuEvents::tsuEvent>();
    qRegisterMetaType<std::string>();
    qRegisterMetaType<QVector<tsuEvents::tsuEvent>>();
    qRegisterMetaType<QPair<int,int>>();

    qRegisterMetaType<tsuProvider::categories>();
    qRegisterMetaType<tsuProvider::sortRules>();
    qRegisterMetaType<tsuProvider::searchItem>();

    // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output
    qInstallMessageHandler(logMessageHandler);

    QApplication::setOrganizationDomain(QStringLiteral(APP_ORGANIZATION_DOMAIN));
    QApplication::setApplicationName(QStringLiteral(APP_PROJECT_NAME));

    QString filePath = QString("%0/%1.ini").arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation))
                                           .arg(QStringLiteral(APP_PROJECT_NAME));
    filePath = QDir::toNativeSeparators(filePath);
    a.setProperty("iniFilePath", filePath); // QApplication property are application wide qApp->property("")

    QPointer<updatemanager> um = new updatemanager();

    qDebug("checking for update");
    um->checkUpdate();

    while (!um->isFinished()) {
        a.processEvents();
    }

    if (um->appNeedRestart()) {
        QSettings settings(filePath, QSettings::IniFormat);
        settings.setValue("justUpdated", true);
        qDebug("restarting");

        // due to squirrel standard behaviour, qApp->arguments()[0] returs path to a tsunami version older than one just downloaded
        // retrieve path of tsunami.exe loader outside current path and launch
        QString fileName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        QDir oldPath(QCoreApplication::applicationDirPath());
        oldPath.cdUp();
        QString newPath = oldPath.filePath(fileName);

        qApp->quit();
        QProcess::startDetached(newPath, qApp->arguments());
        return 0;
    }

    qDebug("showing main window");
    MainWindow w;
    w.show();
    return a.exec();

}
