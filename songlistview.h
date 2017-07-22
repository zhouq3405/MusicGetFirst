#ifndef SONGLISTVIEW_H
#define SONGLISTVIEW_H

#include <QObject>
#include <QListWidget>
#include <QWidget>
#include "songlistitem.h"
class SongListView : public QListWidget
{
public:
    explicit SongListView(QListWidget *parent);
    ~SongListView();
    void addItem(QWidget *parent, int index, QString songName, QString artistName, QString albumName);

signals:
    void pressDownload(int index);
    void pressPlay(int index);

private slots:
    void slot_pressDownload(void);
    void slot_pressPlay(void);
};

#endif // SONGLISTVIEW_H
