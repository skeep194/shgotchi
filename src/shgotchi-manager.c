/*
manager program for each tamagotchi
sm, sm help, sm help -c -> print help message of shgotchi-manager (command)
sm help -g -> print help message of shgotchi game (game description)
sm init -> create folder of tamagotchi's savefile. if exist, ask user for data initialization
sm ls -> print tamagotchi list
sm status 1 -> print 1's status (tamagotchi parameter)
sm echo -> print default tamagotchi's face (shell view)
sm ch 1 -> change default tamagotchi to 1
sm feed (tamagotchi) (item) -> feed item to tamagotchi
sm shop -> UI for tamagotchi shop (use curses library)
*/

#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "user.h"
#include "shgotchi.h"
#include "shgotchi-list.h"
#include "shgotchi-process.h"
#include "save.h"

const int kCmdCount = 8;
const char *kCmdList[] = {"help", "init", "ls", "status", "echo", "ch", "feed", "shop"};
const int kArgcList[] = {3, 2, 2, 3, 2, 3, 2, 2};
const char *kDirName = "/.shgotchi";

extern User user;
extern const char *kShgotchiSaveDirPath;
extern const char *kUserSaveFilePath;
extern const int kBasePort;

typedef enum command
{
    help = 0,
    init,
    ls,
    status,
    echo,
    ch,
    feed,
    shop
}command;

void GameInit()
{
    if(access(kDirName, F_OK) == 0)
    {
        SetUserFromSaveFile();
        SetShgotchiFromSaveFile();
        //TODO: 소켓 확인한 후 닫혀있으면 프로세스 생성, 열려있으면 소켓 연결
        extern int* shgotchi_list;
        extern int list_size;
        for(int i=0;i<list_size;++i)
        {
            int port = shgotchi_list[i];

        }
    }
}

//create save directory and game initilization
void Init()
{
    char buffer[BUFSIZ];
    if(access(kDirName, F_OK) == 0)
    {
        printf("already save directory existed. Did you want new savefile? (yes, no)\n");
        int flag = 0;
        do
        {
            if(flag)
                printf("wrong value %s, yes or no expected.\n", buffer);
            flag = 1;
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer)-1] = 0;
        } while (strcmp(buffer, "yes") != 0 && strcmp(buffer, "no") != 0);
        if(strcmp(buffer, "no") == 0)
            return;
        assert(strcmp(buffer, "yes") == 0);
        system("rm -rf /.shgotchi");
    }
    int status;
    if((status = mkdir(kDirName, 0755)) == -1)
        perror("shgotchi ");
    if((status = creat(kUserSaveFilePath, 0644)) == -1)
        perror("shgotchi ");
    if((status = mkdir(kShgotchiSaveDirPath, 0755)) == -1)
        perror("shgotchi ");
    printf("you got a new shgotchi egg!\nenter name for your first shgotchi\n");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer)-1] = 0;
    AppendShgotchiPort(CreateShgotchi(buffer));
    user.default_shgotchi = kBasePort;
    user.money = 1000;
    Save(kUserSaveFilePath, &user, sizeof(User));
    GameInit();
}

//convert command string to integer
int CmdToInt(const char *cmd)
{
    for (int i = 0; i < kCmdCount; ++i)
    {
        if (strcmp(cmd, kCmdList[i]) == 0)
        {
            return i;
        }
    }
    //if not exist, return -1
    return -1;
}

//print help message of shgotchi-manager
void Help()
{
    printf("manager program for each tamagotchi\n");
}

int main(int argc, char *argv[])
{
    GameInit();
    int cmd;
    if (argc == 1 || (cmd = CmdToInt(argv[1])) == -1 || (argc == 2 && strcmp(argv[1], "help") == 0))
    {
        Help();
        return 0;
    }
    if (kArgcList[CmdToInt(argv[1])] != argc)
    {
        fprintf(stderr, "%s command expect number of argument %d but given %d\n", argv[1], kArgcList[CmdToInt(argv[1])] - 2, argc - 2);
        exit(1);
    }
    switch (CmdToInt(argv[1]))
    {
    case init:
        Init();
        break;
    case ls:
        break;
    case status:
        break;
    case echo:
        break;
    case ch:
        break;
    case feed:
        break;
    case shop:
        break;
    default:
        Help();
    }
    return 0;
}