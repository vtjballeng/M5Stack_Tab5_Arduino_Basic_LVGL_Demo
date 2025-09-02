# M5Stack Tab5 Arduino LVGL Demo with USB Drive Support

This project demonstrates LVGL GUI framework on the M5Stack Tab5 with
ESP32-P4, including USB drive reading capabilities and project-level LVGL
configuration.

## Features

- **LVGL GUI**: Full touchscreen interface with modern UI elements
- **USB Drive Support**: Read and browse USB drives connected to the Tab5's
  USB Type-A port
- **Clock & Alarm**: Digital clock with alarm functionality and snooze
- **Project-Level Configuration**: Self-contained LVGL configuration that
  doesn't affect other projects
- **Display Controls**: Brightness adjustment and demo animations

## Key Updates

### USB Drive Reading

- Automatic detection of USB drives plugged into the Tab5
- File browser with directory navigation
- Display of drive information (vendor, capacity)
- Real-time connection/disconnection handling

### Project-Level LVGL Configuration

- **lv_conf.h** included in project directory
- No need to modify LVGL library files
- Portable and version-control friendly
- Each project can have its own LVGL settings

## Quick Start

1. **Open the sketch** in Arduino IDE

2. **Configure Board Settings**:
   - Board: ESP32P4 Dev Module
   - Flash Size: 16MB
   - Partition Scheme: Custom (use provided partitions.csv)
   - PSRAM: Enabled

3. **Compile and Upload**

## Dependencies

- ESP32 Arduino Core >= 3.2.0
- M5GFX >= 0.2.8
- M5Unified >= 0.2.0
- LVGL == 8.3.11

## Documentation

- [USB Feature Documentation](tab5_arduino_basic/README_USB.md)
- [LVGL Implementation Guide](tab5_arduino_basic/README_LVGL.md)
- See notes at the top of the .ino file for build details

## Original Credits

Original basic demo by nikthefix with display and touch features.
Extended with USB support and project-level LVGL configuration
