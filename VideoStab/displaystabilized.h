#ifndef DISPLAYSTABILIZED_H
#define DISPLAYSTABILIZED_H
#include "player.h"
#include <QVector>
#include <opencv2/imgproc/imgproc.hpp>
#include "stabilizer.h"
using namespace cv;
class DisplayStabilized: public Player
{

    Q_OBJECT

    signals:
    //Signal to output frame to be displayed
         void displayImage(const QImage &image);

public:
    DisplayStabilized(int fps);
    QVector<Mat> buffer;
    int fps,delay;
    Mat frame;
    void run();
    void Play();
};

#endif // DISPLAYSTABILIZED_H
