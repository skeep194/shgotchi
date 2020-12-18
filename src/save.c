#include <fcntl.h>
#include <unistd.h>
#include "save.h"
#include "shgotchi-list.h"
#include "user.h"

extern Shgotchi** shgotchi_list;
extern int list_size;
extern User user;
extern const char* kUserSaveFilePath;
extern const char* kShgotchiSaveFilePath;

void Save()
{
    int shgotchi_fd = open(kShgotchiSaveFilePath, O_WRONLY);
    int user_fd = open(kUserSaveFilePath, O_WRONLY);
    for(int i=0;i<list_size;++i)
    {
        write(shgotchi_fd, shgotchi_list[i], sizeof(Shgotchi));
    }
    write(user_fd, &user, sizeof(User));
    close(shgotchi_fd);
}