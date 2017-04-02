#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<GraphData>("GraphData");

    initGraphs();

    pos_original = 0;
    dir_original = 1;
    pause_original = false;
    timer_original = new QTimer(this);
    connect(timer_original, SIGNAL(timeout()), this, SLOT(updateOriginal()));

    pos_stabilized = 0;
    dir_stabilized = 1;
    pause_stabilized = false;
    timer_stabilized = new QTimer(this);
    connect(timer_stabilized, SIGNAL(timeout()), this, SLOT(updateStabilized()));

    gd = new GraphHandler();
    connect(gd, SIGNAL(graphData(GraphData)), this, SLOT(updateGraphData(GraphData)));

    original_stream = new Player(12346, "./temp1.jpg");
    stabilized_stream = new Player(12345, "./temp2.jpg");

    connect(original_stream, SIGNAL(originalImage(QImage)),
                this, SLOT(updateOriginalStreamUI(QImage)));
    connect(stabilized_stream, SIGNAL(originalImage(QImage)),
                this, SLOT(updateStabilizedStreamUI(QImage)));

    original_stream->loadVideo();
    stabilized_stream->loadVideo();

    FPS_ORIGINAL = 30;//original_stream->getFPS();
    fps_original = FPS_ORIGINAL;

    FPS_STABILIZED = 30;//stabilized_stream->getFPS();
    fps_stabilized = FPS_STABILIZED;

    timer_original->start(1000/fps_original);
    timer_stabilized->start(1000/fps_stabilized);

    original_stream->playVideo();
    stabilized_stream->playVideo();

    ui->label_original->setGeometry(RECT_ORIGINAL_1);
    ui->label_stabilized->setGeometry(RECT_STABILIZED_1);
    setButtonPositions();

    ui->graphButton->setGeometry(this->width()/2-ui->graphButton->width()/2, 610, ui->graphButton->width(),
                                 ui->graphButton->height());
}

MainWindow::~MainWindow()
{
    delete original_stream;
    delete stabilized_stream;
    delete red;
    delete blue;
    delete background;
    delete ui;
}

void MainWindow::updateOriginalStreamUI(QImage img){
    if(!img.isNull())
        buffer_original.push_back(QPixmap::fromImage(img).scaled(RECT_ORIGINAL_2.size()));
}

void MainWindow::updateStabilizedStreamUI(QImage img){
    if(!img.isNull())
        buffer_stabilized.push_back(QPixmap::fromImage(img).scaled(RECT_STABILIZED_2.size()));
}

void MainWindow::updateGraphData(GraphData gd){
    dx_val += gd.dx;
    dy_val += gd.dy;
    da_val += gd.da;
    dx_corrected_val = gd.dx_corrected;
    dy_corrected_val = gd.dy_corrected;
    da_corrected_val = gd.da_corrected;
    frames_val += 1;
    updateGraphs();
}

void MainWindow::updateOriginal(){
    if((int)buffer_original.size() > FPS_ORIGINAL * BUFFER_LIMIT)
        buffer_original.pop_front();
    if(buffer_original.size()>0){
        if(pos_original > (int)buffer_original.size()-1)
            pos_original = buffer_original.size()-1;
        else if(pos_original < 0)
            pos_original = 0;

        ui->label_original->setPixmap(buffer_original.at(pos_original).scaled(ui->label_original->size()));
        if(!pause_original)
            pos_original+=dir_original;
    }
}

void MainWindow::updateStabilized(){
    if((int)buffer_stabilized.size() > FPS_ORIGINAL * BUFFER_LIMIT)
        buffer_stabilized.pop_front();
    if(buffer_stabilized.size()>0){
        if(pos_stabilized > (int)buffer_stabilized.size()-1)
            pos_stabilized = buffer_stabilized.size()-1;
        else if(pos_stabilized < 0)
            pos_stabilized = 0;
        ui->label_stabilized->setPixmap(buffer_stabilized.at(pos_stabilized).scaled(ui->label_stabilized->size()));
        if(!pause_stabilized)
            pos_stabilized+=dir_stabilized;
    }
}

