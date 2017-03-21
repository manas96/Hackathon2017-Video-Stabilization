/*
Thanks Nghia Ho for his excellent code.
And,I modified the smooth step using a simple kalman filter .
So,It can processes live video streaming.
modified by chen jia.
email:chenjia2013@foxmail.com
*/
#include "stabilizer.h"
#include "mainwindow.h"
using namespace std;
using namespace cv;

void stabilizer::loadVideo(std::string s)
{
    cap.open(s);
    assert(cap.isOpened());
    if (cap.isOpened())
    {
        frameRate = (int)cap.get(CV_CAP_PROP_FPS);
   }
}

void stabilizer::Play()
{
    if (!isRunning()) {
        if (isStopped()){
            stop = false;
        }
        start(TimeCriticalPriority);
    }
}
void stabilizer::run()
{
    Mat output;
    int delay = (1000/frameRate);

    //---------------------------------------------------------------------------------------
    /* commenting file printing, not needed
    // For further analysis
    ofstream out_transform("prev_to_cur_transformation.txt");
    ofstream out_trajectory("trajectory.txt");
    ofstream out_smoothed_trajectory("smoothed_trajectory.txt");
    ofstream out_new_transform("new_prev_to_cur_transformation.txt");
    */
    Mat cur, cur_grey;
    Mat prev, prev_grey;

    cap >> prev;//get the first frame.ch
    cvtColor(prev, prev_grey, COLOR_BGR2GRAY);

    // Step 1 - Get previous to current frame transformation (dx, dy, da) for all frames
    vector <TransformParam> prev_to_cur_transform; // previous to current
    // Accumulated frame to frame transform
    double a = 0;
    double x = 0;
    double y = 0;
    // Step 2 - Accumulate the transformations to get the image trajectory
    vector <Trajectory> trajectory; // trajectory at all frames
    //
    // Step 3 - Smooth out the trajectory using an averaging window
    vector <Trajectory> smoothed_trajectory; // trajectory at all frames
    Trajectory X;//posteriori state estimate
    Trajectory	X_;//priori estimate
    Trajectory P;// posteriori estimate error covariance
    Trajectory P_;// priori estimate error covariance
    Trajectory K;//gain
    Trajectory	z;//actual measurement
    double pstd = 4e-3;//can be changed
    double cstd = 0.25;//can be changed
    Trajectory Q(pstd,pstd,pstd);// process noise covariance
    Trajectory R(cstd,cstd,cstd);// measurement noise covariance
    // Step 4 - Generate new set of previous to current transform, such that the trajectory ends up being the same as the smoothed trajectory
    vector <TransformParam> new_prev_to_cur_transform;
    //
    // Step 5 - Apply the new transformation to the video
    //cap.set(CV_CAP_PROP_POS_FRAMES, 0);

   // Mat T(2,3,CV_64F); //manas-commented out as it was not used

    int vert_border = HORIZONTAL_BORDER_CROP * prev.rows / prev.cols; // get the aspect ratio correct
   // VideoWriter outputVideo;
    //outputVideo.open("compare.avi" , CV_FOURCC('X','V','I','D'), 24,cvSize(cur.rows, cur.cols*2+10), true);
    //
    int k=1;
    int max_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
    Mat last_T;
    Mat prev_grey_,cur_grey_;
//-----------------------------------------------------------

    while(!stop){
        if (!cap.read(frame))
        {
            stop = true;
        }
//----------------------------------------------------------------------------------------
        cap >> cur;
        //imshow("cam", cur);
        if(cur.data == NULL) {
            break;
        }

        cvtColor(cur, cur_grey, COLOR_BGR2GRAY);

        // vector from prev to cur
        vector <Point2f> prev_corner, cur_corner;
        vector <Point2f> prev_corner2, cur_corner2;
        vector <uchar> status;
        vector <float> err;

        goodFeaturesToTrack(prev_grey, prev_corner, 200, 0.01, 30);



        calcOpticalFlowPyrLK(prev_grey, cur_grey, prev_corner, cur_corner, status, err);

        // weed out bad matches
        for(size_t i=0; i < status.size(); i++) {
            if(status[i]) {
                prev_corner2.push_back(prev_corner[i]);
                cur_corner2.push_back(cur_corner[i]);
            }

        }
        if(prev_corner2.size()>80){//this fixes problems with sudden panning

        //manas TODO-estimateRigidTransform returns garbage value and crashes app when camera is
        //shaken too much. Replace method/handle error

        // translation + rotation only
        Mat T = estimateRigidTransform(prev_corner2, cur_corner2, false); // false = rigid transform, no scaling/shearing

        // in rare cases no transform is found. We'll just use the last known good transform.

        //manas-added condition T.empty(),was getting segfault
        if(T.data == NULL || T.empty()) {
            last_T.copyTo(T);
        }

        T.copyTo(last_T);

        // decompose T

        double dx = T.at<double>(0,2);
        double dy = T.at<double>(1,2);
        double da = atan2(T.at<double>(1,0), T.at<double>(0,0));


        //------------------Plot unstabilized dx,dy,da-----------------------
        if(showGraphs){
        mw->prev_x1 += delay;
        mw->prev_y1 += dx;
        mw->prev_y2 += dy;
       // mw->prev_y3 += da;


        }
        //-------------------------------------------------------------
        //prev_to_cur_transform.push_back(TransformParam(dx, dy, da));

        //out_transform << k << " " << dx << " " << dy << " " << da << endl;
        //
        // Accumulated frame to frame transform
        x += dx;
        y += dy;
        a += da;

        //trajectory.push_back(Trajectory(x,y,a));
        //
        //out_trajectory << k << " " << x << " " << y << " " << a << endl;
        //
        z = Trajectory(x,y,a);
        //
        if(k==1){
            // intial guesses
            X = Trajectory(0,0,0); //Initial estimate,  set 0
            P =Trajectory(1,1,1); //set error variance,set 1
        }
        else
        {
            //time update£¨prediction£©
            X_ = X; //X_(k) = X(k-1);
            P_ = P+Q; //P_(k) = P(k-1)+Q;
            // measurement update£¨correction£©
            K = P_/( P_+R ); //gain;K(k) = P_(k)/( P_(k)+R );
            X = X_+K*(z-X_); //z-X_ is residual,X(k) = X_(k)+K(k)*(z(k)-X_(k));
            P = (Trajectory(1,1,1)-K)*P_; //P(k) = (1-K(k))*P_(k);
        }
        //smoothed_trajectory.push_back(X);
       // out_smoothed_trajectory << k << " " << X.x << " " << X.y << " " << X.a << endl;
        //-
        // target - current
        double diff_x = X.x - x;//
        double diff_y = X.y - y;
        double diff_a = X.a - a;

        dx = dx + diff_x;
        dy = dy + diff_y;
        da = da + diff_a;

        //--------------plot stabilized dx,dy,da-------------------
        if(showGraphs){
            mw->prev_y1Corrected=dx;
            mw->prev_y2Corrected=dy;
        //    mw->prev_y3Corrected=da;
            mw->updatePlots();
        }


        //----------------------------------------------------------
        //new_prev_to_cur_transform.push_back(TransformParam(dx, dy, da));
        //
        //out_new_transform << k << " " << dx << " " << dy << " " << da << endl;
        //
        T.at<double>(0,0) = cos(da);
        T.at<double>(0,1) = -sin(da);
        T.at<double>(1,0) = sin(da);
        T.at<double>(1,1) = cos(da);

        T.at<double>(0,2) = dx;
        T.at<double>(1,2) = dy;

        Mat cur2;

        warpAffine(prev, cur2, T, cur.size());

        cur2 = cur2(Range(vert_border, cur2.rows-vert_border), Range(HORIZONTAL_BORDER_CROP, cur2.cols-HORIZONTAL_BORDER_CROP));
/*
        // Resize cur2 back to cur size, for better side by side comparison
        resize(cur2, cur2, cur.size());



        // Now draw the original and stablised side by side for coolness
        Mat canvas = Mat::zeros(cur.rows, cur.cols*2+10, cur.type());

        prev.copyTo(canvas(Range::all(), Range(0, cur2.cols)));
        cur2.copyTo(canvas(Range::all(), Range(cur2.cols+10, cur2.cols*2+10)));

        // If too big to fit on the screen, then scale it down by 2, hopefully it'll fit :)
        if(canvas.cols > 1920) {
            resize(canvas, canvas, Size(canvas.cols/2, canvas.rows/2));
        }
        //outputVideo<<canvas;
       // imshow("before and after", canvas);

       // waitKey(10);
        //
        */
        cout << "Frame: " << k << "/" << max_frames << " - good optical flow: " << prev_corner2.size() <<" STABILIZED"<< endl;
        k++;

        prev = cur.clone();//cur.copyTo(prev);
        cur_grey.copyTo(prev_grey);

        //TODO :Calculate FPS

        output=cur2;
        }
        else{
              cout << "Frame: " << k << "/" << max_frames << " - good optical flow: " << prev_corner2.size() << endl;
              prev = cur.clone();//cur.copyTo(prev);
              cur_grey.copyTo(prev_grey);

            output=cur;
        }
        if(showTrackedFeatures){
            for (unsigned int i = 0; i < prev_corner2.size(); ++i) {

                circle(output,prev_corner2[i],3,Scalar(0,0,255),CV_FILLED);  //bgr
                circle(output,cur_corner2[i],3,Scalar(255,0,0),CV_FILLED);
            }
        }

        if (output.channels()== 3){
            cv::cvtColor(output, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }
        /*else
        {
            img = QImage((const unsigned char*)(output.data),
                                 output.cols,output.rows,RGBframe.step,QImage::Format_Indexed8);
        }*/
        emit stabilizedImage(img);
        this->msleep(delay);
    }
}

void stabilizer::toggleTrackedFeatures(){
    showTrackedFeatures=!showTrackedFeatures;
}
void stabilizer::toggleGraphs(){
    showGraphs=!showGraphs;
}

stabilizer::stabilizer(MainWindow *mw){
    this->mw = mw;
}

