#ifndef STABILIZER_H
#define STABILIZER_H

#include "player.h"
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "mainwindow.h"
#include <cmath>

class MainWindow;

using namespace cv;

const int HORIZONTAL_BORDER_CROP = 20;
const int MIN_POINTS_FOR_STAB = 80;

struct TransformParam{
    double dx,dy,da;

    TransformParam(){}

    TransformParam(double dx, double dy, double da){
        this->dx = dx;
        this->dy = dy;
        this->da = da;
    }
};

struct Trajectory{
    double x,y,a;

    Trajectory(){}

    Trajectory(double x, double y, double a){
        this->x = x;
        this->y = y;
        this->a = a;
    }

    friend Trajectory operator+(const Trajectory &c1,const Trajectory  &c2){
        return Trajectory(c1.x+c2.x,c1.y+c2.y,c1.a+c2.a);
    }

    friend Trajectory operator-(const Trajectory &c1,const Trajectory  &c2){
        return Trajectory(c1.x-c2.x,c1.y-c2.y,c1.a-c2.a);
    }
    friend Trajectory operator*(const Trajectory &c1,const Trajectory  &c2){
        return Trajectory(c1.x*c2.x,c1.y*c2.y,c1.a*c2.a);
    }
    friend Trajectory operator/(const Trajectory &c1,const Trajectory  &c2){
        return Trajectory(c1.x/c2.x,c1.y/c2.y,c1.a/c2.a);
    }
    Trajectory operator =(const Trajectory &rx){
        x = rx.x;
        y = rx.y;
        a = rx.a;
        return Trajectory(x,y,a);
    }

};

class Stabilizer:public Player
{
    Q_OBJECT
signals:
    void stabilizedImage(const QImage &image);
protected:
    void run();
    Mat output, cur, cur_grey, prev, prev_grey, last_T, prev_grey_, cur_grey_;
    double a,x,y;
    std::vector <TransformParam> prev_to_cur_transform, new_prev_to_cur_transform;
    std::vector <Trajectory> trajectory, smoothed_trajectory;
    Trajectory X, X_, P, P_, K, Z, Q, R;
    double ptsd,ctsd;
    int vert_border;
    int counter;
    int max_frames;
    MainWindow *mw;

public:
    Stabilizer(const std::string URL, MainWindow *mw);
    void init();
};

#endif // STABILIZER_H
