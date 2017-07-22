#include "mythread.h"


#include <QModelIndex>
MyThread::MyThread()
{
    m_vlc_init = libvlc_new (0, NULL);
    m_pJsonData = NULL;
    m_selectSongIndex = 0;
}

MyThread::~MyThread()
{
    if (m_vlc_init)
    {
        libvlc_release (m_vlc_init);
    }
}

void MyThread::setCallbackFunc(JsonWholeData_s *jsonDat, int index)
{
#if 0
    m_thread_callback_func = func;
    m_thread_callback_arg1 = arg1;
    m_thread_callback_arg2 = arg2;
#endif
    m_pJsonData = jsonDat;
    m_selectSongIndex = index;
}


void MyThread::playClickedSong()
{
    qDebug()<<"3333";
    //JsonWholeData_s *pJsonData = reinterpret_cast<JsonWholeData_s *>(jsData);
    //QModelIndex *index = reinterpret_cast<QModelIndex *>(modelIndex);
   // QString audio_url = jsData->songs.at(modelIndex->row()).audio;
    QString audio_url = m_pJsonData->songs.at(m_selectSongIndex).audio;
    qDebug()<<"播放 ："<<audio_url;

    libvlc_time_t length;
    int width;
    int height;
    int wait_time=5000;

    if (m_vlc_init)
    {
        //m = libvlc_media_new_path (inst, audio_url.toStdString().c_str());
        m_media_t = libvlc_media_new_location(m_vlc_init, audio_url.toStdString().c_str());
        //m = libvlc_media_new_path (inst,  "http://m2.music.126.net/I1Nc9fvRtPgeKwpn9BmQ1g==/18534467511105767.mp3");
        m_mediaPlayer = libvlc_media_player_new_from_media (m_media_t);
        /* No need to keep the media now */
        libvlc_media_release (m_media_t);

        // play the media_player
        libvlc_media_player_play (m_mediaPlayer);

        //wait until the tracks are created
        _sleep (wait_time);
        length = libvlc_media_player_get_length(m_mediaPlayer);
        width = libvlc_video_get_width(m_mediaPlayer);
        height = libvlc_video_get_height(m_mediaPlayer);

        //Let it play
        _sleep (length-wait_time);


    }

}

void MyThread::run()
{
    qDebug()<<"run----";
#if 0
    if (m_thread_callback_func)
    {
        m_thread_callback_func(m_thread_callback_arg1, m_thread_callback_arg2);
    }
#else
    if (m_pJsonData)
    {
        playClickedSong();
    }
#endif
}


void MyThread::slot_stopPlayThread()
{
    if (m_mediaPlayer)
    {
        // Stop playing
        libvlc_media_player_stop (m_mediaPlayer);

        // Free the media_player
        libvlc_media_player_release (m_mediaPlayer);
    }
}

