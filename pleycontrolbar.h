#ifndef PLEYCONTROLBAR_H
#define PLEYCONTROLBAR_H

#include <QWidget>
#include <QDebug>
#include <QPixmap>
namespace Ui {
class PleyControlBar;
}

class PleyControlBar : public QWidget
{
    Q_OBJECT

public:
    explicit PleyControlBar(QWidget *parent = 0);
    ~PleyControlBar();
    int m_isPlaying;
    int setPlayOrPauseText(QString str);
    int setVolBarVal(int value);
    int setProcessBarVal(int value);
    int setPicLabel(QPixmap pix);
    int setSongName(QString str);
    int setPlayInfo(QString str);
signals:
    void play_or_pause();
    void next();
    void previous();
    void changeProcess(int time);
    void changeVol(int vol);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_verticalSlider_valueChanged(int value);

private:
    Ui::PleyControlBar *ui;
};

#endif // PLEYCONTROLBAR_H
