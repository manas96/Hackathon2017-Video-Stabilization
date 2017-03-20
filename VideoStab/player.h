#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
class Player : public QThread
{    Q_OBJECT
 //private:

 signals:
 //Signal to output frame to be displayed
      void originalImage(const QImage &image);

 protected:
     void run();
     void msleep(int ms);
     bool stop;
     QMutex mutex;
     QWaitCondition condition;
     Mat frame;
     int frameRate;
     VideoCapture capture;
     Mat RGBframe;
     QImage img;
 public:
    //Constructor
    Player(QObject *parent = 0);
    //Destructor
    ~Player();
    //Load a video from memory
    bool loadVideo(std::string filename);
    //Play the video
    void Play();
    //Stop the video
    void Stop();
    //check if the player has been stopped
    bool isStopped() const;
    VideoCapture getStream();
};

#endif // PLAYER_H
