#include <assert.h>
#include <stdio.h>

#include "camera_stability.h"

int main(void)
{
    catcam_camera_stability_stats_t stats;
    catcam_camera_stability_init(&stats, 1000000);

    assert(!catcam_camera_stability_record_frame(&stats, 1500000, 1000));
    assert(!catcam_camera_stability_record_frame(&stats, 9000000, 2000));
    assert(catcam_camera_stability_record_failure(&stats, 11000000));

    assert(stats.frames == 2);
    assert(stats.capture_failures == 1);
    assert(stats.jpeg_bytes == 3000);
    assert(stats.reported_at_us == 11000000);

    puts("camera stability test passed");
    return 0;
}
