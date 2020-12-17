#include <sys/types.h>
#include <unistd.h>
#include "src/shgotchi-process.h"

void CreateShgotchiProcess(Shgotchi* element)
{
    pid_t pid = fork();
    if(pid == 0)
    {
        //TODO: 자식 프로세스에서는 소켓 열고 대기상태 만들어야함
    }

}