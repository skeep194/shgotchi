/*
manager program for each tamagotchi
shgotchi, shgotchi help, shgotchi help -c -> print help message of shgotchi-manager (command)
shgotchi help -g -> print help message of shgotchi game (game description)
shgotchi init -> create folder of tamagotchi's savefile. if exist, ask user for data initialization
shgotchi ls -> print tamagotchi list
shgotchi status 1 -> print 1's status (tamagotchi parameter)
shgotchi echo -> print default tamagotchi's face (shell view)
shgotchi ch 1 -> change default tamagotchi to 1
shgotchi feed (tamagotchi) (item) -> feed item to tamagotchi
shgotchi shop -> UI for tamagotchi shop (use curses library)
shgotchi stop 1 -> stop tamagotchi 1 process
*/

#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "user.h"
#include "shgotchi.h"
#include "shgotchi-list.h"
#include "shgotchi-process.h"
#include "save.h"

const int kCmdCount = 8;
const char *kCmdList[] = {"help", "init", "ls", "status", "echo", "ch", "feed", "shop"};
const int kArgcList[] = {3, 2, 2, 3, 2, 3, 2, 2};
const char *kDirName = "/.shgotchi";

int sock_list[100];

extern User user;
extern const char *kShgotchiSaveDirPath;
extern const char *kUserSaveFilePath;
extern const int kBasePort;
extern int* shgotchi_list;
extern int list_size;

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
        //소켓 확인한 후 닫혀있으면 프로세스 생성, 열려있으면 소켓 연결
        for(int i=0;i<list_size;++i)
        {
            int port = shgotchi_list[i];
            int clnt_sock = socket(PF_INET, SOCK_STREAM, 0);
            if(clnt_sock == -1)
            {
                fprintf(stderr, "shgotchi-manager: socket error\n");
                exit(1);
            }
            struct sockaddr_in serv_addr;
            memset(&serv_addr, 0, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
            serv_addr.sin_port = htons(port);
            if(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
            {
                #ifdef DEBUG
                    printf("make process\n");
                #endif
                CreateShgotchiProcess(port);
                //문제: 자식 프로세스에서 소켓을 열기 때문에 부모 프로세스에서 연결을 바로 시도하면 커넥션 에러가 항상 발생함
                //해결: 5초동안 시도해보고 안되면 커넥션 에러.. 뭔가 깔끔한 방법이 없는걸까?
                int cnt = 0;
                while(connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
                {
                    if(cnt++ == 5000)
                    {
                        fprintf(stderr, "shgotchi-manager: socket connection error\n");
                        break;
                    }
                    usleep(1000);
                }
            }
            sock_list[i] = clnt_sock;
        }
    }
    else
    {
        printf("save directory isn't exist. please execute shgotchi init\n");
    }
}

//create save directory and game initilization
void Init()
{
    char buffer[BUFSIZ];
    if(access(kDirName, F_OK) == 0)
    {
        printf("already save directory existed. Did you want delete? (yes, no)\n");
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
        system("killall shgotchi");
        return;
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
}

void Ls()
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        write(sock_list[i], "info", sizeof("info"));
        read(sock_list[i], &buf, sizeof(Shgotchi));
        printf("%s\n", buf.name);
    }
}

void Status(char name[256])
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        write(sock_list[i], "info", sizeof("info"));
        read(sock_list[i], &buf, sizeof(Shgotchi));
        if(strcmp(buf.name, name) == 0)
        {
            printf("%s\n%s\n이름: %s\n배고픔: %d/%d\n", buf.face, LevelToKorean(buf.level), buf.name, buf.hungry, buf.max_hungry);
            return;
        }
    }
    printf("%s isn't exist\n");
}

void Echo()
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        if(shgotchi_list[i] == user.default_shgotchi)
        {
            write(sock_list[i], "info", sizeof("info"));
            read(sock_list[i], &buf, sizeof(Shgotchi));
            printf("%s\n", buf.face);
            return;
        }
    }
}

void Ch(char name[256])
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        write(sock_list[i], "info", sizeof("info"));
        read(sock_list[i], &buf, sizeof(Shgotchi));
        if(strcmp(buf.name, name) == 0)
        {
            printf("change default shgotchi to %s\n", name);
            user.default_shgotchi = buf.port;
            Save(kUserSaveFilePath, &user, sizeof(User));
            return;
        }
    }
    printf("%s isn't exist\n");
}

void Feed(char shgotchi_name[256], char item_name[256])
{

}

void Shop()
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
    if(CmdToInt(argv[1]) != init)
        GameInit();
    switch (CmdToInt(argv[1]))
    {
    case init:
        Init();
        break;
    case ls:
        Ls();
        break;
    case status:
        Status(argv[2]);
        break;
    case echo:
        Echo();
        break;
    case ch:
        Ch(argv[2]);
        break;
    case feed:
        Feed(argv[2], argv[3]);
        break;
    case shop:
        Shop();
        break;
    default:
        Help();
    }
    return 0;
}