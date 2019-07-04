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
    m_bIsMaximized(false)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    m_pInstance = new VlcInstance(VlcCommon::args(), this);
    m_pPlayer = new VlcMediaPlayer(m_pInstance);
    m_pPlayer->setVideoWidget(ui->video);

    ui->action_Open->setIcon(QIcon(":/img/open.png"));
    ui->action_Full_Screen->setIcon(QIcon(":/img/full_screen.png"));
    ui->action_Exit->setIcon(QIcon(":/img/exit.png"));
    ui->action_About->setIcon(QIcon(":/img/about.png"));
    ui->action_Grab->setIcon(QIcon(":/img/grab.png"));

    ui->video->setMediaPlayer(m_pPlayer);
    ui->volume->setMediaPlayer(m_pPlayer);
    ui->volume->setVolume(100);
    ui->volume->setVisible(false);
    ui->seek->setMediaPlayer(m_pPlayer);

    ui->mainToolBar->addAction(ui->action_Open);
    ui->mainToolBar->addAction(ui->action_Full_Screen);
    ui->mainToolBar->addAction(ui->action_Grab);

    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(ui->action_Full_Screen, SIGNAL(triggered()), this, SLOT(onFullScreen()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->action_Grab, SIGNAL(triggered()), this, SLOT(onGrabImage()));
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
    statusBar()->showMessage(strFilePath);
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
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
    QMenu contextMenu(this);
    contextMenu.addAction(ui->action_Full_Screen);
    contextMenu.addAction(ui->action_Grab);
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
    QString strImagePath = QFileInfo(m_strFileName).dir().absolutePath()  + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
    QString cmd = QString("ffmpeg -i %1 -r 1 -ss %2 %3.jpg").arg(m_strFileName).arg(s).arg(strImagePath);
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
