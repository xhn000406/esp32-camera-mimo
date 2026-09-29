#pragma once

#include <stdbool.h>

#include "burn_in_record.h"
#include "esp_err.h"

esp_err_t catcam_burn_in_checkpoint_init(void);
esp_err_t catcam_burn_in_checkpoint_load(catcam_burn_in_record_t *record, bool *found);
esp_err_t catcam_burn_in_checkpoint_save(const catcam_burn_in_record_t *record);
