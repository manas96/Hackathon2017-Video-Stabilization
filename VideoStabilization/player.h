#ifndef PLAYER_H
#define PLAYER_H

#include "PracticalSocket.h"
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

#define BUF_LEN 65540
#define FRAME_HEIGHT 480
#define FRAME_WIDTH 640
#define FRAME_INTERVAL (1000/30)
#define PACK_SIZE 4096 //udp pack size; note that OSX limits < 8100 bytes
#define ENCODE_QUALITY 80

using namespace cv;

class Player:public QThread
{
    Q_OBJECT
signals:
    void originalImage(const QImage &image);

protected:
    void run();
    int FPS;
    std::string URL, filePath;
    bool stop;
    Mat frame, RGBframe;
    QMutex mutex;
    QWaitCondition condition;
    VideoCapture capture;
    QImage img;
    int servPort;

public:
    Player();
    Player(const int servPort, const std::string filePath);
    ~Player();
    bool loadVideo();
    void playVideo();
    void stopVideo();
    bool isStopped();
    int getFPS();
};

#endif // PLAYER_H
