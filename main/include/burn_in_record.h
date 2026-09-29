#pragma once

#include <stdint.h>

typedef struct {
    uint32_t frames;
    uint32_t capture_failures;
    uint64_t jpeg_bytes;
    uint32_t elapsed_seconds;
    uint32_t minimum_heap_bytes;
    uint32_t minimum_largest_8bit_bytes;
    uint32_t minimum_psram_free_bytes;
} catcam_burn_in_record_t;

void catcam_burn_in_record_init(catcam_burn_in_record_t *record,
                                uint32_t heap_bytes,
                                uint32_t largest_8bit_bytes,
                                uint32_t psram_free_bytes);
void catcam_burn_in_record_update(catcam_burn_in_record_t *record,
                                  uint32_t frames,
                                  uint32_t capture_failures,
                                  uint64_t jpeg_bytes,
                                  uint32_t elapsed_seconds,
                                  uint32_t heap_bytes,
                                  uint32_t largest_8bit_bytes,
                                  uint32_t psram_free_bytes);
