#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void)
{
    int pipefd[2];
    pid_t pid;

    char message[] = "Hello from parent process!";
    char buffer[100];

    /*
     * 파이프 생성
     * pipefd[0] : 읽기용
     * pipefd[1] : 쓰기용
     */
    if (pipe(pipefd) == -1) {
        perror("pipe");
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

        // 자식은 읽기만 하므로 쓰기 쪽 닫기
        close(pipefd[1]);

        memset(buffer, 0, sizeof(buffer));

        /*
         * 부모가 보낸 데이터 읽기
         */
        if (read(pipefd[0], buffer, sizeof(buffer)) == -1) {
            perror("read");
            close(pipefd[0]);
            exit(1);
        }

        printf("[자식] 부모에게 받은 메시지: %s\n", buffer);

        // 읽기 쪽 닫기
        close(pipefd[0]);

        printf("[자식] IPC 통신 완료\n");
    }
    else {
        /*
         * 부모 프로세스
         */

        // 부모는 쓰기만 하므로 읽기 쪽 닫기
        close(pipefd[0]);

        printf("[부모] 자식 프로세스에게 메시지 전송\n");

        /*
         * 파이프로 데이터 전송
         */
        if (write(pipefd[1], message, strlen(message) + 1) == -1) {
            perror("write");
            close(pipefd[1]);
            exit(1);
        }

        // 쓰기 쪽 닫기
        close(pipefd[1]);

        /*
         * 자식 종료 대기
         */
        wait(NULL);

        printf("[부모] 자식 프로세스 종료 확인\n");
    }

    return 0;
}
