#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <systemd/sd-bus.h>

int main(void)
{
    sd_bus *bus = NULL;
    sd_bus_message *reply = NULL;
    sd_bus_error error = SD_BUS_ERROR_NULL;

    const char *response = NULL;
    char message[256];

    int r;

    /*
     * 사용자 D-Bus에 연결
     */
    r = sd_bus_default_user(&bus);

    if (r < 0) {
        fprintf(stderr,
                "D-Bus 연결 실패: %s\n",
                strerror(-r));
        goto finish;
    }

    /*
     * 알림 메시지 입력
     */
    printf("전송할 알림 메시지를 입력하세요: ");
    fflush(stdout);

    if (fgets(message, sizeof(message), stdin) == NULL) {
        fprintf(stderr, "메시지 입력 실패\n");
        r = -1;
        goto finish;
    }

    /*
     * 마지막 개행 제거
     */
    message[strcspn(message, "\n")] = '\0';

    /*
     * 서버의 SendNotification 메서드 호출
     */
    r = sd_bus_call_method(
        bus,
        "org.example.NotificationService",
        "/org/example/NotificationService",
        "org.example.NotificationService",
        "SendNotification",
        &error,
        &reply,
        "s",
        message
    );

    if (r < 0) {
        fprintf(stderr,
                "메서드 호출 실패: %s\n",
                error.message ?
                error.message :
                strerror(-r));

        goto finish;
    }

    /*
     * 서버 응답 읽기
     */
    r = sd_bus_message_read(
        reply,
        "s",
        &response
    );

    if (r < 0) {
        fprintf(stderr,
                "응답 읽기 실패: %s\n",
                strerror(-r));

        goto finish;
    }

    printf("[클라이언트] 서버 응답: %s\n",
           response);

finish:

    sd_bus_error_free(&error);
    sd_bus_message_unref(reply);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
