#include <stdio.h>

#include "boot_report.h"
#include "burn_in_checkpoint.h"
#include "burn_in_record.h"
#include "burn_in_schedule.h"
#include "camera_probe.h"
#include "camera_stability.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_psram.h"
#include "esp_system.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hardware_profile.h"

static const char *TAG = "catcam-stage1";

static const char *reset_reason_name(esp_reset_reason_t reason)
{
    switch (reason) {
    case ESP_RST_POWERON:
        return "power-on";
    case ESP_RST_EXT:
        return "external";
    case ESP_RST_SW:
        return "software";
    case ESP_RST_PANIC:
        return "panic";
    case ESP_RST_INT_WDT:
        return "interrupt-watchdog";
    case ESP_RST_TASK_WDT:
        return "task-watchdog";
    case ESP_RST_WDT:
        return "other-watchdog";
    case ESP_RST_DEEPSLEEP:
        return "deep-sleep";
    case ESP_RST_BROWNOUT:
        return "brownout";
    case ESP_RST_SDIO:
        return "sdio";
    default:
        return "unknown";
    }
}

void app_main(void)
{
    esp_chip_info_t chip_info;
    uint32_t flash_bytes = 0;
    uint8_t mac[6];
    char mac_text[18];
    char report[256];
    const catcam_hardware_profile_t profile = catcam_hardware_profile();

    esp_chip_info(&chip_info);
    ESP_ERROR_CHECK(esp_flash_get_size(NULL, &flash_bytes));
    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_STA));

    snprintf(mac_text, sizeof(mac_text), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    const catcam_boot_facts_t facts = {
        .build_id = IDF_VER,
        .reset_reason = reset_reason_name(esp_reset_reason()),
        .flash_mb = flash_bytes / (1024U * 1024U),
        .psram_mb = esp_psram_get_size() / (1024U * 1024U),
        .mac = mac_text,
    };

    ESP_LOGI(TAG, "chip=ESP32-S3 cores=%d revision=%d", chip_info.cores, chip_info.revision);
    ESP_LOGI(TAG, "reference_tf cmd=%d clk=%d dat0=%d camera_pins_verified=%s",
             profile.tf_cmd_gpio, profile.tf_clk_gpio, profile.tf_dat0_gpio,
             profile.camera_pins_verified ? "true" : "false");

    if (catcam_format_boot_report(&facts, report, sizeof(report)) >= 0) {
        ESP_LOGI(TAG, "%s", report);
    }

    const esp_err_t checkpoint_init_err = catcam_burn_in_checkpoint_init();
    if (checkpoint_init_err == ESP_OK) {
        catcam_burn_in_record_t prior_record;
        bool prior_record_found = false;
        const esp_err_t checkpoint_load_err =
            catcam_burn_in_checkpoint_load(&prior_record, &prior_record_found);
        if (checkpoint_load_err == ESP_OK && prior_record_found) {
            ESP_LOGI(TAG, "prior_burn_in elapsed_s=%u frames=%u failures=%u min_heap=%u "
                          "min_largest_8bit=%u min_psram_free=%u",
                     prior_record.elapsed_seconds, prior_record.frames,
                     prior_record.capture_failures, prior_record.minimum_heap_bytes,
                     prior_record.minimum_largest_8bit_bytes,
                     prior_record.minimum_psram_free_bytes);
        } else if (checkpoint_load_err != ESP_OK) {
            ESP_LOGW(TAG, "burn_in checkpoint load failed: %s",
                     esp_err_to_name(checkpoint_load_err));
        }
    } else {
        ESP_LOGW(TAG, "burn_in checkpoint disabled: %s", esp_err_to_name(checkpoint_init_err));
    }

    uint16_t sensor_pid = 0;
    const esp_err_t camera_err = catcam_camera_start(&sensor_pid);
    if (camera_err == ESP_OK) {
        ESP_LOGI(TAG, "camera_burn_in started sensor_pid=0x%04X frame=SVGA jpeg", sensor_pid);
    } else {
        ESP_LOGE(TAG, "camera_burn_in start failed: %s", esp_err_to_name(camera_err));
        return;
    }

    const uint64_t started_at_us = esp_timer_get_time();
    catcam_burn_in_schedule_t burn_in_schedule;
    catcam_burn_in_schedule_init(&burn_in_schedule, started_at_us);
    catcam_camera_stability_stats_t stats;
    catcam_camera_stability_init(&stats, started_at_us);
    catcam_burn_in_record_t burn_in_record;
    catcam_burn_in_record_init(&burn_in_record,
                               esp_get_free_heap_size(),
                               heap_caps_get_largest_free_block(MALLOC_CAP_8BIT),
                               heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    while (true) {
        catcam_camera_probe_result_t frame;
        const esp_err_t capture_err = catcam_camera_capture_frame(&frame);
        const uint64_t now_us = esp_timer_get_time();
        const bool report_due = capture_err == ESP_OK
                                    ? catcam_camera_stability_record_frame(&stats, now_us, frame.jpeg_bytes)
                                    : catcam_camera_stability_record_failure(&stats, now_us);

        if (report_due) {
            const uint64_t elapsed_us = now_us - started_at_us;
            const float average_fps = elapsed_us == 0 ? 0.0F :
                (float) stats.frames * 1000000.0F / (float) elapsed_us;
            const uint32_t heap_bytes = esp_get_free_heap_size();
            const uint32_t largest_8bit_bytes =
                heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
            const uint32_t psram_free_bytes =
                heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
            catcam_burn_in_record_update(&burn_in_record, stats.frames, stats.capture_failures,
                                         stats.jpeg_bytes, elapsed_us / 1000000ULL, heap_bytes,
                                         largest_8bit_bytes, psram_free_bytes);
            if (catcam_burn_in_schedule_checkpoint_due(&burn_in_schedule, now_us)) {
                const esp_err_t checkpoint_save_err =
                    catcam_burn_in_checkpoint_save(&burn_in_record);
                if (checkpoint_save_err == ESP_OK) {
                    ESP_LOGI(TAG,
                             "camera_burn_in elapsed_s=%u frames=%u failures=%u avg_fps=%.2f "
                             "avg_jpeg=%u heap=%u largest_8bit=%u psram_free=%u",
                             (unsigned int) (elapsed_us / 1000000ULL), stats.frames,
                             stats.capture_failures, average_fps,
                             stats.frames == 0 ? 0U : (unsigned int) (stats.jpeg_bytes / stats.frames),
                             (unsigned int) heap_bytes, (unsigned int) largest_8bit_bytes,
                             (unsigned int) psram_free_bytes);
                } else {
                    ESP_LOGW(TAG, "burn_in checkpoint save failed: %s",
                             esp_err_to_name(checkpoint_save_err));
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
