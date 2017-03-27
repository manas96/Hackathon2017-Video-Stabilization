#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);

    prev_x1 = 0;
    prev_y1 = 0;
    prev_y2 = 0;
   // prev_y3 = 0;
    x_start = 0;
    x_width = 10000;

    y1_start = -1000;
    y1_width = 2000;
    y2_start = -1000;
    y2_width = 2000;

    ui->customPlot->addGraph();
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    x1.append(0.0);
    y1.append(0.0);
    ui->customPlot->graph(0)->setData(x1,y1);
    ui->customPlot->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot->yAxis->setRange(y1_start,y1_start+y1_width);

    ui->customPlot_2->addGraph();
    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot_2->graph(0)->setLineStyle(QCPGraph::lsLine);
    y2.append(0.0);
    ui->customPlot_2->graph(0)->setData(x1,y2);
    ui->customPlot_2->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot_2->yAxis->setRange(y2_start,y2_start+y2_width);

    /*
    ui->customPlot_3->addGraph();
    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot_3->graph(0)->setLineStyle(QCPGraph::lsLine);
    y3.append(0.0);
    ui->customPlot_3->graph(0)->setData(x1,y3);
    ui->customPlot_3->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot_3->yAxis->setRange(-1000,+1000);
    */
    ui->customPlot->yAxis->setLabel("horizontal displacement");
    ui->customPlot->xAxis->setLabel("frames");
    ui->customPlot_2->yAxis->setLabel("vertical displacement");
    ui->customPlot_2->xAxis->setLabel("frames");
 //   ui->customPlot_3->yAxis->setLabel("angular displacement");
   // ui->customPlot_3->xAxis->setLabel("time");


    originalPlayer = new Player();

    displayStream= new DisplayStabilized(30);
    stabStream=new stabilizer(this,displayStream);
    QObject::connect(originalPlayer, SIGNAL(originalImage(QImage)),
                                  this, SLOT(updateOriginalPlayerUI(QImage)));
    QObject::connect(stabStream,SIGNAL(stabilizedImage(QImage)),
                                       this, SLOT(updateStabilizedPlayerUI(QImage)));

    QObject::connect(displayStream, SIGNAL(displayImage(QImage)),
                     this,SLOT(renderImage(QImage)));

    red=new QPen(QColor(255,0,0));
    blue=new QPen(QColor(0,0,255));
    //"/home/manas/Desktop/Video stabilization/samples/sample2.mp4"
    //"/videofeed?dummy=param.mjpg"
   // std::string url="http://192.168.0.100:8080/videofeed?dummy=param.mjpg";
    std::string url="/home/manas/Downloads/v2.mp4";
    originalPlayer->loadVideo(url);
    stabStream->loadVideo(url);
    originalPlayer->Play();
    stabStream->Play();
    displayStream->Play();

}

void MainWindow::updatePlots(){
    x1.append(prev_x1);
    y1.append(prev_y1);
    y2.append(prev_y2);
    //y3.append(prev_y3);

    y1Corrected.append(prev_y1Corrected);
    y2Corrected.append(prev_y2Corrected);
    //y3Corrected.append(prev_y3Corrected);

    ui->customPlot->graph(0)->setPen(*red);
    ui->customPlot_2->graph(0)->setPen(*red);
    //ui->customPlot_3->graph(0)->setPen(*red);

    ui->customPlot->graph(0)->setData(x1,y1);
    ui->customPlot_2->graph(0)->setData(x1,y2);
  //  ui->customPlot_3->graph(0)->setData(x1,y3);

    ui->customPlot->graph(1)->setPen(*blue);
    ui->customPlot_2->graph(1)->setPen(*blue);
//    ui->customPlot_3->graph(1)->setPen(*blue);

    ui->customPlot->graph(1)->setData(x1,y1Corrected);
    ui->customPlot_2->graph(1)->setData(x1,y2Corrected);
  //  ui->customPlot_3->graph(1)->setData(x1,y3Corrected);

    if(prev_x1 > 9000){
        x_start = prev_x1 - 9000;
        ui->customPlot->xAxis->setRange(x_start, x_start+x_width);
        ui->customPlot_2->xAxis->setRange(x_start, x_start+x_width);
        //ui->customPlot_3->xAxis->setRange(x_start, x_start+x_width);
    }
    if(prev_y1 > 800 || prev_y1Corrected > 800){
        y1_start = std::max(prev_y1, prev_y1Corrected) - 1800;
        ui->customPlot->yAxis->setRange(y1_start, y1_start+y1_width);
    }
    else if(prev_y1 < -800 || prev_y1Corrected < -800){
        y1_start = std::min(prev_y1, prev_y1Corrected) -200;
        ui->customPlot->yAxis->setRange(y1_start, y1_start+y1_width);
    }
    else{
        y1_start = -1000;
        ui->customPlot->yAxis->setRange(y1_start, y1_start+y1_width);
    }
    if(prev_y2 > 800 || prev_y2Corrected > 800){
        y2_start = std::max(prev_y2, prev_y2Corrected) - 1800;
        ui->customPlot_2->yAxis->setRange(y2_start, y2_start+y2_width);
    }
    else if(prev_y2 < -800 || prev_y2Corrected < -800){
        y2_start = std::min(prev_y2, prev_y2Corrected) -200;
        ui->customPlot_2->yAxis->setRange(y2_start, y2_start+y2_width);
    }
    else{
        y2_start = -1000;
        ui->customPlot_2->yAxis->setRange(y2_start, y2_start+y2_width);
    }

    ui->customPlot->replot();
    ui->customPlot_2->replot();
  //  ui->customPlot_3->replot();   //THIS LINE IS EVIL
    ui->customPlot->update();
    ui->customPlot_2->update();
   // ui->customPlot_3->update();



}

void MainWindow::updateOriginalPlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label->setAlignment(Qt::AlignCenter);
        ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
void MainWindow::updateStabilizedPlayerUI(QImage img)
{




    /*
    if (!img.isNull())
    {
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(img).scaled(ui->label_2->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }*/
}
void MainWindow::renderImage(QImage img){

    if (!img.isNull())
    {
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(img).scaled(ui->label_2->size(),
                                           Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkBox_clicked()
{
    stabStream->toggleTrackedFeatures();
}

void MainWindow::on_checkBox_2_clicked()
{
    stabStream->toggleGraphs();
}
