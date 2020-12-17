#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "src/user.h"

const char* kUserSaveFilePath = "/.shgotchi/user.savefile";
User user;

void SetUserFromSaveFile()
{
    int fd = open(kUserSaveFilePath, O_RDWR);
    int rdsize = read(fd, &user, sizeof(user));
    if(rdsize != sizeof(user))
    {
        fprintf(stderr, "save file crashed!! please run shgotchi init");
        exit(1);
    }
}