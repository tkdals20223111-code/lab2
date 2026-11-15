#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main(void)
{
    int fd;
    char buffer[100];
    ssize_t nread;

    DIR *dir;
    struct dirent *entry;

    printf("=== 파일 및 디렉토리 함수 실습 ===\n\n");

    /*
     * 1. 디렉토리 생성
     */
    if (mkdir("practice_dir", 0755) == -1) {
        perror("mkdir");
    } else {
        printf("[1] practice_dir 디렉토리 생성 완료\n");
    }

    /*
     * 2. 파일 생성 및 열기
     */
    fd = open(
        "practice_dir/test.txt",
        O_CREAT | O_RDWR | O_TRUNC,
        0644
    );

    if (fd == -1) {
        perror("open");
        exit(1);
    }

    printf("[2] test.txt 파일 열기 완료\n");

    /*
     * 3. 파일에 데이터 쓰기
     */
    const char *message =
        "Hello Linux File Processing!\n";

    if (write(fd, message, strlen(message)) == -1) {
        perror("write");
        close(fd);
        exit(1);
    }

    printf("[3] 파일 쓰기 완료\n");

    /*
     * 4. 파일 위치를 처음으로 이동
     */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek");
        close(fd);
        exit(1);
    }

    /*
     * 5. 파일 내용 읽기
     */
    memset(buffer, 0, sizeof(buffer));

    nread = read(
        fd,
        buffer,
        sizeof(buffer) - 1
    );

    if (nread == -1) {
        perror("read");
        close(fd);
        exit(1);
    }

    printf("[4] 파일 읽기 결과: %s", buffer);

    /*
     * 6. 파일 닫기
     */
    close(fd);

    printf("[5] 파일 닫기 완료\n\n");

    /*
     * 7. 디렉토리 열기
     */
    dir = opendir("practice_dir");

    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    printf("=== practice_dir 내부 목록 ===\n");

    /*
     * 8. 디렉토리 항목 읽기
     */
    while ((entry = readdir(dir)) != NULL) {
        printf("%s\n", entry->d_name);
    }

    /*
     * 9. 디렉토리 닫기
     */
    closedir(dir);

    printf("\n디렉토리 탐색 완료\n");

    return 0;
}
