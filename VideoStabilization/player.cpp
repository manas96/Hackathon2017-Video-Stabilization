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

    while(!stop){
        if (!capture.read(frame))
            stop = true;
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }
        if(!img.isNull())
            emit originalImage(img);
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

int Player::getFPS(){ return FPS; }

void Player::stopVideo(){ stop = true; }

bool Player::isStopped(){ return stop; }
