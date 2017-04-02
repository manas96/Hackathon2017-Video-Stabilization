#include "player.h"

Player::Player(){}

Player::Player(const int servPort, const std::string filePath){
    stop = true;
    this->servPort = servPort;
    this->filePath = filePath;
    img = QImage(1000,1000, QImage::Format_RGB32);
    //qDebug()<<"depth "<<image.depth(); //prints 32
    img.fill(qRgba(0, 0, 0, 255));
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

    while(true){
        try{

            UDPSocket sock(servPort);

            char buffer[BUF_LEN]; // Buffer for echo string
            int recvMsgSize; // Size of received message
            string sourceAddress; // Address of datagram source
            unsigned short sourcePort; // Port of datagram source
            do {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
            } while (recvMsgSize > (int)sizeof(int));
            int total_pack = ((int * ) buffer)[0];

            char * longbuf = new char[PACK_SIZE * total_pack];
            for (int i = 0; i < total_pack; i++) {
                recvMsgSize = sock.recvFrom(buffer, BUF_LEN, sourceAddress, sourcePort);
                if (recvMsgSize != PACK_SIZE) {
                    continue;
                }
                memcpy( & longbuf[i * PACK_SIZE], buffer, PACK_SIZE);
            }

            Mat rawData = Mat(1, PACK_SIZE * total_pack, CV_8UC1, longbuf);
            Mat frame = imdecode(rawData, CV_LOAD_IMAGE_COLOR);
            imwrite(filePath.c_str(), frame);
            if (frame.size().width == 0) {
                cerr << "decode failure!" << endl;
                continue;
            }

            if (frame.channels()== 3){
                img.load(filePath.c_str());
            }
            if(!img.isNull())
                emit originalImage(img);

        }catch (SocketException & e) {
            cerr << e.what() << endl;
            exit(1);
        }
    }


    /*while(!stop){
        if (!capture.read(frame))
            stop = true;
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,RGBframe.step,QImage::Format_RGB888);
        }
        if(!img.isNull())
            emit originalImage(img);
    }*/
}

bool Player::loadVideo(){
    /*capture.open(URL);
    if (capture.isOpened())
    {
        FPS = (int)capture.get(CV_CAP_PROP_FPS);
        return true;
    }
    else
        return false;*/
    return true;
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
