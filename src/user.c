#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "user.h"

const char* kUserSaveFilePath = "/.shgotchi/user.savefile";
User user;

void SetUserFromSaveFile()
{
    int fd = open(kUserSaveFilePath, O_RDONLY);
    int rdsize = read(fd, &user, sizeof(user));
    #ifdef DEBUG
        printf("read user data: %d %d\n", user.default_shgotchi, user.money);
    #endif
    if(rdsize != sizeof(user))
    {
        fprintf(stderr, "save file crashed!! please run shgotchi init");
        exit(1);
    }
    close(fd);
}