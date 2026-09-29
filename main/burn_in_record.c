#include "burn_in_record.h"

static uint32_t minimum(uint32_t current, uint32_t candidate)
{
    return candidate < current ? candidate : current;
}

void catcam_burn_in_record_init(catcam_burn_in_record_t *record,
                                uint32_t heap_bytes,
                                uint32_t largest_8bit_bytes,
                                uint32_t psram_free_bytes)
{
    *record = (catcam_burn_in_record_t) {
        .minimum_heap_bytes = heap_bytes,
        .minimum_largest_8bit_bytes = largest_8bit_bytes,
        .minimum_psram_free_bytes = psram_free_bytes,
    };
}

void catcam_burn_in_record_update(catcam_burn_in_record_t *record,
                                  uint32_t frames,
                                  uint32_t capture_failures,
                                  uint64_t jpeg_bytes,
                                  uint32_t elapsed_seconds,
                                  uint32_t heap_bytes,
                                  uint32_t largest_8bit_bytes,
                                  uint32_t psram_free_bytes)
{
    record->frames = frames;
    record->capture_failures = capture_failures;
    record->jpeg_bytes = jpeg_bytes;
    record->elapsed_seconds = elapsed_seconds;
    record->minimum_heap_bytes = minimum(record->minimum_heap_bytes, heap_bytes);
    record->minimum_largest_8bit_bytes = minimum(record->minimum_largest_8bit_bytes,
                                                  largest_8bit_bytes);
    record->minimum_psram_free_bytes = minimum(record->minimum_psram_free_bytes,
                                                psram_free_bytes);
}
