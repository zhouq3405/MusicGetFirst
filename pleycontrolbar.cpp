#include "pleycontrolbar.h"
#include "ui_pleycontrolbar.h"

PleyControlBar::PleyControlBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PleyControlBar)
{
    ui->setupUi(this);
    m_isPlaying = 0;
    //ui->pushButton_2->setText("播放");
    ui->horizontalSlider->setRange(0,100);
    ui->verticalSlider->setRange(0, 100);
}

PleyControlBar::~PleyControlBar()
{
    delete ui;
}


//上一首
void PleyControlBar::on_pushButton_clicked()
{
    qDebug()<<"上一首";
    emit previous();
}
//播放
void PleyControlBar::on_pushButton_2_clicked()
{
#if 0
    qDebug()<<"播放/暂停";
    m_isPlaying = !m_isPlaying;
    if (m_isPlaying)
    {
        ui->pushButton_2->setText("播放");
    }
    else
    {
        ui->pushButton_2->setText("暂停");
    }
#endif
    emit play_or_pause();
}

//下一首
void PleyControlBar::on_pushButton_3_clicked()
{
    qDebug()<<"下一首";
    emit next();
}


//调整进度
void PleyControlBar::on_horizontalSlider_sliderMoved(int value)
{
    qDebug()<<"进度调整: "<<value;
    emit changeProcess(value);
}


//音量调整
void PleyControlBar::on_verticalSlider_valueChanged(int value)
{
    qDebug()<<"音量调整："<<value;
    emit changeVol(value);
}


int PleyControlBar::setPlayOrPauseText(QString str)
{
    ui->pushButton_2->setText(str);
}


int PleyControlBar::setVolBarVal(int value)
{
    if (value < 0)
    {
        value = 0;
    }

    if (value > 100)
    {
        value = 100;
    }
    ui->verticalSlider->setValue(value);
    return 0;
}

int PleyControlBar::setProcessBarVal(int value)
{
    if (value < 0)
    {
        value = 0;
    }

    if (value > 100)
    {
        value = 100;
    }
    ui->horizontalSlider->setValue(value);
    return 0;
}

int PleyControlBar::setSongName(QString str)
{
    ui->labelSong->setText(str);
}

int PleyControlBar::setPlayInfo(QString str)
{
    ui->labelProcess->setText(str);
}


int PleyControlBar::setPicLabel(QPixmap pix)
{
    ui->picLabel->setScaledContents(true);
    ui->picLabel->setPixmap(pix);

}


int PleyControlBar::setPlayButtonStyleSheet(QString str)
{
    ui->pushButton_2->setStyleSheet(str);
}


