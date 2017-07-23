#include "mythread.h"


#include <QModelIndex>
MyThread::MyThread(QObject *parent) : QThread(parent)
{
    m_vlc_init = libvlc_new (0, NULL);
    m_pJsonData = NULL;
    m_selectSongIndex = 0;
    m_currentPos = 0;
    m_isRunning = 0;
}

MyThread::~MyThread()
{
    if (m_vlc_init)
    {
        libvlc_release (m_vlc_init);
    }
    if (m_mediaPlayer)
    {
        m_isRunning = 0;
        wait(2000);  //wait the play thread exit normally.
        libvlc_media_player_release (m_mediaPlayer);
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


int MyThread::playClickedSong()
{
    QString audio_url = m_pJsonData->songs.at(m_selectSongIndex).audio;
    qDebug()<<"播放 ："<<audio_url;


    //int wait_time=5000;

    if (m_vlc_init)
    {

        m_media_t = libvlc_media_new_location(m_vlc_init, audio_url.toStdString().c_str());
        if (m_media_t == NULL)
        {
            emit urlInvalid();    //网址无效
            return -1;  //error
        }

        m_mediaPlayer = libvlc_media_player_new_from_media (m_media_t);
        if (m_mediaPlayer == NULL)
        {
            emit playFailed();
            return -1;
        }
        /* No need to keep the media now */
        libvlc_media_release (m_media_t);

        // play the media_player
        if (libvlc_media_player_play (m_mediaPlayer) == -1)
        {
            libvlc_media_player_release(m_mediaPlayer);
            m_mediaPlayer = NULL;
            emit playFailed();    //播放出错
            return -1;
        }

        //wait until the tracks are created
        //_sleep (wait_time);
        msleep(1000);
        m_mediaLen = libvlc_media_player_get_length(m_mediaPlayer);
        if (m_mediaLen == -1)
        {
            libvlc_media_player_release(m_mediaPlayer);
            m_mediaPlayer = NULL;
            emit playFailed();    //播放出错
            return -1;
        }
        else
        {
            if (m_mediaLen == 0)
            {
                qDebug()<<"长度为0， 未知";
            }
        }
        qDebug()<<"length (ms) = "<<m_mediaLen;
    }
    m_isRunning = 1;
    while(m_isRunning)
    {
       if (m_mediaPlayer)
       {
            m_currentPos = libvlc_media_player_get_time(m_mediaPlayer);
            if (m_currentPos == -1)
            {
                qDebug()<<"获取当前pos出错";
                libvlc_media_player_release(m_mediaPlayer);
                m_mediaPlayer = NULL;
                emit playFailed();    //播放出错
                return -1;
            }
            //qDebug()<<"当前pos :"<<m_currentPos;
            emit reportCurPos(m_currentPos);
       }
       this->msleep(500);
    }
    qDebug()<<"here exit run";

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



int MyThread::pauseOrRunPlaying(int status)
{
    if (m_mediaPlayer)
    {
        libvlc_media_player_set_pause(m_mediaPlayer, status);
    }
    return 0;
}

int MyThread::stopPlaying()
{
    if (m_mediaPlayer)
    {
         m_isRunning = 0;
         libvlc_media_player_stop(m_mediaPlayer);

         wait(2000); //等待线程正常退出
         qDebug()<<"6666";
         emit stopFinished();
    }
    return 0;
}


//offset is 0~100
int MyThread::seekPlaying(int offset)
{
    float offset_t = offset;
    libvlc_time_t real_offset;
    if (offset_t > 100)
        return -1;
    real_offset = m_mediaLen * (offset_t / 100);
    if (m_mediaPlayer)
    {
        libvlc_media_player_set_time(m_mediaPlayer, real_offset);
    }
    return 0;
}

libvlc_time_t MyThread::getTotalLen()
{
    return m_mediaLen;
}


libvlc_time_t MyThread::getCurrentPos()
{
    return m_currentPos;
}




