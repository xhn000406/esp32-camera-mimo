idf_component_register(
    SRCS "main.c" "boot_report.c" "burn_in_checkpoint.c" "burn_in_record.c" "burn_in_schedule.c" "camera_probe.c" "camera_stability.c" "hardware_profile.c"
    INCLUDE_DIRS "include"
    REQUIRES esp32-camera esp_hw_support esp_psram esp_system esp_timer nvs_flash spi_flash
)
