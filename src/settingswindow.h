#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>

#include <QApplication>
#include <QProcess>

#include <src/changelog.h>

namespace Ui {
class settingswindow;
}

class settingswindow : public QWidget
{
    Q_OBJECT

private:
    void changeEvent(QEvent *e);
    void enableSaveCancelButtons(bool enable = true);

public:
    explicit settingswindow(QWidget *parent = nullptr);
    ~settingswindow();

    void loadSettings();
    void saveSettings();

    void setLastBrowsedDir(const QString &path);
    QString getLastBrowsedDir() const;

    static QString settingsFileName;
    static QSettings::Format settingsFileFormat;

    QString getDownloadPath() const;

    int getDebugLevelIndex() const;
    int getCurrentLanguageIndex() const;
signals:
    void sendMessageToStatusBar(const QString & msg);
    void sendRefreshSettings();

private slots:
    void on_btnSave_released();
    void on_btnCancel_released();
    void on_btnFolder_released();

    void on_commandLinkButton_released();

    void on_settingsMenu_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

    void on_btnOpenWeb_released();

private:
    Ui::settingswindow *ui;

    QSettings settings;

    bool m_btnSaveEnabled {false};
    bool m_btnCancelEnabled {false};

//    bool needRestart = false;
};

#endif // SETTINGSWINDOW_H
