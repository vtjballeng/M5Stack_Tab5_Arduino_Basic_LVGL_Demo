# LVGL Implementation for M5Stack Tab5

This project uses LVGL v8.3.11 with a project-level configuration that keeps
everything self-contained without modifying the Arduino library.

## The Single Configuration File

### `lv_conf.h`

This is the ONLY configuration file needed! Located in the project directory,
it contains all LVGL settings optimized for the M5Stack Tab5:

- **Color depth**: 16-bit (RGB565) with byte swap enabled for Tab5's display
- **Memory**: Custom allocation using standard malloc/free for better ESP32
  integration
- **Tick source**: Arduino's millis() for accurate timing
- **Display**: Optimized for 720x1280 with software rotation to landscape
- **Touch**: Full multi-touch support configured

## Building with Arduino IDE

1. **Simply open and compile the sketch**
   - No library modifications needed
   - No files to copy
   - Everything is self-contained in the project

The main sketch defines `LV_CONF_INCLUDE_SIMPLE` before including LVGL, which
tells LVGL to look for `lv_conf.h` in the sketch folder. Simple and clean!

## Building with PlatformIO

1. Use the provided `platformio.ini` file
2. Run: `pio run -e esp32p4`

## Key Configuration Changes

### Memory Management

- Uses standard malloc/free instead of LVGL's built-in memory manager
- Better integration with ESP32's memory management
- Utilizes PSRAM when available

### Display Settings

- 16-bit color depth (RGB565)
- Byte swapping enabled for correct colors on Tab5's display
- Software rotation configured for landscape mode

### Performance Optimizations

- Custom tick source using Arduino millis()
- Optimized buffer sizes for Tab5's display resolution
- Reduced memory footprint for embedded system

## Why This Approach?

1. **Zero Setup**: Clone and compile - no manual steps required
2. **Self-Contained**: All configuration stays with the project
3. **No Library Modifications**: Arduino LVGL library remains untouched
4. **Version Control Friendly**: Configuration is part of the project
5. **Multiple Projects**: Each project can have different LVGL settings

## How It Works

Elegantly simple - just one line of code:

```cpp
#define LV_CONF_INCLUDE_SIMPLE 1  // Tell LVGL to use our config
#include <lvgl.h>                  // Include LVGL normally
```

That's it! LVGL finds `lv_conf.h` in the sketch folder and uses it instead of
looking for library-level configuration.

## Troubleshooting

If you encounter compilation errors:

1. **"lv_conf.h not found"**: Make sure the file is in the sketch folder
2. **Color issues**: Verify LV_COLOR_16_SWAP is set to 1 in lv_conf.h  
3. **Memory errors**: Check that LV_MEM_CUSTOM is set to 1 in lv_conf.h
4. **LVGL version mismatch**: Ensure you have LVGL v8.3.11 installed
