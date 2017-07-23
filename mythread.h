#ifndef MYTHREAED_H
#define MYTHREAED_H

#include <QObject>
#include <QThread>
#include "songlistitem.h"
#include <QDebug>
#include <QVector>
#include <QString>
#include "vlc/vlc.h"

typedef void (*MyThread_CALLBACK_FUNC)(void *arg1, void *arg2);

class MyThread : public QThread
{
    Q_OBJECT

public:
    explicit MyThread(QObject *parent);
    ~MyThread();
#if 0
    void setCallbackFunc(MyThread_CALLBACK_FUNC func, void *arg1, void *arg2);
#else
    void setCallbackFunc(JsonWholeData_s *jsData, int index); 
#endif
    int pauseOrRunPlaying(int status);
    int stopPlaying();
    int seekPlaying(int offset);
    libvlc_time_t getTotalLen();
    libvlc_time_t getCurrentPos();
protected:
    void run();
    int playClickedSong();

signals:
    void stopFinished();
    void urlInvalid();
    void playFailed();
    void reportCurPos(libvlc_time_t pos);
private slots:
    void slot_stopPlayThread();

private:
    libvlc_instance_t * m_vlc_init;
    libvlc_media_player_t *m_mediaPlayer;
    libvlc_media_t *m_media_t;
    libvlc_time_t m_mediaLen;
    libvlc_time_t m_currentPos;
    JsonWholeData_s *m_pJsonData;
    int m_isRunning;

    int m_selectSongIndex;
#if 0
    MyThread_CALLBACK_FUNC m_thread_callback_func;
    void *m_thread_callback_arg1;
    void *m_thread_callback_arg2;
#endif
};

#endif // MYTHREAED_H
