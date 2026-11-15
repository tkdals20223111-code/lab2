#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void list_directory(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char fullpath[1024];

    dir = opendir(path);

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    printf("\n%s:\n", path);

    // 현재 디렉토리의 파일 및 디렉토리 목록 출력
    while ((entry = readdir(dir)) != NULL) {

        // "."과 ".."은 재귀 탐색에서 제외
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        printf("%s  ", entry->d_name);
    }

    printf("\n");

    // 디렉토리 스트림 위치를 처음으로 되돌림
    rewinddir(dir);

    // 하위 디렉토리를 찾아 재귀적으로 탐색
    while ((entry = readdir(dir)) != NULL) {

        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        snprintf(
            fullpath,
            sizeof(fullpath),
            "%s/%s",
            path,
            entry->d_name
        );

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        // 현재 항목이 디렉토리라면 다시 탐색
        if (S_ISDIR(statbuf.st_mode)) {
            list_directory(fullpath);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("사용법: %s <디렉토리 경로>\n", argv[0]);
        return 1;
    }

    list_directory(argv[1]);

    return 0;
}
