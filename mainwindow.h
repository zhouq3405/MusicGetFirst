#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QtNetwork/QNetworkReply>
#include <QVector>
namespace Ui {
class MainWindow;
}

typedef struct
{
    unsigned int id;
    QString name;
    QString picUrl;
}JsonArtist_s;

typedef struct
{
    unsigned int id;
    QString name;
    JsonArtist_s artist;
    QString picUrl;
}JsonAlbum_s;

typedef struct
{
    unsigned int id;
    QString name;
    QVector<JsonArtist_s> artists;
    JsonAlbum_s album;
    QString audio;
    unsigned int djProgramId;
    QString page;
}JsonSongArray_s;


typedef struct
{
    int code;
    int songCount;
    QVector<JsonSongArray_s> songs;
}JsonWholeData_s;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QNetworkAccessManager *m_manager;
    QByteArray gloabUnGzip(QByteArray srcData);
    void analyzeJsonDate(QByteArray jsonData);
    void test();
    JsonWholeData_s m_jsonDate;
private slots:
    void slot_replyFinished(QNetworkReply *reply);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
