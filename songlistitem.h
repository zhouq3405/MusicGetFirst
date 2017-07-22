#ifndef SONGLISTITEM_H
#define SONGLISTITEM_H

#include <QWidget>
#include <QDebug>

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


namespace Ui {
class SongListItem;
}

class SongListItem : public QWidget
{
    Q_OBJECT

public:
    explicit SongListItem(int songIndex, QString songName, QString artistName, QString albumName, QWidget *parent = 0);
    ~SongListItem();
    QString getSongName();
    QString getArtistName();
    QString getAlbumName();
    int getIndex();


    void setSongName(QString str);
    void setArtistName(QString str);
    void setAlbumName(QString str);
    void setIndex(int index);
protected:
    QString m_songName;
    QString m_artistName;
    QString m_albumName;
    int m_index;


private slots:
    void slot_download();
    void slot_play();
signals:
    void downloadSelectSongIndex(int index);
    void playSelectSongIndex(int index);
private:
    Ui::SongListItem *ui;
};

#endif // SONGLISTITEM_H
