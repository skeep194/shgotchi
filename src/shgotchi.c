#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shgotchi.h"
#include "shgotchi-process.h"
#include "save.h"

const int kBasePort = 60194;
const char* kShgotchiSaveDirPath = "/.shgotchi/shgotchi/";
extern int list_size;
const int kMaxExp[4] = {30, 100, 200, -1};

int CreateShgotchi(const char* name)
{
    //create shgotchi sturcture
    Shgotchi* ret = malloc(sizeof(Shgotchi));
    ret->care_miss = 0;
    ret->hungry = 5;
    ret->max_hungry = 5;
    ret->level = 0;
    ret->port = kBasePort + list_size;
    #ifdef DEBUG
        printf("new port: %d\n", ret->port);
    #endif
    ret->isDie = 0;
    ret->exp = 0;
    ret->hungry_timer = 10;
    strcpy(ret->name, name);
    //shgotchi to savefile
    char path[256];
    sprintf(path, "%s%d", kShgotchiSaveDirPath, ret->port);
    creat(path, 0666);
    Save(path, ret, sizeof(Shgotchi));
    CreateShgotchiProcess(ret->port);
    //return port number
    return ret->port;
}

const char* GetShgotchiFace(Shgotchi* shgotchi)
{
    if(shgotchi->isDie)
    {
        return "x∧x";
    }
    const char* face[4][2] = {{"@", "@"}, {"˙ᵕ˙", "(-᷅_-᷄)"}, {"(ﾟ∀ﾟ　)", "o(-`д´- ｡)"}, {"(*≧∀≦*)", "( ≧Д≦)"}};
    return face[shgotchi->level][shgotchi->hungry == 0 ? 1 : 0];
}

const char* LevelToKorean(int level)
{
    const char* convert[4] = {"알", "유아기", "성장기", "성체"};
    return convert[level];
}

void GetShgotchiByPort(int port, Shgotchi* buf)
{
    char path[256];
    sprintf(path, "%s%d", kShgotchiSaveDirPath, port);
    int fd = open(path, O_RDONLY);
    if(fd == -1)
    {
        perror("shgotchi ");
    }
    read(fd, buf, sizeof(Shgotchi));
    close(fd);
}