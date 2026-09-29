#include "boot_report.h"

#include <stdio.h>

int catcam_format_boot_report(const catcam_boot_facts_t *facts, char *buffer, size_t size)
{
    if (facts == NULL || buffer == NULL || size == 0) {
        return -1;
    }

    return snprintf(buffer, size,
                    "build_id=%s reset_reason=%s flash_mb=%u psram_mb=%u mac=%s",
                    facts->build_id, facts->reset_reason, (unsigned int)facts->flash_mb,
                    (unsigned int)facts->psram_mb, facts->mac);
}
