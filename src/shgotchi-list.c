#include <stdlib.h>
#include <string.h>
#include "src/shgotchi-list.h"

const int kBasePort = 60194;
Shgotchi** shgotchi_list = NULL;
int list_size;
int max_size;

Shgotchi* CreateShgotchi(const char* name)
{
    Shgotchi* ret = malloc(sizeof(Shgotchi));
    ret->care_miss = 0;
    ret->hungry = 5;
    ret->id = list_size;
    strcpy(ret->face, "@");
    ret->max_hungry = 5;
    ret->level = 0;
    ret->isDie = 0;
    ret->port = kBasePort + ret->id;
    strcpy(ret->name, name);
    return ret;
}

void Append(Shgotchi* element)
{
    if(shgotchi_list == NULL)
    {
        shgotchi_list = malloc(sizeof(Shgotchi*)*2);
        max_size = 2;
    }
    if(list_size == max_size)
    {
        shgotchi_list = realloc(shgotchi_list, sizeof(Shgotchi*) * max_size*2);
        max_size *= 2;
    }
    shgotchi_list[list_size++] = element;
}