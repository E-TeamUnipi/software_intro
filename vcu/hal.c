#include "hal.h"
#include <posix_hal/scheduler.h>
#include <posix_hal/can.h>

static int can_sockets[1];

bool hal_init(void) {
    phal_scheduler_init(1000);
    return true;
}

bool hal_can_init(void) {
    phal_can_init_socket(&can_sockets[0], "vcan");
    return phal_can_start_receiver_thread(can_sockets, 2, NULL, true);
}

bool hal_send_can_message(uint32_t addr, const uint8_t* data, uint8_t length) {
    return phal_can_send_message(
        can_sockets[0],
        addr,
        data,
        length);
}

void phal_can_receiver_handle(int sock, uint32_t id, const uint8_t *payload, uint8_t len) {
    if (sock == can_sockets[0]) {
        hal_handle_can_message(id, payload, len);
    }
}

void hal_start_scheduler() {
    phal_scheduler_start();
}

uint8_t hal_scheduler_add_task(uint32_t period_ms, scheduler_callback_t cb) {
    return phal_scheduler_add_periodic_callback(period_ms, cb, NULL);
}
