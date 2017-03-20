#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <string>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);
    originalPlayer = new Player();
    stabStream=new stabilizer(this);
    QObject::connect(originalPlayer, SIGNAL(originalImage(QImage)),
                                  this, SLOT(updateOriginalPlayerUI(QImage)));
    QObject::connect(stabStream,SIGNAL(stabilizedImage(QImage)),
                                       this, SLOT(updateStabilizedPlayerUI(QImage)));

    prev_x1 = 0;
    prev_y1 = 0;
    prev_y2 = 0;
    prev_y3 = 0;
    x_start = 0;
    x_width = 20000;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    x1.append(0.0);
    y1.append(0.0);
    ui->customPlot->graph(0)->setData(x1,y1);
    ui->customPlot->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot->yAxis->setRange(-1000,+1000);

    ui->customPlot_2->addGraph();
    ui->customPlot_2->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot_2->graph(0)->setLineStyle(QCPGraph::lsLine);
    y2.append(0.0);
    ui->customPlot_2->graph(0)->setData(x1,y2);
    ui->customPlot_2->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot_2->yAxis->setRange(-1000,+1000);

    ui->customPlot_3->addGraph();
    ui->customPlot_3->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->customPlot_3->graph(0)->setLineStyle(QCPGraph::lsLine);
    y3.append(0.0);
    ui->customPlot_3->graph(0)->setData(x1,y3);
    ui->customPlot_3->xAxis->setRange(x_start,x_start+x_width);      //Time in seconds.
    ui->customPlot_3->yAxis->setRange(-1000,+1000);


    //"/home/manas/Desktop/Video stabilization/samples/sample2.mp4"
    //"/videofeed?dummy=param.mjpg"
    std::string url="http://192.168.0.100:8080/videofeed?dummy=param.mjpg";
    originalPlayer->loadVideo(url);
    stabStream->loadVideo(url);
    originalPlayer->Play();
    stabStream->Play();

}

void MainWindow::updatePlots(){
    x1.append(prev_x1);
    y1.append(prev_y1);
    y2.append(prev_y2);
    y3.append(prev_y3);
    ui->customPlot->graph(0)->setData(x1,y1);
    ui->customPlot_2->graph(0)->setData(x1,y2);
    ui->customPlot_3->graph(0)->setData(x1,y3);
    if(prev_x1 > 18000){
        x_start = prev_x1 - 18000;
        ui->customPlot->xAxis->setRange(x_start, x_start+x_width);
        ui->customPlot_2->xAxis->setRange(x_start, x_start+x_width);
        ui->customPlot_3->xAxis->setRange(x_start, x_start+x_width);
    }
    ui->customPlot->replot();
    ui->customPlot_2->replot();
    ui->customPlot_3->replot();   //THIS LINE IS EVIL
    ui->customPlot->update();
    ui->customPlot_2->update();
    ui->customPlot_3->update();
}

void MainWindow::updateOriginalPlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label->setAlignment(Qt::AlignCenter);
        ui->label->setPixmap(QPixmap::fromImage(img).scaled(ui->label->size(),
                                           Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}
void MainWindow::updateStabilizedPlayerUI(QImage img)
{
    if (!img.isNull())
    {
        ui->label_2->setAlignment(Qt::AlignCenter);
        ui->label_2->setPixmap(QPixmap::fromImage(img).scaled(ui->label_2->size(),
                                           Qt::KeepAspectRatio, Qt::FastTransformation));
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
