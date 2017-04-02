#ifndef GRAPHHANDLER_H
#define GRAPHHANDLER_H

#include <QThread>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#define PORT 1234
#define SERVER_ADDRESS "192.168.1.34"

struct GraphData{
    double dx, dy, da, dx_corrected, dy_corrected, da_corrected;
    GraphData(double d2, double d3, double d4, double d5, double d6, double d7){
        dx = d2;
        dy = d3;
        da = d4;
        dx_corrected = d5;
        dy_corrected = d6;
        da_corrected = d7;
    }
    GraphData(){}
};

class GraphHandler:public QThread
{
    Q_OBJECT
signals:
    void graphData(const GraphData &gd);
private:
    void run();
    bool stop_graphs;
public:

    GraphHandler();
};

#endif // GRAPHHANDLER_H
