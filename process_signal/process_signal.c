#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void signal_handler(int signo)
{
    if (signo == SIGUSR1) {
        printf("[부모] SIGUSR1 시그널을 받았습니다.\n");
    }
}

int main(void)
{
    pid_t pid;
    struct sigaction act;

    printf("=== 프로세스 및 시그널 실습 ===\n");

    /*
     * SIGUSR1 시그널 처리 함수 등록
     */
    act.sa_handler = signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (sigaction(SIGUSR1, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    /*
     * 자식 프로세스 생성
     */
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /*
         * 자식 프로세스
         */
        printf("[자식] PID  = %d\n", getpid());
        printf("[자식] PPID = %d\n", getppid());

        sleep(2);

        printf("[자식] 부모 프로세스에게 SIGUSR1 전송\n");

        kill(getppid(), SIGUSR1);

        printf("[자식] 실행 종료\n");

        exit(0);
    }
    else {
        /*
         * 부모 프로세스
         */
        printf("[부모] PID = %d\n", getpid());
        printf("[부모] 생성한 자식 PID = %d\n", pid);

        /*
         * 자식 프로세스 종료 대기
         */
        wait(NULL);

        printf("[부모] 자식 프로세스 종료 확인\n");
    }

    return 0;
}
