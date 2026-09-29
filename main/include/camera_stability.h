#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint32_t frames;
    uint32_t capture_failures;
    uint64_t jpeg_bytes;
    uint64_t reported_at_us;
} catcam_camera_stability_stats_t;

void catcam_camera_stability_init(catcam_camera_stability_stats_t *stats, uint64_t started_at_us);
bool catcam_camera_stability_record_frame(catcam_camera_stability_stats_t *stats,
                                          uint64_t now_us, uint32_t jpeg_bytes);
bool catcam_camera_stability_record_failure(catcam_camera_stability_stats_t *stats,
                                            uint64_t now_us);
