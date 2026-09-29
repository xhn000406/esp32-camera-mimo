#include "hardware_profile.h"

catcam_hardware_profile_t catcam_hardware_profile(void)
{
    return (catcam_hardware_profile_t){
        .flash_mb = 16,
        .psram_mb = 8,
        .tf_cmd_gpio = 38,
        .tf_clk_gpio = 39,
        .tf_dat0_gpio = 40,
        .camera_pins_verified = true,
    };
}

catcam_camera_profile_t catcam_camera_profile(void)
{
    return (catcam_camera_profile_t) {
        .pin_pwdn = -1,
        .pin_reset = -1,
        .pin_xclk = 15,
        .pin_sccb_sda = 4,
        .pin_sccb_scl = 5,
        .pin_d7 = 16,
        .pin_d6 = 17,
        .pin_d5 = 18,
        .pin_d4 = 12,
        .pin_d3 = 10,
        .pin_d2 = 8,
        .pin_d1 = 9,
        .pin_d0 = 11,
        .pin_vsync = 6,
        .pin_href = 7,
        .pin_pclk = 13,
    };
}
