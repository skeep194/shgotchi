#ifndef SHGOTCHI_H
#define SHGOTCHI_H

typedef struct Shgotchi
{
    char face[20];
    char name[255];
    int level;
    int care_miss;
    int hungry;
    int max_hungry;
    int port;
} Shgotchi;

int CreateShgotchi(const char* name);
void GetShgotchiByPort(int port, Shgotchi* buf);

#endif