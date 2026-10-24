# Tasks

## GitHub issues (synced 2026-10-24)

Open issues in QWavey/ESP32-S3-Waveshare-OLED-Watch: #1 (audit, 30 bullets), #2 to #6 (single items). Several #1 bullets repeat #2 to #6 and are cross-referenced. Paths are relative to the repo root. Line numbers refer to the code as it was at the start of this pass.

Issue #1, security hygiene:

- [ ] #1 moveFile deletes source after a partial copy — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:452-455 — FIX — open
- [ ] #1 Password-mode text written to serial log — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:451-452 — FIX — open
- [ ] #1 truncateFile ignores failures and deletes original first — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:378-390 — FIX — open
- [ ] #1 format() reports success after failed deletions — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:110-127 — FIX — open
- [ ] #1 UIKeyboard keeps a raw pointer to its textbox — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:95, 436-457 — FIX — open
- [ ] #1 Unbounded memory and recursion driven by card contents — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:248-260, 517-580, 987-1016 — FIX — open
- [ ] #1 Path arguments are not confined and the raw FS is exposed — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:880-891; ESP_DISPLAY_TOUCH/SD/SDMounter.h:89 — FIX — open
- [ ] #1 RTC set from unauthenticated NTP without checks — examples/time.cpp:21, 80-89 — FIX — open
- [ ] #1 Credential literals in examples and no .gitignore — examples/time.cpp:19-20; repo root — FIX — open
- [ ] #1 Dependencies unpinned and README asks for latest — README.md:33, 48-57, 75, 80 — SKIP — skipped: improvement only; no defect shown, and the repo has no manifest to pin against

Issue #1, bugs:

- [ ] #1 moveFile deletes the source after a short copy — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:443, 453-454, 979-981 — FIX — open (same fix as the first moveFile item)
- [ ] #1 truncateFile removes the original before the replacement is confirmed — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:378, 386-390 — FIX — open (same fix as truncateFile above)
- [ ] #1 Hot-swap polling silently remounts the card after unmount() — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:796-801, 804-808, 917-934 — FIX — open (same fix as #3)
- [ ] #1 copyFile with identical source and destination truncates the source — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:428-432, 453 — FIX — open
- [ ] #1 Fade-down computes a uint8_t from a negative float — ESP_DISPLAY_TOUCH/BRIGHTNESS/AMOLEDBrightness.cpp:289-290 — FIX — open
- [ ] #1 Keyboard loses its text box link when re-spawned — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:397-405, 436-442 — FIX — open (same fix as #5)
- [ ] #1 onClick() registers another LVGL handler on every call — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:232-237, 239-244 — FIX — open
- [ ] #1 Screen on() does not redraw the UI after off() — ESP_DISPLAY_TOUCH/SCREEN_TOUCH/ESP32-S3-Screen-AMOLED-2.06.h:12-24 — FIX — open
- [ ] #1 Touch read path rejects edge touches and uses a wrong upper bound — ESP_DISPLAY_TOUCH/SCREEN_TOUCH/ESP32-S3-Touch-AMOLED-2.06.h:153 — FIX — open
- [ ] #1 mount() format fallback never runs and insert-time auto-mount ignores its flag — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:37-46, 839-858 — FIX — open (same as #4)

Issue #1, improvements:

- [ ] #1 truncateFile deletes the original before its write and rename are checked — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:372-390 — FIX — open (same fix as truncateFile above)
- [ ] #1 copyFile and moveFile destroy the source when source and destination are the same — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:428-455 — FIX — open (same fix as copyFile identical-path bug above)
- [ ] #1 Keyboard Enter prints the typed text to Serial, including password boxes — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:448-452 — FIX — open (same fix as password-mode item above)
- [ ] #1 Brightness state is committed only when a fade ends — ESP_DISPLAY_TOUCH/BRIGHTNESS/AMOLEDBrightness.cpp:92-105, 145, 259, 272-293 — FIX — open
- [ ] #1 onClick adds a new event registration on every call — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:215-237 — FIX — open (same fix as onClick bug above)
- [ ] #1 Code calls LVGL 8 API names while the README requires LVGL 9 — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:120, 137, 157, 218; ESP_DISPLAY_TOUCH/BATTERY/BatteryDesign.cpp:148, 234, 283; ESP_DISPLAY_TOUCH/BRIGHTNESS/AMOLEDBrightness.cpp:18 — FIX — open (same as #2)
- [ ] #1 Hot-swap polling re-initializes the SD controller every 200 ms without a card — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:804-815, 917-934, 796-802 — FIX — open (same as #3)
- [ ] #1 mount() and format() ignore flags and report success after failures — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:37-46, 85-128, 169 — FIX — open (same as #4)
- [ ] #1 Keyboard link is lost when linkTo() runs before spawn() or spawn() runs again — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:397-442 — FIX — open (same as #5)
- [ ] #1 SimpleUI.cpp repeats every class declaration from SimpleUI.h — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:1-113 — FIX — open (same as #6)

Issues #2 to #6 (each issue is one item):

- [ ] #2 Code calls LVGL 8 API names while the README requires LVGL 9 — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:120, 137, 157, 218; ESP_DISPLAY_TOUCH/BATTERY/BatteryDesign.cpp:148, 234, 283; ESP_DISPLAY_TOUCH/BRIGHTNESS/AMOLEDBrightness.cpp:18 — FIX — open
- [ ] #3 Hot-swap polling re-initializes the SD controller every 200 ms without a card — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:804-815, 917-934, 796-802 — FIX — open
- [ ] #4 mount() and format() ignore flags and report success after failures — ESP_DISPLAY_TOUCH/SD/SDMounter.cpp:37-46, 85-128, 169 — FIX — open
- [ ] #5 Keyboard link is lost when linkTo() runs before spawn() or spawn() runs again — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:397-442 — FIX — open
- [ ] #6 SimpleUI.cpp repeats every class declaration from SimpleUI.h — ESP_DISPLAY_TOUCH/UI/SimpleUI.cpp:1-113; ESP_DISPLAY_TOUCH/UI/SimpleUI.h:1-115 — FIX — open
