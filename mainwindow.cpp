#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "zlib/include/zconf.h"
#include "zlib/include/zlib.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_manager = new QNetworkAccessManager(this);

    ui->lineEdit->setText("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic");
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
    QString qJsonDat = QString(jsonData).toLatin1();
    qDebug()<<qJsonDat;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject whole_object = doc.object();
   // qDebug()<<"res = "<<jsonData;
    memset(&m_jsonDate, 0x00, sizeof(JsonWholeData_s));
    int b = m_jsonDate.code = whole_object.value("code").toInt();
    qDebug()<<"b = "<<b;
    QJsonObject songInfo = whole_object.value("result").toObject();
    int k = m_jsonDate.songCount = songInfo.value("songCount").toInt();
    qDebug()<<"k = "<<k;
    QJsonArray songArray = songInfo.value("songs").toArray();
    int arrayCount =songArray.count();

    for (int i = 0; i < arrayCount; i++)
    {
        song.id = songArray.at(i).toObject().value("id").toString();
        song.name = songArray.at(i).toObject().value("name").toString();
        QJsonArray artistArray = songArray.at(i).toObject().value("artists").toArray();
        int artistCount = artistArray.count();
        for (int j = 0; j < artistCount; j++)
        {

            artist.id = artistArray.at(j).toObject().value("id").toString();
            artist.name = artistArray.at(j).toObject().value("name").toString();
            artist.picUrl = artistArray.at(j).toObject().value("picUrl").toString();
            song.artists << artist;
        }

        QJsonObject albumInfo = songArray.at(i).toObject().value("album").toObject();
        song.album.id = albumInfo.value("id").toString();
        song.album.name = albumInfo.value("name").toString();
        QJsonObject album_artist = albumInfo.value("artist").toObject();
        song.album.artist.id = album_artist.value("id").toString();
        song.album.artist.name = album_artist.value("name").toString();
        song.album.artist.picUrl = album_artist.value("picUrl").toString();

        song.album.picUrl = albumInfo.value("picUrl").toString();

        song.audio = songArray.at(i).toObject().value("audio").toString();
        song.djProgramId = songArray.at(i).toObject().value("djProgramId").toString();
        song.page = songArray.at(i).toObject().value("page").toString();

        m_jsonDate.songs << song;
    }


    qDebug()<<"search results:";
    qDebug()<<"songCount = "<<m_jsonDate.songCount;
    qDebug()<<"code ="<<m_jsonDate.code;
    qDebug()<<"songs:"<<endl;
    for (int i = 0; i < m_jsonDate.songs.count(); i++)
    {
        qDebug()<<"id = "<<m_jsonDate.songs.at(i).id;
        qDebug()<<"name = "<<m_jsonDate.songs.at(i).name;

        qDebug()<<"artists:"<<endl;  //may has more than two person, need to modify the date struct
        for (int j = 0; i < m_jsonDate.songs.at(i).artists.count(); j++)
        {
            qDebug()<<"id = "<<m_jsonDate.songs.at(i).artists.at(j).id;
            qDebug()<<"name = "<<m_jsonDate.songs.at(i).artists.at(j).name;
            qDebug()<<"picUrl = "<<m_jsonDate.songs.at(i).artists.at(j).picUrl;
        }


        qDebug()<<"album:"<<endl;

        qDebug()<<"id = "<<m_jsonDate.songs.at(i).album.id;
        qDebug()<<"name = "<<m_jsonDate.songs.at(i).album.name;
        qDebug()<<"album-artist"<<endl;
        qDebug()<<"id = "<<m_jsonDate.songs.at(i).album.artist.id;
        qDebug()<<"name = "<<m_jsonDate.songs.at(i).album.artist.name;
        qDebug()<<"picUrl = "<<m_jsonDate.songs.at(i).album.artist.picUrl;
        qDebug()<<"picUrl = "<<m_jsonDate.songs.at(i).album.picUrl;
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
    qDebug() <<arry;
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
    int i;

    const unsigned char songName[] = {0xE6, 0x81, 0x8B, 0xE4, 0xBA, 0xBA, 0xE5, 0xBF, 0x83};


    QByteArray byt1;
    byt1.clear();

    byt1.append(0xE6);
    byt1.append(0x81);
    byt1.append(0x8B);
    byt1.append(0xE4);
    byt1.append(0xBA);
    byt1.append(0xBA);
    byt1.append(0xE5);
    byt1.append(0xBF);
    byt1.append(0x83);
    QByteArray arry;
    //QByteArray head_arry = "src=lofter&type=1&filterDj=true&s=" + byt1 + "&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic";
    QByteArray head_arry("/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic");
    QNetworkRequest requestsong;
    QNetworkAccessManager managersong;
    //requestsong.setHeader(QNetworkRequest::LocationHeader,
    //                      "src=lofter&type=1&filterDj=true&s=" + byt1 + "&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic HTTP/1.1");
    //requestsong.setHeader(QNetworkRequest::ContentDispositionHeader,
    //                      "src=lofter&type=1&filterDj=true&s=" + byt1 + "&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic HTTP/1.1");

    requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=6&offset=0&callback=loft.w.g.cbFuncSearchMusic"));
    //requestsong.setUrl(QUrl(QString(head_arry)));
   // qDebug()<<QUrl(QString(head_arry));
    requestsong.setRawHeader("Host", "s.music.163.com");
    requestsong.setRawHeader("Connection", "Keep-Alive");
    requestsong.setRawHeader("Cache-Control", "max-age=0");
    requestsong.setRawHeader("Upgrade-Insecure-Requests", "1");
    requestsong.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");
    //requestsong.setRawHeader("Accept-Encoding", "josn");   //ok
    requestsong.setRawHeader("Accept-Encoding", "gzip, deflate");
    requestsong.setRawHeader("Accept-Language", "zh-CN,zh;q=0.8");
    requestsong.setRawHeader("Cookie", "usertrack=ezq0allwsCsYiQ/vA1YUAg==");
    QNetworkReply *reply = managersong.get(requestsong);



    QEventLoop loop;
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
    QByteArray res = gloabUnGzip(arry);
    //qDebug()<<res;
    analyzeJsonDate(res);
}






























