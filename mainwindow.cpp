#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QString>
#include "zlib/include/zconf.h"



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

    requestsong.setUrl(QUrl("http://s.music.163.com/search/get/?src=lofter&type=1&filterDj=true&s=%E6%81%8B%E4%BA%BA%E5%BF%83&limit=1&offset=0&callback=loft.w.g.cbFuncSearchMusic"));
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
    uncompress();
    qDebug()<<arry;
}






























