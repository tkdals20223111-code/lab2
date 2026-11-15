#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <unistd.h>

#define QUEUE_PARENT_TO_CHILD "/parent_to_child"
#define QUEUE_CHILD_TO_PARENT "/child_to_parent"

#define BUFFER_SIZE 256

int main(void)
{
    mqd_t mq_parent_to_child;
    mqd_t mq_child_to_parent;

    struct mq_attr attr;

    pid_t pid;

    char send_buffer[BUFFER_SIZE];
    char recv_buffer[BUFFER_SIZE];

    /*
     * 메시지 큐 속성 설정
     */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUFFER_SIZE;
    attr.mq_curmsgs = 0;

    /*
     * 기존 메시지 큐가 남아 있을 경우 삭제
     */
    mq_unlink(QUEUE_PARENT_TO_CHILD);
    mq_unlink(QUEUE_CHILD_TO_PARENT);

    /*
     * 부모 -> 자식 메시지 큐 생성
     */
    mq_parent_to_child = mq_open(
        QUEUE_PARENT_TO_CHILD,
        O_CREAT | O_RDWR,
        0666,
        &attr
    );

    if (mq_parent_to_child == (mqd_t)-1) {
        perror("mq_open parent_to_child");
        exit(1);
    }

    /*
     * 자식 -> 부모 메시지 큐 생성
     */
    mq_child_to_parent = mq_open(
        QUEUE_CHILD_TO_PARENT,
        O_CREAT | O_RDWR,
        0666,
        &attr
    );

    if (mq_child_to_parent == (mqd_t)-1) {
        perror("mq_open child_to_parent");
        mq_close(mq_parent_to_child);
        mq_unlink(QUEUE_PARENT_TO_CHILD);
        exit(1);
    }

    printf("=== POSIX 메시지 큐 채팅 ===\n");
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

        while (1) {

            memset(recv_buffer, 0, sizeof(recv_buffer));

            /*
             * 부모의 메시지 수신
             */
            if (mq_receive(
                    mq_parent_to_child,
                    recv_buffer,
                    BUFFER_SIZE,
                    NULL
                ) == -1) {

                perror("mq_receive");
                break;
            }

            printf("[부모] %s", recv_buffer);

            /*
             * 부모가 exit 입력 시 종료
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
            if (mq_send(
                    mq_child_to_parent,
                    send_buffer,
                    strlen(send_buffer) + 1,
                    0
                ) == -1) {

                perror("mq_send");
                break;
            }

            /*
             * 자식이 exit 입력 시 종료
             */
            if (strncmp(send_buffer, "exit", 4) == 0) {
                break;
            }
        }

        mq_close(mq_parent_to_child);
        mq_close(mq_child_to_parent);

        exit(0);
    }

    /*
     * 부모 프로세스
     */
    else {

        while (1) {

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
            if (mq_send(
                    mq_parent_to_child,
                    send_buffer,
                    strlen(send_buffer) + 1,
                    0
                ) == -1) {

                perror("mq_send");
                break;
            }

            /*
             * 부모가 exit 입력 시 종료
             */
            if (strncmp(send_buffer, "exit", 4) == 0) {
                break;
            }

            memset(recv_buffer, 0, sizeof(recv_buffer));

            /*
             * 자식 메시지 수신
             */
            if (mq_receive(
                    mq_child_to_parent,
                    recv_buffer,
                    BUFFER_SIZE,
                    NULL
                ) == -1) {

                perror("mq_receive");
                break;
            }

            printf("[자식] %s", recv_buffer);

            /*
             * 자식이 exit 입력 시 종료
             */
            if (strncmp(recv_buffer, "exit", 4) == 0) {
                break;
            }
        }

        /*
         * 자식 종료 대기
         */
        wait(NULL);

        /*
         * 메시지 큐 닫기
         */
        mq_close(mq_parent_to_child);
        mq_close(mq_child_to_parent);

        /*
         * 메시지 큐 삭제
         */
        mq_unlink(QUEUE_PARENT_TO_CHILD);
        mq_unlink(QUEUE_CHILD_TO_PARENT);

        printf("\n메시지 큐 채팅 프로그램을 종료합니다.\n");
    }

    return 0;
}
