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
    QJsonParseError error;



    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &error);

    if (error.error == QJsonParseError::NoError)
    {

        QJsonObject whole_object = doc.object();

        //memset(&m_jsonDate, 0x00, sizeof(JsonWholeData_s));
        m_jsonDate.songs.clear();
        song.artists.clear();
        m_jsonDate.code = whole_object.value("code").toInt();

        QJsonObject songInfo = whole_object.value("result").toObject();
        m_jsonDate.songCount = songInfo.value("songCount").toInt();

        QJsonArray songArray = songInfo.value("songs").toArray();
        int arrayCount =songArray.count();

        for (int i = 0; i < arrayCount; i++)
        {
            //song.id = songArray.at(i).toObject().value("id").toString();
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


        qDebug()<<"search results:";
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
    }
    else
    {
        qDebug()<<"JsonDocument error: "<<error.errorString().toUtf8();
    }
}

void MainWindow::test()
{
    QString str = "{\"result\":{\"songCount\":65,\"songs\":[{\"id\":28668855,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":9823,\"name\":\"\xE9\xAD\x8F\xE6\x96\xB0\xE9\x9B\xA8\",\"picUrl\":null}],\"album\":{\"id\":2877053,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/OX6lR0jEjtxPfIgjVUtWww==/5786729697194540.jpg\"},\"audio\":\"http://m2.music.126.net/p0-voL9bTV1ZpZv3y6OKnQ==/7953867116601804.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/28668855\"},{\"id\":471969392,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":220705,\"name\":\"\xE9\x99\x88\xE6\xB3\xAF\xE8\xA5\xBF\",\"picUrl\":null}],\"album\":{\"id\":35368100,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/3l4VEtvMsod-2Z0aQkXp0A==/18677404023493684.jpg\"},\"audio\":\"http://m2.music.126.net/dAXOf3mJiC4_WRr7vJye2g==/19027048718743506.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/471969392\"},{\"id\":450846967,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":12256199,\"name\":\"\xE6\x9D\xA8\xE6\x99\x93\xE8\x89\xB3Nono\",\"picUrl\":null}],\"album\":{\"id\":35101716,\"name\":\"\xE7\xAD\x89\xE5\xBE\x85\xE5\x90\x88\xE9\x9B\x86\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/51lMuEfFMR_L6Gh-XNJlvw==/109951162835449389.jpg\"},\"audio\":\"http://m2.music.126.net/MoRSBcyuEp12BiA7Xv8pzA==/18773061534330779.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/450846967\"},{\"id\":443194949,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":12199164,\"name\":\"\xE5\x8D\x81\xE4\xB8\x83\xE5\xB2\x81\xE7\x9A\x84\xE5\x85\x94\xE5\x85\x94\xE5\x90\x9B\",\"picUrl\":null}],\"album\":{\"id\":35013691,\"name\":\"\xE6\x97\xA2\xE6\x98\xAF\xE4\xB8\x83\xE6\x9C\x88\xEF\xBC\x8C\xE4\xB9\x9F\xE6\x98\xAF\xE5\xAE\x89\xE7\x94\x9F\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/rkfvos5b3YLkYeS9WSt2QA==/109951162814739871.jpg\"},\"audio\":\"http://m2.music.126.net/UMfM7UaYIG11awWnMr3_hA==/18696095720154574.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/443194949\"},{\"id\":450785283,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":12256199,\"name\":\"\xE6\x9D\xA8\xE6\x99\x93\xE8\x89\xB3Nono\",\"picUrl\":null}],\"album\":{\"id\":35102408,\"name\":\"\xE7\xAD\x89\xE5\xBE\x85\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/-KOyJTJjovxwPZ6HN4cxtw==/109951162834484947.jpg\"},\"audio\":\"http://m2.music.126.net/Z5IAbu4MX4bkF9j84fNdLw==/18642219650627325.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/450785283\"},{\"id\":442899796,\"name\":\"\xE6\x81\x8B\xE4\xBA\xBA\xE5\xBF\x83\",\"artists\":[{\"id\":0,\"name\":\"\xE9\x98\xB3\xE5\xB0\x8F\xE9\x98\xB3\",\"picUrl\":null}],\"album\":{\"id\":35015283,\"name\":\"\xE5\x86\x8D\xE8\xA7\x81\xE5\x8F\xAA\xE6\x98\xAF\xE9\x99\x8C\xE7\x94\x9F\xE4\xBA\xBA\",\"artist\":{\"id\":0,\"name\":\"\",\"picUrl\":null},\"picUrl\":\"http://p1.music.126.net/oLDKrfWfgCDppk3MOgmQdw==/109951162846676803.jpg\"},\"audio\":\"http://m2.music.126.net/n8Kv5yQF00i7ehuNayX8vQ==/3427177785143925.mp3\",\"djProgramId\":0,\"page\":\"http://music.163.com/m/song/442899796\"}]},\"code\":200}";

        QJsonParseError jsonError;
       // QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toLatin1(), &jsonError);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toLatin1(), &jsonError);
        if(jsonError.error == QJsonParseError::NoError)
        {
            if(jsonDoc.isObject())
            {
                qDebug()<<"OKK";
            }
        }
        else
        {
            qDebug()<<"error :" <<jsonError.errorString();
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
    //qDebug() <<arry;
    analyzeJsonDate(arry);
}

void MainWindow::on_pushButton_clicked()
{

#if 0
    QString str;
    str = ui->lineEdit->text();

    m_manager->get(QNetworkRequest(QUrl(str)));
#else
    test();
#endif
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
    QString str = QString(res);
    int start = str.indexOf("(");
    int end = str.indexOf(")");
    QString str2 = str.mid(start + 1, end - start - 1);
    //qDebug()<<"*********** start="<<start;
    //qDebug()<<str2;
    ///qDebug()<<"$$$$$$$$$$$ end ="<<end;
    analyzeJsonDate(str2.toUtf8());
}






























