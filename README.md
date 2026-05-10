# Gesture Lock

> https://github.com/vetsasp/rtes-proj

Gesture-based lock using the on-board accelerometer and gyroscope. You record a Personal Identification Gesture (PIG) as a sequence of gestures, then enter it to toggle `LED1`. Designed for the *STM32  B-L475E-IOT01A*, making use of the *PlatformIO*/*MBED* framework.

## Build / Upload

- Build: `pio run`
- Clean build: `pio run -t clean && pio run`
- Upload: `pio run -t upload`
- Monitor: `pio device monitor`

Target environment is `disco_l475vg_iot01a` (see `platformio.ini`).

## Usage

- Double press the user button to enter **SET** mode and record a new PIG.
- Single press enters **INPUT** mode only if a PIG has been set.

Gesture segmentation:

- Stillness is not a gesture.
- A gesture starts when the device is in motion.
- A gesture ends when the device becomes still again.

Timeout:

- In SET/INPUT mode, 30s of inactivity (still + not in-gesture) returns to IDLE.

## LEDs (UX)

- `LED1` (output): toggles on successful unlock.
- `LED2` (interface):
  - slow blink while recording in SET/INPUT
  - alert flashes when something happens (set done, unlock/deny, timeout)
  - between gestures: a short alert plays

## Matching

- After an input attempt is recorded, the attempt is compared against the saved PIG using DTW (Sakoe-Chiba band + length ratio gate).

## Configuration / Tuning

Most knobs live in `src/config.h`:

- `GESTURE_COUNT`
- stillness thresholds and segmentation windows (`GYRO_STILL_DPS`, `ACCEL_STILL_G_TOL`, `STILL_HOLD_SAMPLES`, ...)
- timeout (`MODE_INACTIVITY_TIMEOUT_MS`)
- DTW gates / window / threshold (`DTW_*`)
- UX blink counts (`AFFIRMATIVE_BLINK_COUNT`, `NEGATIVE_BLINK_COUNT`, `INTER_GESTURE_BLINK_COUNT`)
