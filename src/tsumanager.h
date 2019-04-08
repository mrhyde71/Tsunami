#ifndef TSUMANAGER_H
#define TSUMANAGER_H

#include <chrono> // needed for std::chrono::milliseconds

#include <QSharedPointer>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QStandardPaths>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <libtorrent/session.hpp>
#include <libtorrent/torrent_info.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/bencode.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session_stats.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/torrent.hpp>
#include <libtorrent/torrent_status.hpp>

#include "tsuitem.h"
#include "tsuevents.h"
#include "tsuCrawler/tsuprovider.h"

#include "src/httpserver/httplistener.h"
#include "src/httpserver/handler/requesthandler.h"
#include "src/httpserver/handler/websockethandler.h"

class tsuManager :public QObject
{
    Q_OBJECT
public:
    tsuManager();
//    explicit tsuManager(const libtorrent::settings_pack & sp);
    ~tsuManager();
    static int outstanding_resume_data;

    void addItems(const QStringList &&items, const QString &path);
    void addFromMagnet(const QStringList &&items, const QString &path);

    QSharedPointer<libtorrent::session> p_session;

signals:
    void updateFromSessionManager(const QVector<tsuEvents::tsuEvent> events);
    void addFromSessionManager(const tsuEvents::tsuEvent &event);
    void torrentDeleted(const std::string &hash);

    void externalIpAssigned();
    void dhtBootstrapExecuted();
    void listenerUpdate(QString type, bool success);

    void stopTimer();
    void finished();
    
    void sessionStatisticUpdate(const quint64 &sent, const quint64 &received,
                                const quint64 &downloading, const quint64 &uploading,
                                const quint64 &checking, const quint64 &stopped,
                                const quint64 &error, const quint64 &queuedDown,
                                const quint64 &queuedSeed);

    // signal to inform Mainwindow to ask Searchwindow to search
    void web_NeedSearch(const QString textToSearch, const int category);

public slots:
    void getCancelRequest(const std::string &hash, bool deleteFilesToo);
    void getPauseRequest(const std::string &hash);
    void getResumeRequest(const std::string &hash);
    void refreshSettings();
    void startManager();
    void stopManager();

    // request from www
    void web_requestedSendTorrentList();
    void web_requestedCancel(const QString& hash, bool deleteFilesToo);
    void web_requestedPause(const QString& hash);
    void web_requestedResume(const QString& hash);
    void web_requestedFileList(const QString& hash);
    void web_requestedSearch(const QString& textToSearch, int category);
    void web_itemFound(const tsuProvider::searchItem item);
    void web_finishedSearch(int itemsFound, qint64 elapsed, int providersCount);
    void web_fileUploaded(const QByteArray& buffer, const QString& fileName);
    void web_downloadMagnet(const QString& magnet);

private:
    void setNotify();
    void loadSettings(libtorrent::settings_pack &settings);

    QString p_tsunamiSessionFolder;

    QTimer *p_timerUpdate{};
    static constexpr std::chrono::milliseconds p_timerUpdateInterval {1 * 1000}; // every second

    QTimer *p_timerResumeData{};
    static constexpr std::chrono::milliseconds p_timerResumeDataInterval {10 * 60 * 1000}; // every ten minutes

    // session stat alert index
    int p_net_recv_bytes{};
    int p_net_recv_payload_bytes{};
    int p_net_recv_ip_overhead_bytes{};

    int p_net_sent_bytes{};
    int p_net_sent_payload_bytes{};
    int p_net_sent_ip_overhead_bytes{};

    int p_ses_num_downloading_torrents{};
    int p_ses_num_queued_download_torrents{};
    int p_ses_num_upload_only_torrents{};
    int p_ses_num_seeding_torrents{};
    int p_ses_num_queued_seeding_torrents{};
    int p_ses_num_checking_torrents{};
    int p_ses_num_stopped_torrents{};
    int p_ses_num_error_torrents{};

