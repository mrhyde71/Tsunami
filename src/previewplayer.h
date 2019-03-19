#ifndef PREVIEWPLAYER_H
#define PREVIEWPLAYER_H

#include <QObject>
#include <QWidget>
#include <QFrame>
#ifdef LIBVLC
#include "vlc/vlc.h"
#else
#include "VLCQtCore/Common.h"
#include "VLCQtCore/Instance.h"
#include "VLCQtCore/Media.h"
#include "VLCQtCore/MediaPlayer.h"
#endif

#define REWIND_VALUE_DEFAULT 5

class PreviewPlayer : public QObject
{
    Q_OBJECT

// member variables
private:
#ifdef LIBVLC
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *mediaPlayer;
    libvlc_media_t *media;
#else
    VlcInstance *vlcInstance {};
    VlcMedia *media = Q_NULLPTR;
    float actualPosition{};
    int length{};
#endif

    int rewindHiddenValueInSeconds{};

public:
    bool isPlaying{};
    VlcMediaPlayer *mediaPlayer{};


// methods

public:
#ifdef LIBVLC
    explicit PreviewPlayer(QObject *parent = nullptr);
#else
    explicit PreviewPlayer(VlcVideoDelegate *videoArea, QObject *parent = nullptr);
#endif
    ~PreviewPlayer();
    //float GetTimelinePosition();
    //int GetVolume();

    void ResetCurrentMedia();
    void rewindForHidden();
signals:

public slots:
#ifdef LIBVLC
    void playFile(const QString& file, WId windowId);
#else
    void playFile(const QString& file);
    void playPause();
#endif
    //void changeVolume(int newVolume);
    //void changeTimelinePosition(int newTimelinePosition, unsigned int positionResolution);
};

#endif // PREVIEWPLAYER_H
