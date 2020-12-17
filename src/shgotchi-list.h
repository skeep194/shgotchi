#ifndef SHGOTCHI_LIST_H
#define SHGOTCHI_LIST_H

#include "src/shgotchi.h"

Shgotchi* CreateShgotchi(const char*);
void SetShgotchiFromSaveFile();
void Append(Shgotchi*);

#endif