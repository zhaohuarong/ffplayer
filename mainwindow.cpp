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
    _instance(nullptr),
    _media(nullptr),
    _player(nullptr),
    m_bIsMaximized(false)
{
    ui->setupUi(this);

    setContextMenuPolicy(Qt::DefaultContextMenu);

    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);
    _player->setVideoWidget(ui->video);

    ui->action_Open->setIcon(QIcon(":/img/open.png"));
    ui->action_Full_Screen->setIcon(QIcon(":/img/full_screen.png"));
    ui->action_Exit->setIcon(QIcon(":/img/exit.png"));
    ui->action_About->setIcon(QIcon(":/img/about.png"));

    ui->video->setMediaPlayer(_player);
    ui->volume->setMediaPlayer(_player);
    ui->volume->setVolume(100);
    ui->volume->setVisible(false);
    ui->seek->setMediaPlayer(_player);

    ui->mainToolBar->addAction(ui->action_Open);
    ui->mainToolBar->addAction(ui->action_Full_Screen);

    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(ui->action_Full_Screen, SIGNAL(triggered()), this, SLOT(onFullScreen()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(onAbout()));
    connect(ui->action_Exit, SIGNAL(triggered()), this, SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete _player;
    delete _media;
    delete _instance;
    delete ui;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_F:
        onFullScreen();
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

void MainWindow::onOpenFile()
{
    QString strFileName = QFileDialog::getOpenFileName(this, tr("Select file"), qApp->applicationFilePath());
    if(strFileName.isEmpty())
        return;
    _media = new VlcMedia(strFileName, true, _instance);
    _player->open(_media);
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
