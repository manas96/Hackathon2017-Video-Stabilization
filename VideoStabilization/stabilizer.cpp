#include "stabilizer.h"

Stabilizer::Stabilizer(const std::string URL, MainWindow *mw){
    stop = true;
    this->URL = URL;
    this->mw = mw;
}

void Stabilizer::run(){
    init();
    const int FRAME_DELAY = (1000/FPS);
    while(!stop){
        if (!capture.read(frame))
            stop = true;

        capture >> cur;

        if(cur.data == NULL)
            break;

        cvtColor(cur, cur_grey, COLOR_BGR2GRAY);

        std::vector <Point2f> prev_corner, cur_corner;
        std::vector <Point2f> prev_corner2, cur_corner2;
        std::vector <uchar> status;
        std::vector <float> err;

        goodFeaturesToTrack(prev_grey, prev_corner, 200, 0.01, 30);

        calcOpticalFlowPyrLK(prev_grey, cur_grey, prev_corner, cur_corner, status, err);

        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                prev_corner2.push_back(prev_corner[i]);
                cur_corner2.push_back(cur_corner[i]);
            }
        }

        if(prev_corner2.size() > MIN_POINTS_FOR_STAB){

            Mat T = estimateRigidTransform(prev_corner2, cur_corner2, false);
            if(T.data == NULL || T.empty()) {
                last_T.copyTo(T);
            }

            T.copyTo(last_T);

            double dx = T.at<double>(0,2);
            double dy = T.at<double>(1,2);
            double da = atan2(T.at<double>(1,0), T.at<double>(0,0));

            mw->frames_val += 1;
            mw->dx_val += dx;
            mw->dy_val += dy;
            mw->da_val += da * (180.0 / M_PI);

            x += dx;
            y += dy;
            a += da;

            Z = Trajectory(x,y,a);

            if(counter==1){
                X = Trajectory(0,0,0);
                P =Trajectory(1,1,1);
            }
            else
            {
                X_ = X;
                P_ = P+Q;
                K = P_/( P_+R );
                X = X_+K*(Z-X_);
                P = (Trajectory(1,1,1)-K)*P_;
            }
            double diff_x = X.x - x;
            double diff_y = X.y - y;
            double diff_a = X.a - a;

            dx = dx + diff_x;
            dy = dy + diff_y;
            da = da + diff_a;

            mw->dx_corrected_val = dx;
            mw->dy_corrected_val = dy;
            mw->da_corrected_val = da * (180.0 / M_PI);
            mw->updateGraphs();

            T.at<double>(0,0) = cos(da);
            T.at<double>(0,1) = -sin(da);
            T.at<double>(1,0) = sin(da);
            T.at<double>(1,1) = cos(da);

            T.at<double>(0,2) = dx;
            T.at<double>(1,2) = dy;

            Mat cur2;

            warpAffine(prev, cur2, T, cur.size());

            cur2 = cur2(Range(vert_border, cur2.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, cur2.cols-HORIZONTAL_BORDER_CROP));

            counter++;

            prev = cur.clone();
            cur_grey.copyTo(prev_grey);
            output=cur2;

        }
        else{
            prev = cur.clone();
            cur_grey.copyTo(prev_grey);
            output=cur;
        }
        //if(showTrackedFeatures){
            for (unsigned int i = 0; i < prev_corner2.size(); ++i) {
                circle(output,prev_corner2[i],3,Scalar(0,0,255),CV_FILLED);
                circle(output,cur_corner2[i],3,Scalar(255,0,0),CV_FILLED);
            }
        //}

        if (output.channels()== 3){
            cv::cvtColor(output, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }

        emit stabilizedImage(img);

        this->msleep(FRAME_DELAY);
    }

}

void Stabilizer::init(){
    capture >> prev;
    cvtColor(prev, prev_grey, COLOR_BGR2GRAY);
    x = 0.0;
    y = 0.0;
    a = 0.0;
    ptsd = 4e-3;
    ctsd = 0.25;
    Q = Trajectory(ptsd, ptsd, ptsd);
    R = Trajectory(ctsd, ctsd, ctsd);
    vert_border = HORIZONTAL_BORDER_CROP * prev.rows / prev.cols;
    counter = 1;
    max_frames = capture.get(CV_CAP_PROP_FRAME_COUNT);
}

