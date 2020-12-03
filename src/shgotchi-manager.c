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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/shgotchi.h"

const int kCmdCount = 8;
const char *kCmdList[] = {"help", "init", "ls", "status", "echo", "ch", "feed", "shop"};
const int kArgcList[] = {3, 2, 2, 3, 2, 3, 2, 2};

//create save directory and game initilization
void Init()
{
}

void CreateShgotchi()
{
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
    int cmd;
    if (argc == 1 || (cmd = CmdToInt(argv[2])) == -1 || (argc == 2 && strcmp(argv[2], "help") == 0))
    {
        Help();
        return 0;
    }
    if (kArgcList[CmdToInt(argv[2])] != argc)
    {
        fprintf(stderr, "%s command expect number of argument %d but given %d", argv[2], kArgcList[CmdToInt(argv[2])] - 2, argc - 2);
        exit(1);
    }
    switch (CmdToInt(argv[2]))
    {
    case 0:
        Init();
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    default:
        Help();
    }
    return 0;
}