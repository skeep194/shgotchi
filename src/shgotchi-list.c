//각 shgotchi의 포트 번호 저장
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include "shgotchi-list.h"
#include "shgotchi.h"

extern const int kBasePort;
extern const char* kShgotchiSaveDirPath;
int* shgotchi_list = NULL;
int list_size;
int max_size;

void AppendShgotchiPort(int element)
{
    if(shgotchi_list == NULL)
    {
        shgotchi_list = malloc(sizeof(int)*2);
        max_size = 2;
    }
    if(list_size == max_size)
    {
        shgotchi_list = realloc(shgotchi_list, sizeof(int) * max_size*2);
        max_size *= 2;
    }
    shgotchi_list[list_size++] = element;
}

void SetShgotchiFromSaveFile()
{
    DIR* dir = opendir(kShgotchiSaveDirPath);
    struct dirent* buf;
    while((buf = readdir(dir)) != NULL)
    {
        AppendShgotchiPort(atoi(buf->d_name));
    }
}