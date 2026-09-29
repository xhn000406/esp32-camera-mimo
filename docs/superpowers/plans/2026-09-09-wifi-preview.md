# Wi-Fi Preview Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Let a phone connect to the device SoftAP and view an OV5640 MJPEG stream at `http://192.168.4.1/`.

**Architecture:** A `wifi_preview` component owns SoftAP, HTTP server and one-client admission. The HTTP stream handler owns each frame only between `catcam_camera_capture_frame()` and `esp_camera_fb_return()`; a slow or second client never blocks the camera task indefinitely.

**Tech Stack:** ESP-IDF 5.5.1, `esp_wifi`, `esp_http_server`, existing `esp32-camera` OV5640 wrapper.

**Spec:** `docs/superpowers/specs/2026-09-09-wifi-preview-design.md`

## Global Constraints

- Retain SVGA JPEG and the verified OV5640 pin map.
- Do not add TF, AI, audio or family-router credentials in this phase.
- SoftAP password comes from `sdkconfig`, never source code.
- A stream handler must return every acquired frame before returning.
- Serve at most one MJPEG client; later clients receive HTTP 503.

---

### Task 1: Preview admission state

**Files:**
- Create: `main/include/wifi_preview.h`
- Create: `main/wifi_preview.c`
- Test: `tests/host/test_wifi_preview.c`

**Interfaces:**
- Produces: `bool catcam_wifi_preview_try_acquire(void)` and `void catcam_wifi_preview_release(void)`.

- [ ] **Step 1: Write the failing test**

```c
assert(catcam_wifi_preview_try_acquire());
assert(!catcam_wifi_preview_try_acquire());
catcam_wifi_preview_release();
assert(catcam_wifi_preview_try_acquire());
```

- [ ] **Step 2: Run the host test and verify it fails because the module is absent.**
- [ ] **Step 3: Implement the one-client flag with atomic compare-and-set.**
- [ ] **Step 4: Run the host test and verify it passes.**

### Task 2: SoftAP and HTTP service

**Files:**
- Modify: `main/wifi_preview.c`
- Modify: `main/CMakeLists.txt`
- Modify: `sdkconfig.defaults`

**Interfaces:**
- Consumes: `esp_err_t catcam_camera_capture_frame(catcam_camera_probe_result_t *)` and `esp_camera_fb_return()`.
- Produces: `esp_err_t catcam_wifi_preview_start(void)`.

- [ ] **Step 1: Add `esp_wifi`, `esp_event`, `esp_netif`, `esp_http_server` dependencies and SoftAP `CatCam-Setup` configuration in sdkconfig.**
- [ ] **Step 2: Start the default event loop, AP netif and Wi-Fi AP; log the AP IP.**
- [ ] **Step 3: Register `/` HTML handler and `/stream` multipart MJPEG handler.**
- [ ] **Step 4: In `/stream`, acquire admission, send each JPEG part with HTTP timeout, return its framebuffer, then release admission. Return 503 if already acquired.**
- [ ] **Step 5: Build and flash; verify AP start and the root page with serial logs.**

### Task 3: Camera application integration and acceptance evidence

**Files:**
- Modify: `main/main.c`
- Test: `tests/host/test_wifi_preview.c`

**Interfaces:**
- Consumes: `esp_err_t catcam_wifi_preview_start(void)`.

- [ ] **Step 1: Start Wi-Fi preview only after successful SVGA camera initialization.**
- [ ] **Step 2: Build all host tests and firmware.**
- [ ] **Step 3: Flash the board, connect a phone to `CatCam-Setup`, and load `http://192.168.4.1/`.**
- [ ] **Step 4: Record 10-minute stability and 20 reconnect attempts before declaring this phase complete.**
