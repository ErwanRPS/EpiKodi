#include "mainwindow.hpp"
#include <QApplication>
#include <QFileInfo>
#include <QTime>
#include <QDir>

static QString mmss(qint64 ms) {
    const QTime t = QTime::fromMSecsSinceStartOfDay(static_cast<int>(ms));
    if (ms >= 60'000) return t.toString("mm:ss");
    return t.toString("m:ss");
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    buildUi();
    wireSignals();

    // Option: charger une vidéo de test si présente
    const QString probe = QDir::current().absoluteFilePath("assets/video/test.mp4");
    if (QFileInfo::exists(probe)) {
        m_currentFile = probe;
        m_player->setSource(QUrl::fromLocalFile(m_currentFile));
    }
}

void MainWindow::buildUi() {
    // Player + audio
    m_player = new QMediaPlayer(this);
    m_audio  = new QAudioOutput(this);
    m_player->setAudioOutput(m_audio);

    // Vidéo
    m_video = new QVideoWidget(this);
    m_player->setVideoOutput(m_video);

    // Contrôles
    m_btnOpen  = new QPushButton("Ouvrir", this);
    m_btnPlay  = new QPushButton("Play", this);
    m_btnPause = new QPushButton("Pause", this);
    m_btnStop  = new QPushButton("Stop", this);
    m_btnFull  = new QPushButton("Fullscreen", this);

    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setRange(0, 0);

    m_volSlider  = new QSlider(Qt::Horizontal, this);
    m_volSlider->setRange(0, 100);
    m_volSlider->setValue(60);
    m_audio->setVolume(0.60); // volume en 0.0 -> 1.0

    m_timeLabel = new QLabel("0:00 / 0:00", this);

    // Layouts
    auto* central = new QWidget(this);
    auto* vbox = new QVBoxLayout(central);
    vbox->setContentsMargins(8, 8, 8, 8);
    vbox->setSpacing(8);

    vbox->addWidget(m_video, /*stretch*/ 1);

    auto* controls1 = new QHBoxLayout();
    controls1->addWidget(m_btnOpen);
    controls1->addSpacing(12);
    controls1->addWidget(m_btnPlay);
    controls1->addWidget(m_btnPause);
    controls1->addWidget(m_btnStop);
    controls1->addSpacing(12);
    controls1->addWidget(new QLabel("Volume:", this));
    controls1->addWidget(m_volSlider, /*stretch*/ 1);
    controls1->addSpacing(12);
    controls1->addWidget(m_btnFull);

    auto* controls2 = new QHBoxLayout();
    controls2->addWidget(m_seekSlider, /*stretch*/ 1);
    controls2->addSpacing(8);
    controls2->addWidget(m_timeLabel);

    vbox->addLayout(controls2);
    vbox->addLayout(controls1);

    setCentralWidget(central);
    setWindowTitle("EpiKodi – M1 Player");
    resize(1000, 620);
}

void MainWindow::wireSignals() {
    // Boutons
    connect(m_btnOpen,  &QPushButton::clicked, this, &MainWindow::openFile);
    connect(m_btnPlay,  &QPushButton::clicked, this, &MainWindow::play);
    connect(m_btnPause, &QPushButton::clicked, this, &MainWindow::pause);
    connect(m_btnStop,  &QPushButton::clicked, this, &MainWindow::stop);
    connect(m_btnFull,  &QPushButton::clicked, this, &MainWindow::toggleFullScreen);

    // Sliders
    connect(m_seekSlider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(m_volSlider,  &QSlider::valueChanged, this, &MainWindow::setVolume);

    // Player -> UI
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
}

void MainWindow::openFile() {
    const QString file = QFileDialog::getOpenFileName(
        this, "Ouvrir une vidéo",
        m_currentFile.isEmpty() ? QDir::homePath() : QFileInfo(m_currentFile).absolutePath(),
        "Vidéos (*.mp4 *.mkv *.avi *.mov);;Tous les fichiers (*.*)"
    );
    if (file.isEmpty()) return;

    m_currentFile = file;
    m_player->setSource(QUrl::fromLocalFile(m_currentFile));
    play();
}

void MainWindow::play()  { m_player->play(); }
void MainWindow::pause() { m_player->pause(); }
void MainWindow::stop()  { m_player->stop(); }

void MainWindow::setPosition(int pos) {
    // Slider -> player (ms)
    m_player->setPosition(static_cast<qint64>(pos));
}

void MainWindow::onPositionChanged(qint64 pos) {
    // Player -> slider + time label
    if (!m_seekSlider->isSliderDown())
        m_seekSlider->setValue(static_cast<int>(pos));

    const qint64 dur = m_player->duration();
    m_timeLabel->setText(QString("%1 / %2").arg(mmss(pos), mmss(dur)));
}

void MainWindow::onDurationChanged(qint64 dur) {
    m_seekSlider->setRange(0, static_cast<int>(dur));
    // on rafraîchit aussi le label
    m_timeLabel->setText(QString("%1 / %2").arg(mmss(m_player->position()), mmss(dur)));
}

void MainWindow::setVolume(int vol) {
    // QAudioOutput attend [0.0 ; 1.0]
    m_audio->setVolume(qBound(0, vol, 100) / 100.0);
}

void MainWindow::toggleFullScreen() {
    if (m_video->isFullScreen()) {
        m_video->setFullScreen(false);
        showNormal();
    } else {
        m_video->setFullScreen(true);
    }
}
