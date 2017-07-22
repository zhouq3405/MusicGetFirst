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
public:
    MyThread();
    ~MyThread();
#if 0
    void setCallbackFunc(MyThread_CALLBACK_FUNC func, void *arg1, void *arg2);
#else
    void setCallbackFunc(JsonWholeData_s *jsData, int index);
#endif
protected:
    void run();
    void playClickedSong();

signals:
    void stopPlayThread();

private slots:
    void slot_stopPlayThread();

private:
    libvlc_instance_t * m_vlc_init;
    libvlc_media_player_t *m_mediaPlayer;
    libvlc_media_t *m_media_t;
    JsonWholeData_s *m_pJsonData;

    int m_selectSongIndex;
#if 0
    MyThread_CALLBACK_FUNC m_thread_callback_func;
    void *m_thread_callback_arg1;
    void *m_thread_callback_arg2;
#endif
};

#endif // MYTHREAED_H
