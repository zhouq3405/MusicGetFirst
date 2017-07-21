#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "zlib/include/zconf.h"
#include "zlib/include/zlib.h"


#define QDEBUG_ENABLE


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_manager = new QNetworkAccessManager(this);

    ui->lineEdit->setText("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0");
   connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_replyFinished(QNetworkReply*)));
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

void MainWindow::on_pushButton_clicked()
{
    QString str;
    str = ui->lineEdit->text();
    m_manager->get(QNetworkRequest(QUrl(str)));
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


    //requestsong.setHeader(QNetworkRequest::LocationHeader,
    //                      "src=lofter&type=1&filterDj=true&s=" + byt1 + "&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic HTTP/1.1");
#if 0
    requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0&callback=loft.w.g.cbFuncSearchMusic"));
#else
    requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=100&offset=0"));
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

    qDebug()<<arry;
    analyzeJsonDate(arry);
#endif
}






























