#ifndef PLAYER_H
#define PLAYER_H

#include <QThread>
#include <string>
#include <QWaitCondition>
#include <QImage>
#include <QMutex>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <deque>
#include <iostream>

using namespace cv;

class Player:public QThread
{
    Q_OBJECT
signals:
    void originalImage(const QImage &image);

protected:
    void run();
    int FPS;
    std::string URL;
    bool stop;
    Mat frame, RGBframe;
    QMutex mutex;
    QWaitCondition condition;
    VideoCapture capture;
    QImage img;

public:
    Player();
    Player(const std::string URL);
    ~Player();
    bool loadVideo();
    void playVideo();
    void stopVideo();
    bool isStopped();
    int getFPS();
};

#endif // PLAYER_H
