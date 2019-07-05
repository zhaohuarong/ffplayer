#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QLabel;
class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void openFile(const QString &strFilePath);

protected:
    void keyReleaseEvent(QKeyEvent *e);
    void contextMenuEvent(QContextMenuEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private slots:
    void onOpenFile();
    void onFullScreen();
    void onAbout();
    void onGrabImage();
    void onClipLeft();
    void onClipRight();
    void onClip();

private:
    void backward(int ms);
    void forward(int ms);
    void grabImage(int s);
    inline QString convertSecondToTimeFormat(int second);

private:
    Ui::MainWindow *ui;

    VlcInstance *m_pInstance;
    VlcMedia *m_pMedia;
    VlcMediaPlayer *m_pPlayer;
    bool m_bIsMaximized;
    QString m_strFileName;
    int m_clipStartSecond;
    int m_clipEndSecond;
    QLabel *m_pClipLabel;
};

#endif // MAINWINDOW_H
