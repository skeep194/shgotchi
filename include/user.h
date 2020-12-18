#ifndef USER_H
#define USER_H

typedef struct User 
{
    int default_shgotchi;
    int food;
    int money;
} User;

void SetUserFromSaveFile();
void CreateUser();

#endif