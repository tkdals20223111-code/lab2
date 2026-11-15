#include <stdio.h>
#include <stdlib.h>
#include <systemd/sd-bus.h>

/*
 * 클라이언트가 호출할 SendNotification 메서드
 */
static int method_send_notification(
    sd_bus_message *m,
    void *userdata,
    sd_bus_error *ret_error)
{
    const char *message;
    int r;

    /*
     * 클라이언트가 보낸 문자열 읽기
     */
    r = sd_bus_message_read(m, "s", &message);

    if (r < 0) {
        fprintf(stderr, "메시지를 읽는 데 실패했습니다.\n");
        return r;
    }

    /*
     * 서버에서 알림 메시지 출력
     */
    printf("[서버] 알림 메시지 수신: %s\n", message);

    /*
     * 클라이언트에게 응답 반환
     */
    return sd_bus_reply_method_return(
        m,
        "s",
        "알림 메시지를 정상적으로 받았습니다."
    );
}

/*
 * D-Bus 인터페이스 정의
 */
static const sd_bus_vtable notification_vtable[] = {

    SD_BUS_VTABLE_START(0),

    SD_BUS_METHOD(
        "SendNotification",
        "s",
        "s",
        method_send_notification,
        SD_BUS_VTABLE_UNPRIVILEGED
    ),

    SD_BUS_VTABLE_END
};

int main(void)
{
    sd_bus *bus = NULL;
    sd_bus_slot *slot = NULL;

    int r;

    /*
     * 1. 사용자 세션 D-Bus 연결
     */
    r = sd_bus_default_user(&bus);

    if (r < 0) {
        fprintf(stderr, "D-Bus 연결 실패\n");
        goto finish;
    }

    /*
     * 2. 서비스 이름 등록
     */
    r = sd_bus_request_name(
        bus,
        "org.example.NotificationService",
        0
    );

    if (r < 0) {
        fprintf(stderr, "서비스 이름 등록 실패\n");
        goto finish;
    }

    /*
     * 3. 객체 경로와 인터페이스 등록
     */
    r = sd_bus_add_object_vtable(
        bus,
        &slot,
        "/org/example/NotificationService",
        "org.example.NotificationService",
        notification_vtable,
        NULL
    );

    if (r < 0) {
        fprintf(stderr, "객체 인터페이스 등록 실패\n");
        goto finish;
    }

    printf("=== D-Bus 알림 서버 시작 ===\n");
    printf("서비스 이름 : org.example.NotificationService\n");
    printf("객체 경로   : /org/example/NotificationService\n");
    printf("클라이언트 메시지를 기다리는 중...\n");

    /*
     * 4. 메시지 처리 반복
     */
    while (1) {

        r = sd_bus_process(bus, NULL);

        if (r < 0) {
            fprintf(stderr, "D-Bus 메시지 처리 실패\n");
            break;
        }

        /*
         * 처리할 메시지가 없으면 대기
         */
        if (r == 0) {

            r = sd_bus_wait(
                bus,
                (uint64_t)-1
            );

            if (r < 0) {
                fprintf(stderr, "D-Bus 대기 실패\n");
                break;
            }
        }
    }

finish:

    sd_bus_slot_unref(slot);
    sd_bus_unref(bus);

    return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
