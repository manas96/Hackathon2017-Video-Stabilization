#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "player.h"
#include <QMainWindow>
#include "stabilizer.h"
#include "displaystabilized.h"
class stabilizer;
class DisplayStabilized;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class stabilizer;
    explicit MainWindow(QWidget *parent = 0);
    QVector<double> x1,y1,y2;
    //QVector<double>y3;
    QVector<double> y1Corrected,y2Corrected;
    //QVector<double> y3Corrected;
    double prev_x1,prev_y1,prev_y2;
    //double prev_y3;
    double prev_y1Corrected,prev_y2Corrected;
   // double prev_y3Corrected;
    double x_start, x_width, y1_start, y1_width, y2_start, y2_width;
    ~MainWindow();
private slots:
    //Display video frame in player UI
    void updateOriginalPlayerUI(QImage img);
    void updateStabilizedPlayerUI(QImage img);
    void renderImage(QImage img);
    void on_checkBox_clicked();

    void on_checkBox_2_clicked();

public slots:
    void updatePlots();

private:
    Ui::MainWindow *ui;
    Player* originalPlayer;
    Player* processedPlayer;
    stabilizer* stabStream;
    QPen* blue;
    QPen* red;
    DisplayStabilized* displayStream;

};

#endif // MAINWINDOW_H
