#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int src_fd;
    int dst_fd;

    struct stat src_stat;

    void *src_addr;
    void *dst_addr;

    /*
     * 사용법 확인
     */
    if (argc != 3) {
        printf("사용법: %s <원본파일> <목적파일>\n", argv[0]);
        return 1;
    }

    /*
     * 1. 원본 파일 열기
     */
    src_fd = open(argv[1], O_RDONLY);

    if (src_fd == -1) {
        perror("open source");
        exit(1);
    }

    /*
     * 2. 원본 파일 크기 확인
     */
    if (fstat(src_fd, &src_stat) == -1) {
        perror("fstat");
        close(src_fd);
        exit(1);
    }

    if (src_stat.st_size == 0) {
        printf("원본 파일이 비어 있습니다.\n");
        close(src_fd);
        return 1;
    }

    /*
     * 3. 목적 파일 생성
     */
    dst_fd = open(
        argv[2],
        O_RDWR | O_CREAT | O_TRUNC,
        0644
    );

    if (dst_fd == -1) {
        perror("open destination");
        close(src_fd);
        exit(1);
    }

    /*
     * 4. 목적 파일 크기를
     *    원본 파일 크기와 같게 설정
     */
    if (ftruncate(dst_fd, src_stat.st_size) == -1) {
        perror("ftruncate");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }

    /*
     * 5. 원본 파일 mmap
     */
    src_addr = mmap(
        NULL,
        src_stat.st_size,
        PROT_READ,
        MAP_PRIVATE,
        src_fd,
        0
    );

    if (src_addr == MAP_FAILED) {
        perror("mmap source");
        close(src_fd);
        close(dst_fd);
        exit(1);
    }

    /*
     * 6. 목적 파일 mmap
     */
    dst_addr = mmap(
        NULL,
        src_stat.st_size,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        dst_fd,
        0
    );

    if (dst_addr == MAP_FAILED) {
        perror("mmap destination");

        munmap(src_addr, src_stat.st_size);

        close(src_fd);
        close(dst_fd);

        exit(1);
    }

    /*
     * 7. 메모리 복사를 이용해
     *    원본 내용을 목적 파일로 복사
     */
    memcpy(
        dst_addr,
        src_addr,
        src_stat.st_size
    );

    /*
     * 8. 변경 내용을 파일에 반영
     */
    if (msync(
            dst_addr,
            src_stat.st_size,
            MS_SYNC
        ) == -1) {

        perror("msync");
    }

    /*
     * 9. mmap 해제
     */
    munmap(src_addr, src_stat.st_size);
    munmap(dst_addr, src_stat.st_size);

    /*
     * 10. 파일 닫기
     */
    close(src_fd);
    close(dst_fd);

    printf("파일 복사가 완료되었습니다.\n");
    printf("원본 파일 : %s\n", argv[1]);
    printf("복사 파일 : %s\n", argv[2]);

    return 0;
}
