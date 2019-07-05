#include <QLabel>
#include <QDateTime>
#include <QProcess>
#include <QDebug>
#include <QMenu>
#include <QKeyEvent>
#include <QMessageBox>
#include <QFileDialog>

#include "VLCQtCore/Common.h"
#include "VLCQtCore/Instance.h"
#include "VLCQtCore/Media.h"
#include "VLCQtCore/MediaPlayer.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pInstance(nullptr),
    m_pMedia(nullptr),
    m_pPlayer(nullptr),
    m_bIsMaximized(false),
    m_clipStartSecond(0),
    m_clipEndSecond(0),
    m_pClipLabel(nullptr)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::DefaultContextMenu);
    setWindowTitle(tr("FFPlayer"));

    m_pInstance = new VlcInstance(VlcCommon::args(), this);
    m_pPlayer = new VlcMediaPlayer(m_pInstance);
    m_pPlayer->setVideoWidget(ui->video);

    ui->action_Open->setIcon(QIcon(":/img/open.png"));
    ui->action_Full_Screen->setIcon(QIcon(":/img/full_screen.png"));
    ui->action_Exit->setIcon(QIcon(":/img/exit.png"));
    ui->action_About->setIcon(QIcon(":/img/about.png"));
    ui->action_Grab->setIcon(QIcon(":/img/grab.png"));
    ui->actionClip_Left->setIcon(QIcon(":/img/clip_left.png"));
    ui->actionClip_Right->setIcon(QIcon(":/img/clip_right.png"));
    ui->action_Clip->setIcon(QIcon(":/img/clip.png"));

    ui->video->setMediaPlayer(m_pPlayer);
    ui->volume->setMediaPlayer(m_pPlayer);
    ui->volume->setVolume(100);
    ui->volume->setVisible(false);
    ui->seek->setMediaPlayer(m_pPlayer);

    m_pClipLabel = new QLabel(this);
    m_pClipLabel->setText("00:00:00 - 00:00:00");

    ui->mainToolBar->addAction(ui->action_Open);
    ui->mainToolBar->addAction(ui->action_Full_Screen);
    ui->mainToolBar->addAction(ui->action_Grab);
    ui->clipToolBar->addAction(ui->actionClip_Left);
    ui->clipToolBar->addWidget(m_pClipLabel);
    ui->clipToolBar->addAction(ui->actionClip_Right);
    ui->clipToolBar->addAction(ui->action_Clip);

    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(ui->action_Full_Screen, SIGNAL(triggered()), this, SLOT(onFullScreen()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_Grab, SIGNAL(triggered()), this, SLOT(onGrabImage()));
    connect(ui->actionClip_Left, SIGNAL(triggered()), this, SLOT(onClipLeft()));
    connect(ui->actionClip_Right, SIGNAL(triggered()), this, SLOT(onClipRight()));
    connect(ui->action_Clip, SIGNAL(triggered()), this, SLOT(onClip()));
}

MainWindow::~MainWindow()
{
    delete m_pPlayer;
    delete m_pMedia;
    delete m_pInstance;
    delete ui;
}

void MainWindow::openFile(const QString &strFilePath)
{
    if(m_pMedia != nullptr)
    {
        delete m_pMedia;
        m_pMedia = nullptr;
    }
    m_strFileName = strFilePath;
    m_pMedia = new VlcMedia(strFilePath, true, m_pInstance);
    m_pPlayer->open(m_pMedia);
    setWindowTitle(strFilePath);
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_F:
        onFullScreen();
        break;
    case Qt::Key_Q:
        close();
        break;
    case Qt::Key_Space:
        m_pPlayer->togglePause();
        break;
    case Qt::Key_Left:
        backward(10 * 1000);
        break;
    case Qt::Key_Right:
        forward(10 * 1000);
        break;
    case Qt::Key_1:
        onGrabImage();
        break;
    case Qt::Key_Comma:
        onClipLeft();
        break;
    case Qt::Key_Period:
        onClipRight();
        break;
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
    QMenu contextMenu(this);
    contextMenu.addAction(ui->action_Full_Screen);
    contextMenu.addAction(ui->action_Grab);
    contextMenu.addAction(ui->actionClip_Left);
    contextMenu.addAction(ui->actionClip_Right);
    contextMenu.addAction(ui->action_Clip);
    contextMenu.exec(QCursor::pos());
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    onFullScreen();
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    e->delta() > 0 ? forward(5 * 1000) : backward(5 * 1000);
}

