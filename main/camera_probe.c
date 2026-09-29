#include "camera_probe.h"

#include <string.h>

#include "esp_camera.h"
#include "hardware_profile.h"

static camera_config_t catcam_camera_config(void)
{
    const catcam_camera_profile_t pins = catcam_camera_profile();
    return (camera_config_t) {
        .pin_pwdn = pins.pin_pwdn,
        .pin_reset = pins.pin_reset,
        .pin_xclk = pins.pin_xclk,
        .pin_sccb_sda = pins.pin_sccb_sda,
        .pin_sccb_scl = pins.pin_sccb_scl,
        .pin_d7 = pins.pin_d7,
        .pin_d6 = pins.pin_d6,
        .pin_d5 = pins.pin_d5,
        .pin_d4 = pins.pin_d4,
        .pin_d3 = pins.pin_d3,
        .pin_d2 = pins.pin_d2,
        .pin_d1 = pins.pin_d1,
        .pin_d0 = pins.pin_d0,
        .pin_vsync = pins.pin_vsync,
        .pin_href = pins.pin_href,
        .pin_pclk = pins.pin_pclk,
        .xclk_freq_hz = 20000000,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,
        .pixel_format = PIXFORMAT_JPEG,
        .frame_size = FRAMESIZE_SVGA,
        .jpeg_quality = 12,
        .fb_count = 1,
        .fb_location = CAMERA_FB_IN_PSRAM,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
    };
}

esp_err_t catcam_camera_start(uint16_t *sensor_pid)
{
    camera_config_t config = catcam_camera_config();
    esp_err_t err = esp_camera_init(&config);
    if (err == ESP_OK && sensor_pid != NULL) {
        *sensor_pid = esp_camera_sensor_get()->id.PID;
    }
    return err;
}

esp_err_t catcam_camera_capture_frame(catcam_camera_probe_result_t *result)
{
    camera_fb_t *frame = esp_camera_fb_get();
    if (frame == NULL) {
        return ESP_FAIL;
    }

    if (result != NULL) {
        sensor_t *sensor = esp_camera_sensor_get();
        memset(result, 0, sizeof(*result));
        result->sensor_pid = sensor->id.PID;
        result->width = frame->width;
        result->height = frame->height;
        result->jpeg_bytes = frame->len;
    }

    esp_camera_fb_return(frame);
    return ESP_OK;
}

esp_err_t catcam_camera_stop(void)
{
    return esp_camera_deinit();
}

esp_err_t catcam_camera_probe_once(catcam_camera_probe_result_t *result)
{
    esp_err_t err = catcam_camera_start(NULL);
    if (err != ESP_OK) {
        return err;
    }

    err = catcam_camera_capture_frame(result);
    const esp_err_t stop_err = catcam_camera_stop();
    return err != ESP_OK ? err : stop_err;
}
