#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "zlib/include/zconf.h"
#include "zlib/include/zlib.h"
#include "vlc/vlc.h"
#include <QMessageBox>


#define MAX_LIST_NAME 10

//#define QDEBUG_ENABLE


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_threadPlay = new MyThread(this);
    m_manager = new QNetworkAccessManager(this);
    m_songListModel = new QStandardItemModel;

    m_songListModel->setHorizontalHeaderItem(0, new QStandardItem("歌曲名"));
    m_songListModel->setHorizontalHeaderItem(1, new QStandardItem("歌手名"));
    m_songListModel->setHorizontalHeaderItem(2, new QStandardItem("专辑名"));
    ui->tableView->setColumnWidth(0, 200);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 100);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(startPlaySong(QModelIndex)));
    //ui->tableView->verticalHeader()->hide();
    ui->tableView->setModel(m_songListModel);

    ui->label_2->setText("总计：");
    //ui->lineEdit->setText("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0");
   connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_replyFinished(QNetworkReply*)));
   connect(m_threadPlay, SIGNAL(urlInvalid()), this, SLOT(songUrlInvalid()));
   connect(m_threadPlay, SIGNAL(playFailed()), this, SLOT(vlcParseMediaFailed()));
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::analyzeJsonDate(QByteArray jsonData)
{
    JsonSongArray_s song;
    JsonArtist_s artist;
    QJsonParseError error;

    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (error.error == QJsonParseError::NoError)
    {

        QJsonObject whole_object = doc.object();

        m_jsonDate.songs.clear();
        song.artists.clear();
        m_jsonDate.code = whole_object.value("code").toInt();

        QJsonObject songInfo = whole_object.value("result").toObject();
        m_jsonDate.songCount = songInfo.value("songCount").toInt();

        QJsonArray songArray = songInfo.value("songs").toArray();
        int arrayCount =songArray.count();

        for (int i = 0; i < arrayCount; i++)
        {
            song.id  = songArray.at(i).toObject().value("id").toInt();
            song.name = songArray.at(i).toObject().value("name").toString();
            QJsonArray artistArray = songArray.at(i).toObject().value("artists").toArray();
            int artistCount = artistArray.count();
            for (int j = 0; j < artistCount; j++)
            {

                artist.id = artistArray.at(j).toObject().value("id").toInt();
                artist.name = artistArray.at(j).toObject().value("name").toString();
                artist.picUrl = artistArray.at(j).toObject().value("picUrl").toString();
                song.artists << artist;
            }

            QJsonObject albumInfo = songArray.at(i).toObject().value("album").toObject();
            song.album.id = albumInfo.value("id").toInt();
            song.album.name = albumInfo.value("name").toString();
            QJsonObject album_artist = albumInfo.value("artist").toObject();
            song.album.artist.id = album_artist.value("id").toInt();
            song.album.artist.name = album_artist.value("name").toString();
            song.album.artist.picUrl = album_artist.value("picUrl").toString();

            song.album.picUrl = albumInfo.value("picUrl").toString();

            song.audio = songArray.at(i).toObject().value("audio").toString();
            song.djProgramId = songArray.at(i).toObject().value("djProgramId").toInt();
            song.page = songArray.at(i).toObject().value("page").toString();

            m_jsonDate.songs << song;
        }

#ifdef QDEBUG_ENABLE
        qDebug()<<"search results:"<<"Total "<<m_jsonDate.songCount;
        qDebug()<<"songCount = "<<m_jsonDate.songCount;
        qDebug()<<"code ="<<m_jsonDate.code;

        for (int i = 0; i < m_jsonDate.songs.count(); i++)
        {
            qDebug()<<"########################################";
            qDebug()<<"id = "<<m_jsonDate.songs.at(i).id;
            qDebug()<<"name = "<<m_jsonDate.songs.at(i).name;

            qDebug()<<"artists:";  //may has more than two person, need to modify the date struct
            for (int j = 0; j < m_jsonDate.songs.at(i).artists.count(); j++)
            {
                qDebug()<<"-id = "<<m_jsonDate.songs.at(i).artists.at(j).id;
                qDebug()<<"-name = "<<m_jsonDate.songs.at(i).artists.at(j).name;
                qDebug()<<"-picUrl = "<<m_jsonDate.songs.at(i).artists.at(j).picUrl;
                qDebug()<<endl;
            }


            qDebug()<<"album:";

            qDebug()<<"-id = "<<m_jsonDate.songs.at(i).album.id;
            qDebug()<<"-name = "<<m_jsonDate.songs.at(i).album.name;
            qDebug()<<"-artist:";
            qDebug()<<"--id = "<<m_jsonDate.songs.at(i).album.artist.id;
            qDebug()<<"--name = "<<m_jsonDate.songs.at(i).album.artist.name;
            qDebug()<<"--picUrl = "<<m_jsonDate.songs.at(i).album.artist.picUrl;
            qDebug()<<"-picUrl = "<<m_jsonDate.songs.at(i).album.picUrl;

            qDebug()<<"audio = "<<m_jsonDate.songs.at(i).audio;
            qDebug()<<"djProgramId ="<<m_jsonDate.songs.at(i).djProgramId;
            qDebug()<<"page ="<<m_jsonDate.songs.at(i).page;
        }
#endif
    }
    else
    {
        qDebug()<<"JsonDocument error: "<<error.errorString().toUtf8();
    }
}



