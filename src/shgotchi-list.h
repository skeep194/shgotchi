#ifndef SHGOTCHI_LIST_H
#define SHGOTCHI_LIST_H

typedef struct Shgotchi
{
    char face[10];
    char name[255];
    int id;
    int level;
    int care_miss;
    int hungry;
    int max_hungry;
    int isDie;
    int port;
} Shgotchi;

Shgotchi* CreateShgotchi(const char*);
void SetShgotchiFromSaveFile();
void Append(Shgotchi*);

#endif