void MainWindow::initGraphs(){

    red = new QPen(QColor(255,1,100));
    blue = new QPen(QColor(0,191,255));
    background = new QBrush(QColor(49,54,59));

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

    ui->plot_dx->setBackground(*background);
    ui->plot_dx->axisRect()->setBackground(*background);
    ui->plot_dy->setBackground(*background);
    ui->plot_dy->axisRect()->setBackground(*background);
    ui->plot_da->setBackground(*background);
    ui->plot_da->axisRect()->setBackground(*background);

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

void MainWindow::on_pauseButton_original_clicked()
{
    if(ui->pauseButton_original->text() == "▮▮"){
        pause_original = true;
        ui->pauseButton_original->setText("▶");
    }
    else{
        pause_original = false;
        dir_original = 1;
        fps_original = FPS_ORIGINAL;
        timer_original->setInterval(1000/fps_original);
        ui->pauseButton_original->setText("▮▮");
    }
    ui->forwardButton_original->setText("▸▸");
    ui->rewindButton_original->setText("◂◂");
}
void MainWindow::on_forwardButton_original_clicked()
{
    pause_original = false;
    if(dir_original == -1)
        fps_original = FPS_ORIGINAL;
    dir_original = 1;
    fps_original = std::min(fps_original+FPS_ORIGINAL, FPS_ORIGINAL*3);
    timer_original->setInterval(1000/fps_original);
    if(fps_original == FPS_ORIGINAL * 2)
        ui->forwardButton_original->setText("▸▸▸");
    else
        ui->forwardButton_original->setText("▸▸▸▸");
    ui->rewindButton_original->setText("◂◂");
    ui->pauseButton_original->setText("▶");
}

void MainWindow::on_rewindButton_original_clicked()
{
    pause_original = false;
    if(dir_original == 1)
        fps_original = FPS_ORIGINAL;
    dir_original = -1;
    fps_original = std::min(fps_original+FPS_ORIGINAL, FPS_ORIGINAL*3);
    timer_original->setInterval(1000/fps_original);
    if(fps_original == FPS_ORIGINAL * 2)
        ui->rewindButton_original->setText("◂◂◂");
    else
        ui->rewindButton_original->setText("◂◂◂◂");
    ui->forwardButton_original->setText("▸▸");
    ui->pauseButton_original->setText("▶");
}

void MainWindow::on_jumpforwardButton_original_clicked()
{
    pos_original = buffer_original.size()-1;
}

void MainWindow::on_jumpbackButton_original_clicked()
{
    pos_original = 0;
}




void MainWindow::on_pauseButton_stabilized_clicked()
{
    if(ui->pauseButton_stabilized->text() == "▮▮"){
        pause_stabilized = true;
        ui->pauseButton_stabilized->setText("▶");
    }
    else{
        pause_stabilized = false;
        dir_stabilized = 1;
        fps_stabilized = FPS_STABILIZED;
        timer_stabilized->setInterval(1000/fps_stabilized);
        ui->pauseButton_stabilized->setText("▮▮");
    }
    ui->forwardButton_stabilized->setText("▸▸");
    ui->rewindButton_stabilized->setText("◂◂");
}
void MainWindow::on_forwardButton_stabilized_clicked()
{
    pause_stabilized = false;
    if(dir_stabilized == -1)
        fps_stabilized = FPS_STABILIZED;
    dir_stabilized = 1;
    fps_stabilized = std::min(fps_stabilized+FPS_STABILIZED, FPS_STABILIZED*3);
    timer_stabilized->setInterval(1000/fps_stabilized);
    if(fps_stabilized == FPS_STABILIZED * 2)
        ui->forwardButton_stabilized->setText("▸▸▸");
    else
        ui->forwardButton_stabilized->setText("▸▸▸▸");
    ui->rewindButton_stabilized->setText("◂◂");
    ui->pauseButton_stabilized->setText("▶");
}

void MainWindow::on_rewindButton_stabilized_clicked()
{
    pause_stabilized = false;
    if(dir_stabilized == 1)
        fps_stabilized = FPS_STABILIZED;
    dir_stabilized = -1;
    fps_stabilized = std::min(fps_stabilized+FPS_STABILIZED, FPS_STABILIZED*3);
    timer_stabilized->setInterval(1000/fps_stabilized);
    if(fps_stabilized == FPS_STABILIZED * 2)
        ui->rewindButton_stabilized->setText("◂◂◂");
    else
        ui->rewindButton_stabilized->setText("◂◂◂◂");
    ui->forwardButton_stabilized->setText("▸▸");
    ui->pauseButton_stabilized->setText("▶");

}

void MainWindow::on_jumpforwardButton_stabilized_clicked()
{
    pos_stabilized = buffer_stabilized.size()-1;
}

void MainWindow::on_jumpbackButton_stabilized_clicked()
{
    pos_stabilized = 0;
}



void MainWindow::toggleGraphs(){
    ui->plot_dx->setVisible(!ui->plot_dx->isVisible());
    ui->plot_dy->setVisible(!ui->plot_dy->isVisible());
    ui->plot_da->setVisible(!ui->plot_da->isVisible());
}

void MainWindow::setButtonPositions(){
    int X1_START = ui->label_original->x() + ui->label_original->width()/2 - BUTTON_WIDTH/2;
    int Y1_START = ui->label_original->y() + ui->label_original->height() + 5;
    int X2_START = ui->label_stabilized->x() + ui->label_stabilized->width()/2 - BUTTON_WIDTH/2;
    int Y2_START = ui->label_stabilized->y() + ui->label_stabilized->height() + 5;
    ui->pauseButton_original->setGeometry(X1_START, Y1_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->forwardButton_original->setGeometry(X1_START+BUTTON_WIDTH, Y1_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->rewindButton_original->setGeometry(X1_START-BUTTON_WIDTH, Y1_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->jumpforwardButton_original->setGeometry(X1_START+BUTTON_WIDTH*2, Y1_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->jumpbackButton_original->setGeometry(X1_START-BUTTON_WIDTH*2, Y1_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->pauseButton_stabilized->setGeometry(X2_START, Y2_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->forwardButton_stabilized->setGeometry(X2_START+BUTTON_WIDTH, Y2_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->rewindButton_stabilized->setGeometry(X2_START-BUTTON_WIDTH, Y2_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->jumpforwardButton_stabilized->setGeometry(X2_START+BUTTON_WIDTH*2, Y2_START, BUTTON_WIDTH, BUTTON_HEIGHT);
    ui->jumpbackButton_stabilized->setGeometry(X2_START-BUTTON_WIDTH*2, Y2_START, BUTTON_WIDTH, BUTTON_HEIGHT);
}

void MainWindow::on_graphButton_clicked()
{
    if(ui->graphButton->text() == "Hide Graphs"){
        toggleGraphs();
        ui->label_original->setGeometry(RECT_ORIGINAL_2);
        ui->label_stabilized->setGeometry(RECT_STABILIZED_2);
        ui->graphButton->setText("Show Graphs");
    }
    else{
        toggleGraphs();
        ui->label_original->setGeometry(RECT_ORIGINAL_1);
        ui->label_stabilized->setGeometry(RECT_STABILIZED_1);
        ui->graphButton->setText("Hide Graphs");
    }
    setButtonPositions();
}
