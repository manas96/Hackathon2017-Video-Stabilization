#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "player.h"
#include <string>
#include "qcustomplot.h"
#include <QVector>
#include <QPen>
#include <deque>
#include <QTimer>

const std::string URL_ORIGINAL = "rtsp://localhost:5454/liveStream.mp4";
const std::string URL_STABILIZED = "rtsp://localhost:5454/liveStream.mp4";
const int FRAMES_WIDTH = 400;
const int DX_START = -1000;
const int DX_WIDTH = 2000;
const int DY_START = -1000;
const int DY_WIDTH = 2000;
const int DA_START = -360;
const int DA_WIDTH = 720;
const int BUTTON_WIDTH = 51;
const int BUTTON_HEIGHT = 31;
const int BUFFER_LIMIT = 60;
const QRect RECT_ORIGINAL_1 = QRect(20, 0, 388, 218);
const QRect RECT_STABILIZED_1 = QRect(20, 340, 388, 218);
const QRect RECT_ORIGINAL_2 = QRect(2, 160, 590, 332);
const QRect RECT_STABILIZED_2 = QRect(594, 160, 590, 332);

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
    std::deque<QPixmap> buffer_original, buffer_stabilized;
    QTimer *timer_original, *timer_stabilized;
    int FPS_ORIGINAL, FPS_STABILIZED;
    int fps_original, fps_stabilized;
    int pos_original, pos_stabilized;
    int dir_original, dir_stabilized;
    bool pause_original, pause_stabilized;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void initGraphs();
    void updateGraphs();
    void toggleGraphs();
    void setButtonPositions();

private slots:
    void updateOriginalStreamUI(QImage img);
    void updateStabilizedStreamUI(QImage img);
    void updateOriginal();
    void updateStabilized();
    void on_pauseButton_original_clicked();
    void on_forwardButton_original_clicked();
    void on_rewindButton_original_clicked();
    void on_jumpforwardButton_original_clicked();
    void on_jumpbackButton_original_clicked();
    void on_pauseButton_stabilized_clicked();
    void on_forwardButton_stabilized_clicked();
    void on_rewindButton_stabilized_clicked();
    void on_jumpforwardButton_stabilized_clicked();
    void on_jumpbackButton_stabilized_clicked();
    void on_graphButton_clicked();
};

#endif
