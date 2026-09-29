#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "boot_report.h"

int main(void)
{
    const catcam_boot_facts_t facts = {
        .build_id = "stage1",
        .reset_reason = "power-on",
        .flash_mb = 16,
        .psram_mb = 8,
        .mac = "28:84:85:4A:2F:E8",
    };
    char report[256];

    const int written = catcam_format_boot_report(&facts, report, sizeof(report));

    assert(written > 0);
    assert(strstr(report, "build_id=stage1") != NULL);
    assert(strstr(report, "reset_reason=power-on") != NULL);
    assert(strstr(report, "flash_mb=16") != NULL);
    assert(strstr(report, "psram_mb=8") != NULL);
    assert(strstr(report, "mac=28:84:85:4A:2F:E8") != NULL);

    puts("boot report test passed");
    return 0;
}
