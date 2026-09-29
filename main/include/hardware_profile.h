#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t flash_mb;
    uint8_t psram_mb;
    int tf_cmd_gpio;
    int tf_clk_gpio;
    int tf_dat0_gpio;
    bool camera_pins_verified;
} catcam_hardware_profile_t;

typedef struct {
    int pin_pwdn;
    int pin_reset;
    int pin_xclk;
    int pin_sccb_sda;
    int pin_sccb_scl;
    int pin_d7;
    int pin_d6;
    int pin_d5;
    int pin_d4;
    int pin_d3;
    int pin_d2;
    int pin_d1;
    int pin_d0;
    int pin_vsync;
    int pin_href;
    int pin_pclk;
} catcam_camera_profile_t;

catcam_hardware_profile_t catcam_hardware_profile(void);
catcam_camera_profile_t catcam_camera_profile(void);
