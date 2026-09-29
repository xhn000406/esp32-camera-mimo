#include <assert.h>
#include <stdio.h>

#include "burn_in_schedule.h"

int main(void)
{
    catcam_burn_in_schedule_t schedule;
    catcam_burn_in_schedule_init(&schedule, 1000000);

    assert(!catcam_burn_in_schedule_checkpoint_due(&schedule, 300999999));
    assert(catcam_burn_in_schedule_checkpoint_due(&schedule, 301000000));
    assert(!catcam_burn_in_schedule_checkpoint_due(&schedule, 302000000));
    puts("burn-in schedule test passed");
}
