#include "player.h"
#include <iostream>
Player::Player(QObject *parent)
 : QThread(parent)
{
    stop = true;
}
VideoCapture Player::getStream()
{
    return capture;
}

bool Player::loadVideo(std::string filename) {

    capture.open(filename);
    /*Not working:
    capture.set(CV_CAP_PROP_FRAME_WIDTH,1280);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,720);
    */
    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CV_CAP_PROP_FPS);
       // frameRate=30;
        return true;
    }
    else
        return false;
}

void Player::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

void Player::run()
{
    int delay = (1000/frameRate);
    int fno=0;
    while(!stop){
        if (!capture.read(frame))
        {
            stop = true;
  //          std::cout<<"NULL F OUND AT  ____------------- "<<fno<<std::endl;
        }
        fno++;
        std::cout<<"Frame "<<fno<<" displayed from raw input"<<std::endl;
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }
        /*
        else
        {
            img = QImage((const unsigned char*)(frame.data),
                                 frame.cols,frame.rows,RGBframe.step,QImage::Format_Indexed8);
        }*/
        emit originalImage(img);
        this->msleep(1000/30);
    }
}

Player::~Player()
{
    mutex.lock();
    stop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}
void Player::Stop()
{
    stop = true;
}
void Player::msleep(int ms){
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}
bool Player::isStopped() const{
    return this->stop;
}

