#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QWidget>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QThread>
#include <QDateTime>
#include <QDesktopServices>

#include <src/tsuCrawler/tsucrawler.h>

namespace Ui {
class searchwindow;
}

class searchwindow : public QWidget
{
    Q_OBJECT

public:
    explicit searchwindow(QWidget *parent = nullptr);
    ~searchwindow();

    enum tableColumns {
        Name,
        OpenLink,
        OpenMagnet,
        Eta,
        Seeds,
        Leeches,
        Size,
        Category,
        Provider,
        Hash,
        Link,
        Magnet
    };
    Q_ENUM(tableColumns)

protected:
    enum class searchButtonStatus_t : uint8_t
    {
        search,
        cancel
    };

    searchButtonStatus_t m_searchButtonStatus {searchButtonStatus_t::search};

    bool isSearchButton_search() const {return (m_searchButtonStatus == searchButtonStatus_t::search);}
    bool isSearchButton_cancel() const {return (m_searchButtonStatus == searchButtonStatus_t::cancel);}

    void switchSearchButton();
    void setSearchButtonToCancel();
    void setSearchButtonToSearch();

public slots:
    void providerAdded(const tsuProvider *item);
    void providerNotValid(const QString providerName);
    void itemFound(const tsuProvider::searchItem item);
//    void iconDownloaded(const QString providerName, const QPixmap icon);
    void finishedSearch(int itemsFound, qint64 elapsed);

    void cellClicked(int row, int column);

    void searchRequestFromWeb(const QString textToSearch, const int category);

signals:
    void loadProviders();
//    void providerNeedIconDownload(QString providerName);
//    void changeProviderActivation(QString providerName, bool active);
    void cancelSearch();
    void downloadFromMagnet(const QString magnet);

    // for web
    void web_itemFound(const tsuProvider::searchItem item);
    void web_finishedSearch(int itemsFound, qint64 elapsed, int providersCount);

private:
    Ui::searchwindow *ui {};
    void changeEvent(QEvent *e);

    void sortTable();

    QString p_tsunamiScriptFolder;
    QString p_tsunamiCacheFolder;

    tsuCrawler *p_crawler {};
    QThread *p_crawler_thread{};

    tableColumns p_orderBy;
    Qt::SortOrder p_sortOrder;

private slots:
    void on_btnSearch_released();

    void on_btnOrderName_released();
    void on_btnOrderSeeds_released();
    void on_btnOrderDirection_released();
    void txtSearch_returnPressed();
};

#endif // SEARCHWINDOW_H
