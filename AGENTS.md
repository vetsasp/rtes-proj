# Project Notes

## Build & Upload
- Build: `pio run`
- Upload: `pio run -t upload`
- Target: STM32 B-L475E-IOT01A (disco_l475vg_iot01a)

## Upload Troubleshooting
- ST-LINK must be visible: `lsusb` shows `0483:374b`
- If "init mode failed" error: press RESET on board, power cycle, or replug USB
- Device requires udev rules or root access to access `/dev/bus/usb/*`

## mbed-Specific Notes
- Default parameters must be in `.h` declaration, not `.cpp` definition
- Objects with hardware timers (Ticker, etc.) should be `static` or heap-allocated to persist beyond function scope
- Global/static local recommended for single-instance hardware objects

## Key Files
- `src/main.cpp`: Entry point
- `src/led.cpp` / `led.h`: LED control
- `src/button.cpp` / `button.h`: Button input
- `src/state.cpp` / `state.h`: State management