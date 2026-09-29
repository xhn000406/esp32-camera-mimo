#include "camera_stability.h"

#define CATCAM_STABILITY_REPORT_INTERVAL_US (10ULL * 1000ULL * 1000ULL)

void catcam_camera_stability_init(catcam_camera_stability_stats_t *stats, uint64_t started_at_us)
{
    *stats = (catcam_camera_stability_stats_t) {
        .reported_at_us = started_at_us,
    };
}

static bool should_report(catcam_camera_stability_stats_t *stats, uint64_t now_us)
{
    if (now_us - stats->reported_at_us < CATCAM_STABILITY_REPORT_INTERVAL_US) {
        return false;
    }

    stats->reported_at_us = now_us;
    return true;
}

bool catcam_camera_stability_record_frame(catcam_camera_stability_stats_t *stats,
                                          uint64_t now_us, uint32_t jpeg_bytes)
{
    stats->frames++;
    stats->jpeg_bytes += jpeg_bytes;
    return should_report(stats, now_us);
}

bool catcam_camera_stability_record_failure(catcam_camera_stability_stats_t *stats,
                                            uint64_t now_us)
{
    stats->capture_failures++;
    return should_report(stats, now_us);
}
