#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "save.h"
#include "shgotchi-list.h"
#include "shgotchi.h"
#include "user.h"

extern int* shgotchi_list;
extern int list_size;
extern User user;
extern const char* kUserSaveFilePath;
extern const char* kShgotchiSaveDirPath;

void Save(const char* path, void* buffer, size_t bufsize)
{
    int fd = open(path, O_WRONLY);
    if(fd == -1)
        perror("save error");
    write(fd, buffer, bufsize);
    close(fd);
}