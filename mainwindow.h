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
#include "songlistitem.h"
#include "pleycontrolbar.h"
#include <QStandardItemModel>
#include "mythread.h"


namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QNetworkAccessManager *m_manager;
    QByteArray gloabUnGzip(QByteArray srcData);
    void analyzeJsonDate(QByteArray jsonData);
    void dispSearchRes();
    //void startPlaySong();
    //static void playClickedSong(void *arg); //作为函数指针参数， 需要时static的，相当于是一般的函数
    JsonWholeData_s m_jsonDate;
    MyThread *m_threadPlay;


signals:
    void selectPlay(QModelIndex *);

private slots:
    void slot_replyFinished(QNetworkReply *reply);

    void on_pushButton_2_clicked();
    void startPlaySong(QModelIndex index);
    void waitPlayThreadStop();
    void songUrlInvalid();
    void vlcParseMediaFailed();
    void slot_playNext();
    void slot_playPre();
    void slot_play_or_pause();
    void slot_changePlayProcess(int value);
    void slot_changeVol(int value);
private:
    Ui::MainWindow *ui;
    QStandardItemModel *m_songListModel;
    int m_currSelectSongIndex;
    int m_isPause;

};

#endif // MAINWINDOW_H
