#include "burn_in_checkpoint.h"

#include "nvs.h"
#include "nvs_flash.h"

#define CATCAM_BURN_IN_NAMESPACE "burn_in"
#define CATCAM_BURN_IN_RECORD_KEY "latest"

esp_err_t catcam_burn_in_checkpoint_init(void)
{
    return nvs_flash_init();
}

esp_err_t catcam_burn_in_checkpoint_load(catcam_burn_in_record_t *record, bool *found)
{
    nvs_handle_t handle;
    size_t length = sizeof(*record);
    esp_err_t err = nvs_open(CATCAM_BURN_IN_NAMESPACE, NVS_READONLY, &handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *found = false;
        return ESP_OK;
    }
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_get_blob(handle, CATCAM_BURN_IN_RECORD_KEY, record, &length);
    nvs_close(handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        *found = false;
        return ESP_OK;
    }
    if (err == ESP_OK && length != sizeof(*record)) {
        return ESP_ERR_INVALID_SIZE;
    }
    *found = err == ESP_OK;
    return err;
}

esp_err_t catcam_burn_in_checkpoint_save(const catcam_burn_in_record_t *record)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(CATCAM_BURN_IN_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }

    err = nvs_set_blob(handle, CATCAM_BURN_IN_RECORD_KEY, record, sizeof(*record));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}
