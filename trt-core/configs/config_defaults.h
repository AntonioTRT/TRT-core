#pragma once

// Compile-time configuration defaults for TRT-core runtime policy.
// Boards may override or extend these defaults during platform integration.

#define TRT_CFG_BOARD_NAME "TRT_CORE"
#define TRT_CFG_BOARD_TYPE "GENERIC"
#define TRT_CFG_BOARD_REVISION "A1"

#define TRT_CFG_PROTOCOL_VERSION 1
#define TRT_CFG_DEBUG_LEVEL 1

#define TRT_CFG_LED_RING_ENABLED 1
#define TRT_CFG_LED_RING_PIXEL_COUNT 16
#define TRT_CFG_LED_RING_BRIGHTNESS 50

#define TRT_CFG_ENCODER_ENABLED 1
#define TRT_CFG_ENCODER_REVERSE_DIRECTION 0
#define TRT_CFG_ENCODER_STEPS_PER_DETENT 4
