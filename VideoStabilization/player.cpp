#include "player.h"

Player::Player(){}

Player::Player(const std::string URL){
    stop = true;
    this->URL = URL;
}

Player::~Player(){
    mutex.lock();
    stop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void Player::run(){
    const int FRAME_DELAY = (1000/FPS);
    while(!stop){
        if (!capture.read(frame))
            stop = true;
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }
        emit originalImage(img);
        msleep(FRAME_DELAY);
    }
}

bool Player::loadVideo(){
    capture.open(URL);
    if (capture.isOpened())
    {
        FPS = (int)capture.get(CV_CAP_PROP_FPS);
        return true;
    }
    else
        return false;
}

void Player::playVideo(){
    if(!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

void Player::msleep(int ms){
    struct timespec ts = {ms / 1000, (ms % 1000) * 1000 * 1000};
    nanosleep(&ts, NULL);
}

void Player::stopVideo(){ stop = true; }

bool Player::isStopped(){ return stop; }