void MainWindow::slot_replyFinished(QNetworkReply *reply)
{
    //QTextCodec *codec = QTextCodec::codecForName("utf8");
        //使用utf8编码, 这样可以显示中文
   //QString str = codec->toUnicode(reply->readAll());
    QByteArray arry = reply->readAll();

   // reply->deleteLater();//最后要释放replay对象
    qDebug()<<"get data ok";
    qDebug()<<arry;
    analyzeJsonDate(arry);
}


QByteArray MainWindow::gloabUnGzip(QByteArray srcData)
{
   z_stream ungzipstream;
   unsigned char buffer[4096];
   int ret = 0;
   QByteArray unComprData;

   ungzipstream.zalloc = Z_NULL;
   ungzipstream.zfree = Z_NULL;
   ungzipstream.opaque = Z_NULL;
   ungzipstream.avail_in = srcData.size();
   ungzipstream.next_in = (Bytef*)srcData.data();
   ret = inflateInit2(&ungzipstream,47);
   if(ret != Z_OK)
   return QByteArray();
   do
   {
      memset(buffer,0,4096);
      ungzipstream.avail_out = 4096;
      ungzipstream.next_out = buffer;
      ret = inflate(&ungzipstream,Z_NO_FLUSH);
      switch(ret)
      {
         case Z_NEED_DICT:
            ret = Z_DATA_ERROR;
         case Z_DATA_ERROR:
         case Z_MEM_ERROR:
            return QByteArray();
       }

      if(ret != Z_FINISH)
      {
         unComprData.append((char*)buffer);
      }

   }while(ungzipstream.avail_out == 0);

   return unComprData;
}
void MainWindow::on_pushButton_2_clicked()
{
    QByteArray arry;
    QNetworkRequest requestsong;
    QNetworkAccessManager managersong;
    QString url("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%1&limit=100&offset=0");
    url = url.arg(ui->lineEdit->text());

    //requestsong.setHeader(QNetworkRequest::LocationHeader,
    //                      "src=lofter&type=1&filterDj=true&s=" + byt1 + "&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic HTTP/1.1");
#if 0
    requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0&callback=loft.w.g.cbFuncSearchMusic"));
#else
    //requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0"));
    requestsong.setUrl(QUrl(url));
#endif
    requestsong.setRawHeader("Host", "s.music.163.com");
    requestsong.setRawHeader("Connection", "Keep-Alive");
    requestsong.setRawHeader("Cache-Control", "max-age=0");
    requestsong.setRawHeader("Upgrade-Insecure-Requests", "1");

    requestsong.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
#ifdef USE_WIRESHARK
    //requestsong.setRawHeader("Accept-Encoding", "gzip, deflate");
#else
    //requestsong.setRawHeader("Accept-Encoding", "josn");   //ok
    requestsong.setRawHeader("Accept-Encoding", "json");
#endif
    requestsong.setRawHeader("Accept-Language", "zh-CN,zh;q=0.8");
    requestsong.setRawHeader("Cookie", "usertrack=ezq0allwsCsYiQ/vA1YUAg==");
    QNetworkReply *reply = managersong.get(requestsong);



    QEventLoop loop;;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    if(reply->error()==QNetworkReply::NoError)
    {
         arry=reply->readAll();
    }
    else
    {
        qDebug()<<"Error: "<<reply->error();
        qDebug()<< "failed to get data";
        reply->deleteLater();
        return;
    }
    //reply->deleteLater();
    qDebug()<<"Get Data ok";

    //QByteArray res = gloabUnGzip(arry);
#if 0
    QString str = QString(res);
    int start = str.indexOf("(");
    int end = str.indexOf(")");
    QString str2 = str.mid(start + 1, end - start - 1);
    analyzeJsonDate(str2.toUtf8());
#else
    //QByteArray rearch_dat = QString(res).toUtf8();

    //qDebug()<<arry;
    analyzeJsonDate(arry);
    dispSearchRes();
#endif
}



