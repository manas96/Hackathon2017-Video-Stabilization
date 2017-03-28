#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initGraphs();

    original_stream = new Player(URL);
    stabilized_stream = new Stabilizer(URL, this);

    connect(original_stream, SIGNAL(originalImage(QImage)),
                this, SLOT(updateOriginalStreamUI(QImage)));
    connect(stabilized_stream, SIGNAL(stabilizedImage(QImage)),
                this, SLOT(updateStabilizedStreamUI(QImage)));

    original_stream->loadVideo();
    stabilized_stream->loadVideo();

    original_stream->playVideo();
    stabilized_stream->playVideo();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateOriginalStreamUI(QImage img){
    if(!img.isNull())
    {
        ui->label_original->setAlignment(Qt::AlignCenter);
        ui->label_original->setPixmap(QPixmap::fromImage(img).scaled(ui->label_original->size(),
                                           Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void MainWindow::updateStabilizedStreamUI(QImage img){
    if(!img.isNull())
    {
        ui->label_stabilized->setAlignment(Qt::AlignCenter);
        ui->label_stabilized->setPixmap(QPixmap::fromImage(img).scaled(ui->label_stabilized->size(),
                                           Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void MainWindow::initGraphs(){

    red = new QPen(QColor(255,0,0));
    blue = new QPen(QColor(0,0,255));

    dx.clear();
    dx_corrected.clear();
    dy.clear();
    dy_corrected.clear();
    da.clear();
    da_corrected.clear();
    frames.clear();

    dx_val = 0.0;
    dx_corrected_val = 0.0;
    dy_val = 0.0;
    dy_corrected_val = 0.0;
    da_val = 0.0;
    da_corrected_val = 0.0;
    frames_val = 0.0;

    frames_start = 0;
    dx_start = DX_START;
    dy_start = DY_START;
    da_start = DA_START;

    dx.append(0.0);
    dx_corrected.append(0.0);
    dy.append(0.0);
    dy_corrected.append(0.0);
    da.append(0.0);
    da_corrected.append(0.0);
    frames.append(0.0);

    ui->plot_dx->addGraph();
    ui->plot_dx->addGraph();
    ui->plot_dx->graph(0)->setPen(*red);
    ui->plot_dx->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_dx->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dx->graph(0)->setData(frames, dx);
    ui->plot_dx->graph(1)->setPen(*blue);
    ui->plot_dx->graph(1)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_dx->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dx->graph(1)->setData(frames, dx_corrected);
    ui->plot_dx->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_dx->yAxis->setRange(dx_start, dx_start + DX_WIDTH);
    ui->plot_dx->xAxis->setLabel("Frames");
    ui->plot_dx->yAxis->setLabel("Horizontal Displacement");

    ui->plot_dy->addGraph();
    ui->plot_dy->addGraph();
    ui->plot_dy->graph(0)->setPen(*red);
    ui->plot_dy->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_dy->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dy->graph(0)->setData(frames, dy);
    ui->plot_dy->graph(1)->setPen(*blue);
    ui->plot_dy->graph(1)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_dy->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dy->graph(1)->setData(frames, dy_corrected);
    ui->plot_dy->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_dy->yAxis->setRange(dy_start, dy_start + DY_WIDTH);
    ui->plot_dy->xAxis->setLabel("Frames");
    ui->plot_dy->yAxis->setLabel("Vertical Displacement");

    ui->plot_da->addGraph();
    ui->plot_da->addGraph();
    ui->plot_da->graph(0)->setPen(*red);
    ui->plot_da->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_da->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_da->graph(0)->setData(frames, da);
    ui->plot_da->graph(1)->setPen(*blue);
    ui->plot_da->graph(1)->setScatterStyle(QCPScatterStyle::ssDot);
    ui->plot_da->graph(1)->setLineStyle(QCPGraph::lsLine);
    ui->plot_da->graph(1)->setData(frames, da_corrected);
    ui->plot_da->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_da->yAxis->setRange(da_start, da_start + DA_WIDTH);
    ui->plot_da->xAxis->setLabel("Frames");
    ui->plot_da->yAxis->setLabel("Angular Displacement");

}

void MainWindow::updateGraphs(){

    dx.append(dx_val);
    dx_corrected.append(dx_corrected_val);
    dy.append(dy_val);
    dy_corrected.append(dy_corrected_val);
    da.append(da_val);
    da_corrected.append(da_corrected_val);
    frames.append(frames_val);

    ui->plot_dx->graph(0)->setData(frames, dx);
    ui->plot_dx->graph(1)->setData(frames, dx_corrected);
    ui->plot_dy->graph(0)->setData(frames, dy);
    ui->plot_dy->graph(1)->setData(frames, dy_corrected);
    ui->plot_da->graph(0)->setData(frames, da);
    ui->plot_da->graph(1)->setData(frames, da_corrected);

    if(frames_val > FRAMES_WIDTH*0.9)
        frames_start = frames_val - FRAMES_WIDTH * 0.9;

    if(dx_val > DX_START+DX_WIDTH*0.9 || dx_corrected_val > DX_START+DX_WIDTH*0.9)
        dx_start = std::max(dx_val, dx_corrected_val) - DX_WIDTH * 0.9;
    else if(dx_val < DX_START+DX_WIDTH*0.1 || dx_corrected_val < DX_START+DX_WIDTH*0.1)
        dx_start = std::min(dx_val, dx_corrected_val) - DX_WIDTH * 0.1;
    else
        dx_start = DX_START;

    if(dy_val > DY_START+DY_WIDTH*0.9 || dy_corrected_val > DY_START+DY_WIDTH*0.9)
        dy_start = std::max(dy_val, dy_corrected_val) - DY_WIDTH * 0.9;
    else if(dy_val < DY_START+DY_WIDTH*0.1 || dy_corrected_val < DY_START+DY_WIDTH*0.1)
        dy_start = std::min(dy_val, dy_corrected_val) - DY_WIDTH * 0.1;
    else
        dy_start = DY_START;

    if(da_val > DA_START+DA_WIDTH*0.9 || da_corrected_val > DA_START+DA_WIDTH*0.9)
        da_start = std::max(da_val, da_corrected_val) - DA_WIDTH * 0.9;
    else if(da_val < DA_START+DA_WIDTH*0.1 || da_corrected_val < DA_START+DA_WIDTH*0.1)
        da_start = std::min(da_val, da_corrected_val) - DA_WIDTH * 0.1;
    else
        da_start = DA_START;

    ui->plot_dx->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_dx->yAxis->setRange(dx_start, dx_start + DX_WIDTH);

    ui->plot_dy->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_dy->yAxis->setRange(dy_start, dy_start + DY_WIDTH);

    ui->plot_da->xAxis->setRange(frames_start, frames_start + FRAMES_WIDTH);
    ui->plot_da->yAxis->setRange(da_start, da_start + DA_WIDTH);

    ui->plot_dx->replot();
    ui->plot_dx->update();
    ui->plot_dy->replot();
    ui->plot_dy->update();
    ui->plot_da->replot();
    ui->plot_da->update();
}