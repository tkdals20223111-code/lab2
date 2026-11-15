#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <time.h>

int main(void)
{
    int fd;
    int nread;
    int count = 0;
    int errcnt = 0;

    char ch;

    const char text[] =
        "The magic thing is that you can change it.";

    struct termios init_attr;
    struct termios new_attr;

    time_t start_time;
    time_t end_time;

    double elapsed;
    double cpm;

    /*
     * 현재 터미널 열기
     */
    fd = open(ttyname(fileno(stdin)), O_RDWR);

    if (fd < 0) {
        perror("open");
        exit(1);
    }

    /*
     * 현재 터미널 설정 저장
     */
    if (tcgetattr(fd, &init_attr) != 0) {
        perror("tcgetattr");
        close(fd);
        exit(1);
    }

    new_attr = init_attr;

    /*
     * 비정규 모드 설정
     * ICANON 제거 : 한 글자씩 입력 처리
     * ECHO 제거   : 입력 문자를 자동 표시하지 않음
     */
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO;

    /*
     * 최소 1글자가 입력될 때까지 대기
     */
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        perror("tcsetattr");
        close(fd);
        exit(1);
    }

    printf("다음 문장을 그대로 입력하세요.\n");
    printf("%s\n\n", text);

    fflush(stdout);

    /*
     * 타자 시작 시간
     */
    start_time = time(NULL);

    /*
     * 문자 단위 입력
     */
    while ((nread = read(fd, &ch, 1)) > 0) {

        /*
         * Enter 입력 시 종료
         */
        if (ch == '\n') {
            break;
        }

        /*
         * 원문 길이를 넘어서 입력한 경우
         */
        if (count >= (int)strlen(text)) {
            write(fd, "*", 1);
            errcnt++;
            count++;
            continue;
        }

        /*
         * 입력 문자가 원문과 같으면 그대로 출력
         */
        if (ch == text[count]) {
            write(fd, &ch, 1);
        }
        else {
            /*
             * 틀린 문자는 * 표시
             */
            write(fd, "*", 1);
            errcnt++;
        }

        count++;
    }

    /*
     * 타자 종료 시간
     */
    end_time = time(NULL);

    /*
     * 터미널 원래 상태로 복구
     */
    tcsetattr(fd, TCSANOW, &init_attr);

    close(fd);

    /*
     * 경과 시간 계산
     */
    elapsed = difftime(end_time, start_time);

    /*
     * 0으로 나누는 문제 방지
     */
    if (elapsed <= 0) {
        elapsed = 1;
    }

    /*
     * 분당 타자수 계산
     * CPM = 입력 문자 수 / 경과시간(초) * 60
     */
    cpm = ((double)count / elapsed) * 60.0;

    printf("\n\n=== 타자 연습 결과 ===\n");
    printf("입력 문자 수       : %d\n", count);
    printf("잘못 입력한 횟수   : %d\n", errcnt);
    printf("걸린 시간          : %.0f초\n", elapsed);
    printf("평균 분당 타자수   : %.2f타/분\n", cpm);

    return 0;
}
