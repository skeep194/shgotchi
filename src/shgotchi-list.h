#ifndef SHGOTCHI_LIST_H
#define SHGOTCHI_LIST_H

#include "src/shgotchi.h"

int kBasePort = 60194;

Shgotchi* CreateShgotchi(const char*);
void SetShgotchiFromSaveFile();
void AppendShgotchi(Shgotchi*);

#endif