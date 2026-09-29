#include "wifi_preview.h"

#include <stdatomic.h>

static atomic_bool preview_in_use = ATOMIC_VAR_INIT(false);

bool catcam_wifi_preview_try_acquire(void)
{
    bool expected = false;
    return atomic_compare_exchange_strong(&preview_in_use, &expected, true);
}

void catcam_wifi_preview_release(void)
{
    atomic_store(&preview_in_use, false);
}
