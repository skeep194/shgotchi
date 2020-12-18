#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shgotchi.h"
#include "save.h"

const int kBasePort = 60194;
const char* kShgotchiSaveDirPath = "/.shgotchi/shgotchi/";

int CreateShgotchi(const char* name)
{
    //create shgotchi sturcture
    Shgotchi* ret = malloc(sizeof(Shgotchi));
    ret->care_miss = 0;
    ret->hungry = 5;
    strcpy(ret->face, "@");
    ret->max_hungry = 5;
    ret->level = 0;
    ret->isDie = 0;
    ret->port = kBasePort + ret->id;
    strcpy(ret->name, name);
    //shgotchi to savefile
    char path[256];
    sprintf(path, "%s%d", kShgotchiSaveDirPath, ret->port);
    creat(path, 0644);
    Save(path, ret, sizeof(Shgotchi));
    //return port number
    return ret->port;
}

//deprecated
Shgotchi* GetShgotchiByPort(int port)
{
    DIR* dir = opendir(kShgotchiSaveDirPath);
    struct dirent* buf;
    while((buf = readdir(dir)) != NULL)
    {
        if(atoi(buf->d_name) == port)
        {
            Shgotchi* here = malloc(sizeof(Shgotchi));
            char path[256];
            sprintf(path, "%s%s", kShgotchiSaveDirPath, buf->d_name);
            int fd = open(path, O_RDONLY);
            read(fd, here, sizeof(Shgotchi));
            close(fd);
            return here;
        }
    }
    return NULL;
}