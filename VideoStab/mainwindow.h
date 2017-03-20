#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "player.h"
#include <QMainWindow>
#include "stabilizer.h"

class stabilizer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    friend class stabilizer;
    explicit MainWindow(QWidget *parent = 0);
    QVector<double> x1,y1,y2,y3;
    double prev_x1,prev_y1,prev_y2,prev_y3;
    double x_start, x_width;
    ~MainWindow();
private slots:
    //Display video frame in player UI
    void updateOriginalPlayerUI(QImage img);
    void updateStabilizedPlayerUI(QImage img);
    void on_checkBox_clicked();

public slots:
    void updatePlots();

private:
    Ui::MainWindow *ui;
    Player* originalPlayer;
    Player* processedPlayer;
    stabilizer* stabStream;

};

#endif // MAINWINDOW_H
