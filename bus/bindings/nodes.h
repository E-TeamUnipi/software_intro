#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAIN_DBC_VERSION "0.0.1"
#define MAIN_NODE_BITS 3U
#define MAIN_NODE_MASK 0x7U
#define MAIN_MSG_MASK 0x7f8U

#define MAIN_NODE_VCU 0U
#define MAIN_NODE_SB1 1U
#define MAIN_NODE_SB2 2U
#define MAIN_NODE_COOLING_CTRL 3U
#define MAIN_NODE_INVERTER 4U
#define MAIN_NODE_SIM 5U

#define SIM_DBC_VERSION "0.0.1"
#define SIM_NODE_BITS 3U
#define SIM_NODE_MASK 0x7U
#define SIM_MSG_MASK 0x7f8U

#define SIM_NODE_VCU 0U
#define SIM_NODE_SB1 1U
#define SIM_NODE_SB2 2U
#define SIM_NODE_COOLING_CTRL 3U
#define SIM_NODE_INVERTER 4U
#define SIM_NODE_SIM 5U



#ifdef __cplusplus
}
#endif
