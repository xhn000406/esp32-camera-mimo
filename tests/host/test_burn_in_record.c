#include <assert.h>
#include <stdio.h>

#include "burn_in_record.h"

int main(void)
{
    catcam_burn_in_record_t record;
    catcam_burn_in_record_init(&record, 100, 80, 70);

    catcam_burn_in_record_update(&record, 120, 0, 1200, 10, 95, 75, 65);
    catcam_burn_in_record_update(&record, 240, 1, 3600, 20, 90, 78, 60);

    assert(record.frames == 240);
    assert(record.capture_failures == 1);
    assert(record.jpeg_bytes == 3600);
    assert(record.elapsed_seconds == 20);
    assert(record.minimum_heap_bytes == 90);
    assert(record.minimum_largest_8bit_bytes == 75);
    assert(record.minimum_psram_free_bytes == 60);
    puts("burn-in record test passed");
}
