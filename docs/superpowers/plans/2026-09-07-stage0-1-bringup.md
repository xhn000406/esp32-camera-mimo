# ESP32-S3 Cat Camera Stage 0–1 Bring-up Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build and flash an ESP-IDF startup diagnostic for the confirmed ESP32-S3 N16R8 board over native USB.

**Architecture:** One ESP-IDF `main` component owns startup diagnostics. A small hardware-profile header carries reference-only board mappings so no unverified camera, TF, or audio GPIO leaks into application logic. The firmware writes a stable 115200 boot report to the USB Serial/JTAG console.

**Tech Stack:** ESP-IDF 5.5.1, C, ESP32-S3, Unity host-free component test support.

**Spec:** `docs/superpowers/specs/2026-09-07-cat-camera-stage0-1-design.md`

## Global Constraints

- Target exactly `esp32s3`.
- Use native USB Serial/JTAG at 115200; do not require a CH304 driver.
- Do not initialize camera, SDMMC/TF, Wi-Fi, I2S, or audio in this task.
- Treat camera and TF GPIO mappings as reference-only until physical functional validation.
- Use `/dev/cu.usbmodem101` for this currently connected board.

---

### Task 1: Create the project contract and hardware profile

**Files:**
- Create: `CMakeLists.txt`
- Create: `main/CMakeLists.txt`
- Create: `main/include/hardware_profile.h`
- Create: `main/hardware_profile.c`
- Test: `tests/host/test_hardware_profile.c`

**Interfaces:**
- Produces: `catcam_hardware_profile_t` with `flash_mb`, `psram_mb`, TF pins, and camera-map status.
- Consumes: ESP-IDF CMake and Unity test component.

- [ ] **Step 1: Write the failing test**

```c
TEST_CASE("hardware profile preserves N16R8 storage facts", "[hardware_profile]")
{
    const catcam_hardware_profile_t profile = catcam_hardware_profile();
    TEST_ASSERT_EQUAL_UINT8(16, profile.flash_mb);
    TEST_ASSERT_EQUAL_UINT8(8, profile.psram_mb);
    TEST_ASSERT_EQUAL_INT(38, profile.tf_cmd_gpio);
    TEST_ASSERT_EQUAL_INT(39, profile.tf_clk_gpio);
    TEST_ASSERT_EQUAL_INT(40, profile.tf_dat0_gpio);
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `clang -std=c11 -Wall -Wextra -Werror -I main/include main/hardware_profile.c tests/host/test_hardware_profile.c -o work/test_hardware_profile && work/test_hardware_profile`

Expected: FAIL because the profile header and implementation do not exist.

- [ ] **Step 3: Write minimal implementation**

```c
typedef struct {
    uint8_t flash_mb;
    uint8_t psram_mb;
    int tf_cmd_gpio;
    int tf_clk_gpio;
    int tf_dat0_gpio;
} catcam_hardware_profile_t;

static inline catcam_hardware_profile_t catcam_hardware_profile(void) {
    return (catcam_hardware_profile_t){16, 8, 38, 39, 40};
}
```

- [ ] **Step 4: Run test to verify it passes**

Run: `clang -std=c11 -Wall -Wextra -Werror -I main/include main/hardware_profile.c tests/host/test_hardware_profile.c -o work/test_hardware_profile && work/test_hardware_profile`

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add CMakeLists.txt main/CMakeLists.txt main/include/hardware_profile.h main/test/test_hardware_profile.c
git commit -m "feat: add validated board hardware profile"
```

### Task 2: Add the startup diagnostic application

**Files:**
- Create: `main/boot_report.c`
- Create: `main/include/boot_report.h`
- Create: `main/main.c`
- Modify: `main/CMakeLists.txt`
- Test: `tests/host/test_boot_report.c`

**Interfaces:**
- Consumes: `catcam_hardware_profile()`.
- Produces: `catcam_format_boot_report(const catcam_boot_facts_t *facts, char *buffer, size_t size)` containing build ID, reset reason, Flash MB, PSRAM MB, and MAC label.

- [ ] **Step 1: Write the failing test**

```c
TEST_CASE("boot report includes all operating facts", "[boot_report]")
{
    char buffer[256];
    const catcam_boot_facts_t facts = {
        .build_id = "stage1",
        .reset_reason = "power-on",
        .flash_mb = 16,
        .psram_mb = 8,
        .mac = "28:84:85:4A:2F:E8",
    };
    catcam_format_boot_report(&facts, buffer, sizeof(buffer));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "flash_mb="));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "psram_mb="));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "reset_reason="));
    TEST_ASSERT_NOT_NULL(strstr(buffer, "mac="));
}
```

- [ ] **Step 2: Run test to verify it fails**

Run: `clang -std=c11 -Wall -Wextra -Werror -I main/include main/boot_report.c tests/host/test_boot_report.c -o work/test_boot_report && work/test_boot_report`

Expected: FAIL because `boot_report.h` and `catcam_format_boot_report` do not exist.

- [ ] **Step 3: Write minimal implementation**

```c
int written = snprintf(buffer, size,
    "build_id=%s reset_reason=%s flash_mb=%u psram_mb=%u mac=%s",
    facts->build_id, facts->reset_reason, facts->flash_mb,
    facts->psram_mb, facts->mac);
```

In `main.c`, use `esp_chip_info`, `esp_flash_get_size`, `esp_psram_get_size`, `esp_reset_reason`, and `esp_read_mac` to fill `catcam_boot_facts_t`, then log the formatted result. Do not call camera, SDMMC, Wi-Fi, or I2S APIs.

- [ ] **Step 4: Run test to verify it passes**

Run: `clang -std=c11 -Wall -Wextra -Werror -I main/include main/boot_report.c tests/host/test_boot_report.c -o work/test_boot_report && work/test_boot_report`

Expected: PASS.

- [ ] **Step 5: Commit**

```bash
git add main/main.c main/CMakeLists.txt main/test/test_boot_report.c
git commit -m "feat: add ESP32-S3 boot diagnostics"
```

### Task 3: Build, flash, and record the physical bring-up evidence

**Files:**
- Create: `docs/bringup/2026-09-07-stage1-report.md`

**Interfaces:**
- Consumes: built `build/catcam-stage1.bin` and `/dev/cu.usbmodem101`.
- Produces: a report with build ID, upload result, first boot output, Flash, PSRAM, MAC, reset reason, and remaining unknowns.

- [ ] **Step 1: Build target firmware**

Run: `source .tools/esp-idf-v5.5.1-clean/export.sh && idf.py set-target esp32s3 && idf.py build`

Expected: zero compiler and linker errors.

- [ ] **Step 2: Flash through native USB**

Run: `source .tools/esp-idf-v5.5.1-clean/export.sh && idf.py -p /dev/cu.usbmodem101 flash`

Expected: esptool verifies the written image and resets the board.

- [ ] **Step 3: Capture boot output**

Run: `source .tools/esp-idf-v5.5.1-clean/export.sh && idf.py -p /dev/cu.usbmodem101 monitor`

Expected: one report containing the required resource and reset fields at 115200.

- [ ] **Step 4: Write the evidence record**

Record exact command output and whether camera/TF APIs were absent from the firmware.

- [ ] **Step 5: Commit**

```bash
git add docs/bringup/2026-09-07-stage1-report.md
git commit -m "docs: record stage 1 board bring-up"
```
