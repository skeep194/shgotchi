#ifndef SHGOTCHI_H
#define SHGOTCHI_H

typedef struct Shgotchi
{
    char name[255];
    int level;
    int exp;
    int care_miss;
    int hungry;
    int max_hungry;
    int port;
    int hungry_timer;
    int isDie;
} Shgotchi;

int CreateShgotchi(const char* name);
const char* LevelToKorean(int level);
void GetShgotchiByPort(int port, Shgotchi* buf);
const char* GetShgotchiFace(Shgotchi* shgotchi);

#endif