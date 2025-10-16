#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void openFile();
    void play();
    void pause();
    void stop();
    void setPosition(int pos);
    void onPositionChanged(qint64 pos);
    void onDurationChanged(qint64 dur);
    void setVolume(int vol);
    void toggleFullScreen();

private:
    void buildUi();
    void wireSignals();

    QMediaPlayer*  m_player {nullptr};
    QAudioOutput*  m_audio  {nullptr};
    QVideoWidget*  m_video  {nullptr};

    QSlider*       m_seekSlider {nullptr};
    QSlider*       m_volSlider  {nullptr};
    QPushButton*   m_btnOpen    {nullptr};
    QPushButton*   m_btnPlay    {nullptr};
    QPushButton*   m_btnPause   {nullptr};
    QPushButton*   m_btnStop    {nullptr};
    QPushButton*   m_btnFull    {nullptr};
    QLabel*        m_timeLabel  {nullptr};

    QString        m_currentFile;
};
