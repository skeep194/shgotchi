/*
manager program for each tamagotchi
shgotchi, shgotchi help, shgotchi help -c -> print help message of shgotchi-manager (command)
shgotchi help -g -> print help message of shgotchi game (game description)
shgotchi init -> create folder of tamagotchi's savefile. if exist, ask user for data initialization
shgotchi ls -> print tamagotchi list
shgotchi status 1 -> print 1's status (tamagotchi parameter)
shgotchi echo -> print default tamagotchi's face (shell view)
shgotchi ch 1 -> change default tamagotchi to 1
shgotchi feed (tamagotchi) -> feed tamagotchi
shgotchi shop -> UI for tamagotchi shop (use curses library)
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
#include <time.h>
#include "user.h"
#include "shgotchi.h"
#include "shgotchi-list.h"
#include "shgotchi-process.h"
#include "save.h"

#define MIN(x,y) ( (x)<(y)?(x):(y) )

const int kCmdCount = 8;
const char *kCmdList[] = {"help", "init", "ls", "status", "echo", "ch", "feed", "shop"};
const int kArgcList[] = {3, 2, 2, 3, 2, 3, 3, 2};
const char *kDirName = "/.shgotchi";

int sock_list[100];
int isWrite[100];
int socket_open[100];

extern User user;
extern const char *kShgotchiSaveDirPath;
extern const char *kUserSaveFilePath;
extern const int kMaxExp[4];
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
            socket_open[i] = 1;
        }
    }
    else
    {
        printf("세이브 디렉토리가 존재하지 않습니다. shgotchi init을 실행 해주세요.\n");
    }
}

//create save directory and game initilization
void Init()
{
    char buffer[BUFSIZ];
    if(access(kDirName, F_OK) == 0)
    {
        printf("세이브 디렉토리가 이미 존재합니다. 디렉토리를 지우겠습니까? (yes, no)\n");
        int flag = 0;
        do
        {
            if(flag)
                printf("yes 혹은 no만 입력할 수 있습니다.\n", buffer);
            flag = 1;
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strlen(buffer)-1] = 0;
        } while (strcmp(buffer, "yes") != 0 && strcmp(buffer, "no") != 0);
        if(strcmp(buffer, "no") == 0)
            return;
        assert(strcmp(buffer, "yes") == 0);
        system("rm -rf /.shgotchi");
        system("killall shgotchi");
        exit(0);
    }
    int status;
    if((status = mkdir(kDirName, 0755)) == -1)
        perror("shgotchi ");
    if((status = creat(kUserSaveFilePath, 0666)) == -1)
        perror("shgotchi ");
    if((status = mkdir(kShgotchiSaveDirPath, 0755)) == -1)
        perror("shgotchi ");
    printf("첫 다마고치의 이름은?\n");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer)-1] = 0;
    AppendShgotchiPort(CreateShgotchi(buffer));
    user.default_shgotchi = kBasePort;
    user.money = 1000;
    user.food = 0;
    Save(kUserSaveFilePath, &user, sizeof(User));
}

void Ls()
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        write(sock_list[i], "info", sizeof("info"));
        read(sock_list[i], &buf, sizeof(Shgotchi));
        isWrite[i] = 1;
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
        isWrite[i] = 1;
        if(strcmp(buf.name, name) == 0)
        {
            if(buf.isDie)
                printf("%s는 죽었다...\n", buf.name);
            else
                printf("%s\n%s\n이름: %s\n배고픔: %d/%d\n경험치: %d/%d\n", GetShgotchiFace(&buf), LevelToKorean(buf.level), buf.name, buf.hungry, buf.max_hungry, buf.exp, kMaxExp[buf.level]);
            return;
        }
    }
    printf("%s는 존재하지 않는다.\n");
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
            isWrite[i] = 1;
            printf("%s\n", GetShgotchiFace(&buf));
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
        isWrite[i] = 1;
        if(strcmp(buf.name, name) == 0)
        {
            printf("기본 다마고치가 %s로 변경되었다.\n", name);
            user.default_shgotchi = buf.port;
            Save(kUserSaveFilePath, &user, sizeof(User));
            return;
        }
    }
    printf("%s는 존재하지 않는다.\n");
}

void Feed(char shgotchi_name[256])
{
    if(user.food > 0)
    {
        Shgotchi buf;
        for(int i=0;i<list_size;++i)
        {
            write(sock_list[i], "info", sizeof("info"));
            read(sock_list[i], &buf, sizeof(Shgotchi));
            isWrite[i] = 1;
            if(strcmp(buf.name, shgotchi_name) == 0)
            {
                if(buf.isDie)
                    printf("%s는 죽어서 먹이를 줄 수 없다...\n", buf.name);
                else
                {
                    printf("%s에게 먹이를 주었다.\n", buf.name);
                    user.food -= 1;
                    buf.hungry = MIN(buf.max_hungry, buf.hungry + 3);
                    Save(kUserSaveFilePath, &user, sizeof(User));
                    char path[256];
                    sprintf(path, "%s%d", kShgotchiSaveDirPath, buf.port);
                    Save(path, &buf, sizeof(Shgotchi));
                }
                return;
            }
        }
        printf("%s는 존재하지 않는다.\n", buf.name);
    }
    else
    {
        printf("먹이가 없습니다. 상점에서 먹이를 구입하세요.\n");
    }
}

void Shop()
{
    printf("다마고치 상점! 당신은 %d원을 가지고 있다.\n", user.money);
    printf("구입할 항목의 숫자를 입력해 주세요.\n");
    printf("1. 먹이 (100)\n");
    printf("2. 다마고치 알 (500)\n");
    int num;
    scanf("%d", &num);
    switch(num)
    {
        case 1:
            if(user.money >= 100)
            {
                user.money -= 100;
                user.food += 1;
                printf("먹이를 성공적으로 구입했습니다!\n");
            }
            else
            {
                printf("먹이를 구입하기에는 돈이 부족합니다.\n");
            }
            break;
        case 2:
            if(user.money >= 500)
            {
                user.money -= 500;
                printf("새 다마고치의 이름은?\n");
                char buffer[BUFSIZ];
                //개행문자가 버퍼에 남아있는데 더 깔끔하게 처리 못하겠다..
                scanf(" %[^\n]", buffer);
                AppendShgotchiPort(CreateShgotchi(buffer));
            }
            break;
        default:
            printf("잘못된 숫자입니다.\n");
    }
    Save(kUserSaveFilePath, &user, sizeof(User));
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

void reset()
{
    Shgotchi buf;
    for(int i=0;i<list_size;++i)
    {
        if(socket_open[i] && !isWrite[i])
        {
            write(sock_list[i], "info", sizeof("info"));
            read(sock_list[i], &buf, sizeof(Shgotchi));
        }
    }
}

//print help message of shgotchi-manager
void Help()
{
    printf("manager program for each tamagotchi\n");
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
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
        Feed(argv[2]);
        break;
    case shop:
        Shop();
        break;
    default:
        Help();
    }
    if(rand()%10 == 0)
    {
        int num = rand()%300;
        printf("%d원을 발견했다.\n", num);
        user.money += num;
        Save(kUserSaveFilePath, &user, sizeof(User));
    }
    reset();
    return 0;
}