void MainWindow::onOpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this, tr("Select file"), qApp->applicationFilePath());
    if(strFileName.isEmpty())
        return;
    openFile(strFileName);
}

void MainWindow::onFullScreen()
{
    if(isFullScreen())
    {
        ui->mainToolBar->show();
        ui->menuBar->show();
        //ui->seek->show();
        statusBar()->show();
        m_bIsMaximized ? showMaximized() : showNormal();
    }
    else
    {
        m_bIsMaximized = isMaximized();
        ui->mainToolBar->hide();
        ui->menuBar->hide();
        //ui->seek->hide();
        statusBar()->hide();
        showFullScreen();
    }
}

void MainWindow::onAbout()
{
    QMessageBox box(this);
    box.setIcon(QMessageBox::Information);
    box.setWindowTitle(tr("about"));
    box.setText(tr("zhaohuarong@gmail.com"));
    box.exec();
}

void MainWindow::onGrabImage()
{
    grabImage(m_pPlayer->position() * m_pMedia->duration() / 1000);
}

void MainWindow::onClipLeft()
{
    m_clipStartSecond = m_pPlayer->position() * m_pMedia->duration() / 1000;
    m_pClipLabel->setText(QString("%1 - %2").arg(convertSecondToTimeFormat(m_clipStartSecond)).arg(convertSecondToTimeFormat(m_clipEndSecond)));
}

void MainWindow::onClipRight()
{
    m_clipEndSecond = m_pPlayer->position() * m_pMedia->duration() / 1000;
    m_pClipLabel->setText(QString("%1 - %2").arg(convertSecondToTimeFormat(m_clipStartSecond)).arg(convertSecondToTimeFormat(m_clipEndSecond)));
}

void MainWindow::onClip()
{
    QString strStartTime = convertSecondToTimeFormat(m_clipStartSecond);
    QString strEndTime = convertSecondToTimeFormat(m_clipEndSecond);

    QMessageBox box(this);
    box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    box.setText(tr("[%1, %2]").arg(strStartTime).arg(strEndTime));
    if(box.exec() == QMessageBox::Ok)
    {
        QString strOutName = QFileInfo(m_strFileName).absoluteDir().absolutePath() + "/out." + QFileInfo(m_strFileName).suffix();
        QString cmd = QString("ffmpeg -i %1 -vcodec copy -acodec copy -ss %2 -to %3 %4 -y").arg(m_strFileName).arg(strStartTime).arg(strEndTime).arg(strOutName);
        qDebug() << "====" << cmd;

        QMessageBox testMassage;
        testMassage.setText(cmd);
        testMassage.show();
        qApp->processEvents();

        QProcess p;
        p.start("cmd", QStringList() << "/c" << cmd);
        p.waitForStarted();
        p.waitForFinished();
        //QString strTemp=QString::fromLocal8Bit(p.readAllStandardOutput());
    }

}

void MainWindow::backward(int ms)
{
    float delta = (float)ms / m_pMedia->duration();
    m_pPlayer->setPosition(m_pPlayer->position() - delta);
}

void MainWindow::forward(int ms)
{
    float delta = (float)ms / m_pMedia->duration();
    m_pPlayer->setPosition(m_pPlayer->position() + delta);
}

void MainWindow::grabImage(int s)
{
    QString strImagePath = QFileInfo(m_strFileName).dir().absolutePath()  + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".jpg";
    QString cmd = QString("ffmpeg -i %1 -r 1 -ss %2 %3").arg(m_strFileName).arg(s).arg(strImagePath);
    qDebug() << "cmd :" << cmd;
    QProcess p;
    p.start("cmd", QStringList() << "/c" << cmd);
    p.waitForStarted();
    p.waitForFinished();
    QString strTemp = QString::fromLocal8Bit(p.readAllStandardOutput());
    if(!strTemp.isEmpty())
    {
        QMessageBox::warning(this, tr("warning"), strTemp);
    }
    else
    {
        statusBar()->showMessage(tr("Grab success, saved %1").arg(strImagePath));
    }
}

QString MainWindow::convertSecondToTimeFormat(int second)
{
    return QTime(0, 0, 0).addSecs(second).toString("HH:mm:ss");
}
