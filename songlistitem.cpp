#include "songlistitem.h"
#include "ui_songlistitem.h"

SongListItem::SongListItem(int songIndex, QString songName, QString artistName, QString albumName, QWidget *parent) :
    QWidget(parent),m_index(songIndex),
    ui(new Ui::SongListItem)
{
    ui->setupUi(this);
    setSongName(songName);
    setArtistName(artistName);
    setAlbumName(albumName);
    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(slot_download()));
    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(slot_play()));
}

SongListItem::~SongListItem()
{
    delete ui;
}

void SongListItem::slot_download()
{
    emit downloadSelectSongIndex(m_index);
}

void SongListItem::slot_play()
{
    emit playSelectSongIndex(m_index);
}


QString SongListItem::getSongName()
{
    return m_songName;
}

QString SongListItem::getArtistName()
{
    return m_artistName;
}

QString SongListItem::getAlbumName()
{
    return m_albumName;
}

void SongListItem::setSongName(QString str)
{
    m_songName = str;
    ui->label->setText(str);
}

void SongListItem::setArtistName(QString str)
{
    m_artistName = str;
    ui->label_2->setText(str);
}


void SongListItem::setAlbumName(QString str)
{
    m_albumName = str;
    ui->label_3->setText(str);
}


int SongListItem::getIndex()
{
    return m_index;
}

void SongListItem::setIndex(int index)
{
    m_index = index;
}






























