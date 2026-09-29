#pragma once

#include <stddef.h>
#include <stdint.h>

#include "esp_err.h"

typedef struct {
    uint16_t sensor_pid;
    uint16_t width;
    uint16_t height;
    size_t jpeg_bytes;
} catcam_camera_probe_result_t;

esp_err_t catcam_camera_start(uint16_t *sensor_pid);
esp_err_t catcam_camera_capture_frame(catcam_camera_probe_result_t *result);
esp_err_t catcam_camera_stop(void);
esp_err_t catcam_camera_probe_once(catcam_camera_probe_result_t *result);
