#include <assert.h>
#include <stdio.h>

#include "wifi_preview.h"

int main(void)
{
    catcam_wifi_preview_release();
    assert(catcam_wifi_preview_try_acquire());
    assert(!catcam_wifi_preview_try_acquire());
    catcam_wifi_preview_release();
    assert(catcam_wifi_preview_try_acquire());
    catcam_wifi_preview_release();
    puts("wifi preview admission test passed");
}
