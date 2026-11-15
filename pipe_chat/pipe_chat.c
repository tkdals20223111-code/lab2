#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

int main(void)
{
    int parent_to_child[2];
    int child_to_parent[2];

    pid_t pid;

    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];

    /*
     * 파이프 2개 생성
     * parent_to_child : 부모 -> 자식
     * child_to_parent : 자식 -> 부모
     */
    if (pipe(parent_to_child) == -1) {
        perror("pipe");
        exit(1);
    }

    if (pipe(child_to_parent) == -1) {
        perror("pipe");
        exit(1);
    }

    /*
     * fork 전에 한 번만 출력
     */
    printf("=== 파이프 기반 텍스트 채팅 프로그램 ===\n");
    printf("종료하려면 exit를 입력하세요.\n\n");
    fflush(stdout);

    /*
     * 자식 프로세스 생성
     */
    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    /*
     * 자식 프로세스
     */
    if (pid == 0) {

        // 사용하지 않는 파이프 닫기
        close(parent_to_child[1]);
        close(child_to_parent[0]);

        while (1) {

            memset(recv_buffer, 0, sizeof(recv_buffer));

            /*
             * 부모 메시지 수신
             */
            ssize_t n = read(
                parent_to_child[0],
                recv_buffer,
                sizeof(recv_buffer) - 1
            );

            if (n <= 0) {
                break;
            }

            printf("[부모] %s", recv_buffer);

            /*
             * 부모가 exit 입력
             */
            if (strncmp(recv_buffer, "exit", 4) == 0) {
                break;
            }

            /*
             * 자식 메시지 입력
             */
            printf("[자식 입력] ");
            fflush(stdout);

            if (fgets(
                    send_buffer,
                    sizeof(send_buffer),
                    stdin
                ) == NULL) {
                break;
            }

            /*
             * 부모에게 메시지 전송
             */
            if (write(
                    child_to_parent[1],
                    send_buffer,
                    strlen(send_buffer) + 1
                ) == -1) {

                perror("write");
                break;
            }

            /*
             * 자식이 exit 입력
             */
            if (strncmp(send_buffer, "exit", 4) == 0) {
                break;
            }
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);

        exit(0);
    }

    /*
     * 부모 프로세스
     */
    else {

        // 사용하지 않는 파이프 닫기
        close(parent_to_child[0]);
        close(child_to_parent[1]);

        while (1) {

            /*
             * 부모 메시지 입력
             */
            printf("[부모 입력] ");
            fflush(stdout);

            if (fgets(
                    send_buffer,
                    sizeof(send_buffer),
                    stdin
                ) == NULL) {
                break;
            }

            /*
             * 자식에게 메시지 전송
             */
            if (write(
                    parent_to_child[1],
                    send_buffer,
                    strlen(send_buffer) + 1
                ) == -1) {

                perror("write");
                break;
            }

            /*
             * 부모가 exit 입력
             */
            if (strncmp(send_buffer, "exit", 4) == 0) {
                break;
            }

            memset(recv_buffer, 0, sizeof(recv_buffer));

            /*
             * 자식 응답 수신
             */
            ssize_t n = read(
                child_to_parent[0],
                recv_buffer,
                sizeof(recv_buffer) - 1
            );

            if (n <= 0) {
                break;
            }

            printf("[자식] %s", recv_buffer);

            /*
             * 자식이 exit 입력
             */
            if (strncmp(recv_buffer, "exit", 4) == 0) {
                break;
            }
        }

        close(parent_to_child[1]);
        close(child_to_parent[0]);

        wait(NULL);

        printf("\n채팅 프로그램을 종료합니다.\n");
    }

    return 0;
}
