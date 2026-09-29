#include <assert.h>
#include <stdio.h>

#include "hardware_profile.h"

int main(void)
{
    const catcam_hardware_profile_t profile = catcam_hardware_profile();
    const catcam_camera_profile_t camera = catcam_camera_profile();

    assert(profile.flash_mb == 16);
    assert(profile.psram_mb == 8);
    assert(profile.tf_cmd_gpio == 38);
    assert(profile.tf_clk_gpio == 39);
    assert(profile.tf_dat0_gpio == 40);
    assert(profile.camera_pins_verified);

    assert(camera.pin_pwdn == -1);
    assert(camera.pin_reset == -1);
    assert(camera.pin_xclk == 15);
    assert(camera.pin_sccb_sda == 4);
    assert(camera.pin_sccb_scl == 5);
    assert(camera.pin_d7 == 16);
    assert(camera.pin_d6 == 17);
    assert(camera.pin_d5 == 18);
    assert(camera.pin_d4 == 12);
    assert(camera.pin_d3 == 10);
    assert(camera.pin_d2 == 8);
    assert(camera.pin_d1 == 9);
    assert(camera.pin_d0 == 11);
    assert(camera.pin_vsync == 6);
    assert(camera.pin_href == 7);
    assert(camera.pin_pclk == 13);

    puts("hardware profile test passed");
    return 0;
}
