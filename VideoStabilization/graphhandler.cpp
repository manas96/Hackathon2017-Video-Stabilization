#include "graphhandler.h"

GraphHandler::GraphHandler()
{
    stop_graphs = false;
    start();
}


void GraphHandler::run(){
    int sock = 0;
    struct sockaddr_in serv_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, SERVER_ADDRESS, &serv_addr.sin_addr);

    ::connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while(!stop_graphs){
        GraphData gd;
        int val = read(sock, &gd, sizeof(GraphData));
        if(val <= 0)
            stop_graphs = true;
        else
            emit(graphData(gd));
    }
    close(sock);
}
