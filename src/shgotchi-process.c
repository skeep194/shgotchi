#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "save.h"
#include "shgotchi-process.h"

extern const char* kShgotchiSaveDirPath;
extern const int kMaxExp[4];
Shgotchi shgotchi;
int serv_sock;
int port_num;

void ShgotchiPause(int signum)
{
    char path[256];
    sprintf(path, "%s%d", kShgotchiSaveDirPath, shgotchi.port);
    Save(path, &shgotchi, sizeof(Shgotchi));
    close(serv_sock);
    exit(0);
}

void SetHungry(int signum)
{
    #ifdef DEBUG
        printf("alarm call\n");
    #endif
    GetShgotchiByPort(port_num, &shgotchi);
    #ifdef DEBUG
    printf("%d %d %d %d\n", shgotchi.hungry_timer, shgotchi.exp, shgotchi.level, shgotchi.hungry);
    #endif
    //TODO: 다시 살피기
    if(shgotchi.isDie)
        exit(0);
    if(--shgotchi.hungry_timer <= 0)
    {
        if(shgotchi.hungry > 0)
        {
            shgotchi.hungry -= 1;
        }
        else
        {
            shgotchi.care_miss += 1;
            if(shgotchi.care_miss >= 3)
            {
                printf("\n오랫동안 먹이를 주지 않아 %s는 아사했다...\n", shgotchi.name);
                shgotchi.isDie = 1;
            }
            else if(shgotchi.care_miss == 2)
            {
                printf("\n%s: 이러다가 굶어 죽겠어요!!!\n", shgotchi.name);
            }
            else if(shgotchi.care_miss == 1)
            {
                printf("\n%s: 배고파요..ㅠㅠ\n", shgotchi.name);
            }
        }
        shgotchi.hungry_timer = 10;
    }
    if(++shgotchi.exp == kMaxExp[shgotchi.level])
    {
        printf("\n%s는 %s에서 %s로 성장했다!\n", shgotchi.name, LevelToKorean(shgotchi.level), LevelToKorean(shgotchi.level+1));
        shgotchi.level += 1;
        shgotchi.max_hungry *= 2;
    }
    #ifdef DEBUG
    printf("%d %d %d %d\n", shgotchi.hungry_timer, shgotchi.exp, shgotchi.level, shgotchi.hungry);
    #endif
    char path[256];
    sprintf(path, "%s%d", kShgotchiSaveDirPath, port_num);
    Save(path, &shgotchi, sizeof(Shgotchi));
    alarm(1);
}

void CreateShgotchiProcess(int port)
{
    pid_t pid = fork();
    if(pid == 0)
    {
        port_num = port;
        //SIGINT, SIGTERM 시그널을 받을 경우 세이브하고 종료
        signal(SIGINT, ShgotchiPause);
        signal(SIGTERM, ShgotchiPause);
        //1초마다 배고픔 게이지 체크
        alarm(1);
        signal(SIGALRM, SetHungry);
        //소켓 통신을 위한 서버 프로그램
        //각각의 shgotchi가 하나의 소켓 서버
        serv_sock = socket(PF_INET, SOCK_STREAM, 0);
        if(serv_sock == -1)
        {
            fprintf(stderr, "socket error\n");
            exit(1);
        }
        int opt = 1;
        setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(port);
        if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
        {
            fprintf(stderr, "bind error\n");
            exit(1);
        }
        if(listen(serv_sock, 10) == -1)
        {
            fprintf(stderr, "listen error\n");
            exit(1);
        }
        
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        while(1)
        {
            GetShgotchiByPort(port, &shgotchi);
            int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if(clnt_sock == -1)
            {
                fprintf(stderr, "accept error\n");
                exit(1);
            }
            char readbuf[BUFSIZ];
            int readsiz = read(clnt_sock, readbuf, sizeof(readbuf));
            if(readsiz == -1)
            {
                fprintf(stderr, "socket read error\n");
                exit(1);
            }
            #ifdef DEBUG
                printf("%s\n", readbuf);
            #endif
            //assert read data is string
            readbuf[BUFSIZ-1] = 0;
            if(strcmp(readbuf, "info") == 0)
            {
                write(clnt_sock, &shgotchi, sizeof(Shgotchi));
            }
            close(clnt_sock);
            char path[256];
            sprintf(path, "%s%d", kShgotchiSaveDirPath, port);
        }
        close(serv_sock);
    }
}