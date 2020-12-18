#ifndef SHGOTCHI_LIST_H
#define SHGOTCHI_LIST_H

#include "shgotchi.h"

Shgotchi* CreateShgotchi(const char*);
void SetShgotchiFromSaveFile();
void AppendShgotchi(Shgotchi*);

#endif