//startIndex就是从搜到的歌曲第几个开始，0为开始
void MainWindow::dispSearchRes()
{

    if (m_jsonDate.songs.count() != 0)
    {


        //connect(ui->tableView, SIGNAL(clicked(QModelIndex)), this, SLOT(sendSelectPalySignal(QModelIndex)));
       // connect(this, SIGNAL(selectPlay(QModelIndex *)), this, SLOT(startPlaySong(QModelIndex *)));

        for (int i =0; i < m_jsonDate.songs.count(); i++)
        {
            m_songListModel->setItem(i, 0, new QStandardItem(m_jsonDate.songs.at(i).name));
            if (m_jsonDate.songs.at(i).artists.count() > 1)
            {
                QString artists;
                artists.clear();
                for (int ii = 0; ii < m_jsonDate.songs.at(i).artists.count() && ii < 4; ii++)
                {
                    artists += m_jsonDate.songs.at(i).artists.at(ii).name + " ";

                }
                m_songListModel->setItem(i, 1, new QStandardItem(artists));
            }
            else
            {
                m_songListModel->setItem(i, 1, new QStandardItem(m_jsonDate.songs.at(i).artists.at(0).name));
            }
            m_songListModel->setItem(i, 2, new QStandardItem(m_jsonDate.songs.at(i).album.name));
        }

    }
    else
    {
        qDebug()<<"没有搜到歌曲";
    }
    ui->label_2->setText(QString("总计：%1").arg(QString::number(m_jsonDate.songs.count())));
}


#if 0
static void playClickedSong(void *jsData, void *modelIndex)
{
    qDebug()<<"3333";
    JsonWholeData_s *pJsonData = reinterpret_cast<JsonWholeData_s *>(jsData);
    QModelIndex *index = reinterpret_cast<QModelIndex *>(modelIndex);
    QString audio_url = pJsonData->songs.at(index->row()).audio;
    qDebug()<<"播放 ："<<audio_url;
    libvlc_instance_t * inst;
    libvlc_media_player_t *mp;
    libvlc_media_t *m;

    libvlc_time_t length;
    int width;
    int height;
    int wait_time=5000;
    inst = libvlc_new (0, NULL);

    //m = libvlc_media_new_path (inst, audio_url.toStdString().c_str());
    m = libvlc_media_new_location(inst, audio_url.toStdString().c_str());
    //m = libvlc_media_new_path (inst,  "http://m2.music.126.net/I1Nc9fvRtPgeKwpn9BmQ1g==/18534467511105767.mp3");
    mp = libvlc_media_player_new_from_media (m);
    /* No need to keep the media now */
    libvlc_media_release (m);

    // play the media_player
    libvlc_media_player_play (mp);

    //wait until the tracks are created
    _sleep (wait_time);
    length = libvlc_media_player_get_length(mp);
    width = libvlc_video_get_width(mp);
    height = libvlc_video_get_height(mp);
  //  qDebug()<<QString("Stream Duration: %1s").arg(QString::number(length/1000));
  // printf("Resolution: %d x %d\n",width,height);
    //Let it play
    _sleep (length-wait_time);

    // Stop playing
    libvlc_media_player_stop (mp);

    // Free the media_player
    libvlc_media_player_release (mp);

    libvlc_release (inst);
}
#endif


#if 0   //日后研究， 两个类之间的函数的回调，和类型的转换问题
void MainWindow::startPlaySong(QModelIndex *index)
{
   qDebug()<<"44444444";
   m_threadPlay->setCallbackFunc(playClickedSong, reinterpret_cast<void *>(&m_jsonDate), reinterpret_cast<void *>(index));
   m_threadPlay->start();
   qDebug()<<"ssss";
}
#else
void MainWindow::startPlaySong(QModelIndex index)
{
   qDebug()<<"44444444";
   m_currSelectSongIndex = index.row();
#if 1

   if (m_threadPlay->isRunning())
   {
       qDebug()<<"5555";
       connect(m_threadPlay, SIGNAL(stopFinished()), this, SLOT(waitPlayThreadStop()));
       m_threadPlay->stopPlaying();
   }
   else
   {
       m_threadPlay->setCallbackFunc(&m_jsonDate, m_currSelectSongIndex);
       m_threadPlay->start();
       qDebug()<<"ssss";
   }
#endif
}

#endif


void MainWindow::waitPlayThreadStop()
{
    qDebug()<<"start other";
    disconnect(m_threadPlay, SIGNAL(stopFinished()), this, SLOT(waitPlayThreadStop()));
    if (!m_threadPlay->isRunning())  //只有一个线程维持播放， 放下一首的时候必须先退出线程，释放资源后再启动线程
    {
        m_threadPlay->setCallbackFunc(&m_jsonDate, m_currSelectSongIndex);
        m_threadPlay->start();
    }
}


void MainWindow::songUrlInvalid()
{
    QMessageBox::information(this, "网络错误", "获取该网络资源出错，请尝试其他歌曲");
}

void MainWindow::vlcParseMediaFailed()
{
    QMessageBox::information(this, "解码", "媒体解析错误");
}