    bool p_isWebEnabled {false};
    void initializeListener();
    void updateListener();
//    HttpListener* p_webListener;
//    webSocketHandler* p_socketListener;
//    RequestHandler* p_requestHandler;
    QSharedPointer<HttpListener> p_webListener;
    QSharedPointer<webSocketHandler> p_socketListener;
    QSharedPointer<RequestHandler> p_requestHandler;

private slots:
    void alertsHandler();
    void postUpdates();
    void postResumeData();
};

/*
#include <QObject>
#include <QTimer>
#include <QDebug>


#include "libtorrent/session.hpp"
#include "libtorrent/session_status.hpp"
#include "libtorrent/torrent_handle.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/torrent_status.hpp"
#include "libtorrent/alert_types.hpp"

namespace tsuEvents {
    class tsuEvent
    {
        public:
        tsuEvent()
        { }
        tsuEvent(const std::string &_hash, const QString &_name, const int &_downloaded = 0,
                 const int &_uploaded = 0, const int &_downRate = 0, const int &_upRate = 0,
                 const int &_size = 0, const int &_state = 0, const int &_percentage = 0):
            hash(_hash),
            name(_name),
            downloaded(_downloaded),
            uploaded(_uploaded),
            downloadRate(_downRate),
            uploadRate(_upRate),
            total_size(_size),
            state(_state),
            percentage(_percentage)
        { }
        ~tsuEvent()
        { }

            std::string hash ="";
            QString name ="";
            int downloaded = 0;
            int uploaded = 0;
            int downloadRate = 0;
            int uploadRate = 0;
            int total_size = 0;
            int state = 0;
            int percentage = 0;
    };
}

class Worker : public QObject
{
    Q_OBJECT
//    QThread workerThread;

public slots:
    void doWork(std::vector<libtorrent::alert*> alerts) {
//        std::vector<libtorrent::alert*> alerts;
//        p_session.get()->pop_alerts(&alerts);

    //    qDebug() << QString("processing %1 alerts:").arg(alerts.size());
        for (libtorrent::alert* a : alerts) {
            if (a == nullptr) continue;
    //        qDebug() << QString(" %1").arg(a->message().c_str());

            if (auto st = libtorrent::alert_cast<libtorrent::state_update_alert>(a)) {
                if (st == nullptr) continue;
                if (st->status.empty()) continue;
                for (libtorrent::torrent_status s : st->status) {
                    try {
                        tsuEvents::tsuEvent ev(s.info_hash.to_string(), QString::fromStdString(s.name), s.total_done, s.total_upload, s.download_rate, s.upload_rate, s.total_wanted, s.state, s.progress_ppm);
                        emit eventFromLibtorrent(ev);
                    } catch (std::exception &exc) {
                        qDebug() << QString("timerTick throws %1").arg(exc.what());
                    }
                }
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_error_alert>(a)) {
                qDebug() << QString("torrent_error_alert %1").arg(QString::fromStdString(aa->message()));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::add_torrent_alert>(a)) {
                libtorrent::torrent_handle th = aa->handle;
                libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
                tsuEvents::tsuEvent ev(st.info_hash.to_string(), QString::fromStdString(st.name), 0, 0, 0, 0, st.total_wanted, 0, 0);
                emit eventFromLibtorrent(ev);
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_finished_alert>(a)) {
                libtorrent::torrent_handle th = aa->handle;
                libtorrent::torrent_status st = th.status(libtorrent::torrent_handle::query_name);
    //            emit sendPopupInfo(QString("%1 finished!").arg(QString::fromStdString(st.name)));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::tracker_error_alert>(a)) {
                qDebug() << QString("tracker_error_alert %1").arg(QString::fromStdString(aa->message()));
            }

            if (auto aa = libtorrent::alert_cast<libtorrent::torrent_removed_alert>(a)) {
                emit torrentDeletedFromLibtorrent(aa->info_hash.to_string());
            }

        }
//        p_session.get()->post_torrent_updates(libtorrent::alert::status_notification);
    }


};


*/

Q_DECLARE_METATYPE(tsuEvents::tsuEvent)
Q_DECLARE_METATYPE(std::string)

#endif // TSUMANAGER_H
