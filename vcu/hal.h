#pragma once

#include <stdbool.h>
#include <stdint.h>

bool hal_can_init();
extern bool hal_send_can_message(uint32_t addr, const uint8_t* data, uint8_t length);
extern void hal_handle_can_message(uint32_t addr, const uint8_t* data, uint8_t length);
