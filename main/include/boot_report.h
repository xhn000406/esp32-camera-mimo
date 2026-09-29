#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    const char *build_id;
    const char *reset_reason;
    uint32_t flash_mb;
    uint32_t psram_mb;
    const char *mac;
} catcam_boot_facts_t;

int catcam_format_boot_report(const catcam_boot_facts_t *facts, char *buffer, size_t size);
