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

    ui->video->setMediaPlayer(m_pPlayer);
    ui->volume->setMediaPlayer(m_pPlayer);
    ui->volume->setVolume(100);
    ui->volume->setVisible(false);
    ui->seek->setMediaPlayer(m_pPlayer);

    ui->mainToolBar->addAction(ui->action_Open);
    ui->mainToolBar->addAction(ui->action_Full_Screen);

    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(ui->action_Full_Screen, SIGNAL(triggered()), this, SLOT(onFullScreen()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete m_pPlayer;
    delete m_pMedia;
    delete m_pInstance;
    delete ui;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_F:
        onFullScreen();
        break;
    case Qt::Key_Left:
        backward(10 * 1000);
        break;
    case Qt::Key_Right:
        forward(10 * 1000);
        break;
    }
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e);
    QMenu contextMenu(this);
    contextMenu.addAction(ui->action_Full_Screen);
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
    m_pMedia = new VlcMedia(strFileName, true, m_pInstance);
    m_pPlayer->open(m_pMedia);
    statusBar()->showMessage(strFileName);
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
