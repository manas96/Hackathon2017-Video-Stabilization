#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"
#include "stabilizer.h"
#include "display.h"
#include <string>
#include "qcustomplot.h"
#include <QVector>
#include <QPen>
#include <deque>
#include <QTimer>

const std::string URL = "rtsp://localhost:5454/liveStream.mp4";//"http://192.168.0.26:8080/videofeed?dummy=param.mjpg";
const int FRAMES_WIDTH = 400;
const int DX_START = -1000;
const int DX_WIDTH = 2000;
const int DY_START = -1000;
const int DY_WIDTH = 2000;
const int DA_START = -360;
const int DA_WIDTH = 720;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    friend class Stabilizer;

    Ui::MainWindow *ui;
    Player *original_stream;
    Player *stabilized_stream;
    Display *original_display;
    double frames_start, dx_start, dy_start, da_start;
    double dx_val, dx_corrected_val;
    double dy_val, dy_corrected_val;
    double da_val, da_corrected_val;
    double frames_val;
    QVector<double> dx, dx_corrected;
    QVector<double> dy, dy_corrected;
    QVector<double> da, da_corrected;
    QVector<double> frames;
    QPen *blue, *red;
    QBrush *background;
    std::deque<QPixmap> buffer;
    int FPS;
    int pos;
    bool pause;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initGraphs();
    void updateGraphs();

private slots:
    void updateOriginalStreamUI(QImage img);
    void updateStabilizedStreamUI(QImage img);
    void update();
    void on_reverseButton_clicked();
    void on_pauseButton_clicked();
    void on_forwardButton_clicked();
};

#endif
