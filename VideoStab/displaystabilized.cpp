#include "displaystabilized.h"
#include <iostream>
DisplayStabilized::DisplayStabilized(int fps)
{
    this->fps=fps;
    this->delay=(1000/fps);
}

void DisplayStabilized::run(){

    int fNo=0;
    while(true){
        std::cout<<"Frame "<<fNo<<" Displayed"<<"From processed outptu"<<std::endl;
        if (buffer.empty())
        {

           continue;
        }

        frame=buffer.at(0);


        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }

        buffer.pop_front();
        fNo++;
        emit displayImage(img);
        this->msleep(delay);
    }


}

void DisplayStabilized::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}
