# USB Drive Reading Feature for M5Stack Tab5

This implementation adds USB drive reading capabilities to the M5Stack Tab5 Arduino project.

## Features

- **USB Drive Detection**: Automatically detects when a USB drive is connected or disconnected
- **File Browser**: Browse directories and files on the USB drive
- **Drive Information**: Display drive capacity, vendor, and product information
- **LVGL UI Integration**: Seamless integration with the existing LVGL interface

## Implementation Details

### Files Added

1. **usb_host_msc.h/cpp**: USB Host Mass Storage Class wrapper
   - Provides Arduino-friendly interface to ESP-IDF USB Host library
   - Handles drive detection, mounting, and file operations

2. **ui_usb.h/c**: LVGL UI components for USB functionality
   - USB status display
   - File browser list
   - Navigation controls

### UI Components

The USB panel includes:
- Status indicator (Connected/Not Connected)
- Drive information display (vendor, product, capacity)
- File browser with directory navigation
- Back and Refresh buttons
- Current path display

## Usage

1. Connect a FAT-formatted USB drive to the Tab5's USB Type-A port
2. The UI will automatically detect the drive and display its contents
3. Click on directories to navigate into them
4. Use the Back button to go to parent directory
5. Use Refresh to update the file list

## Current Limitations

1. **ESP32-P4 Arduino Support**: The ESP32-P4 USB Host support in Arduino is still experimental. The implementation uses a hybrid approach with ESP-IDF components.

2. **File System**: Currently supports FAT-formatted drives only

3. **Read-Only**: The current implementation is read-only. Write operations are not yet implemented.

4. **MSC Driver**: The full USB MSC driver integration requires additional ESP-IDF components that may need to be installed separately.

## Building

Ensure you have:
- ESP32 Arduino Core v3.2.0 or later
- LVGL 8.3.11
- M5GFX and M5Unified libraries

The USB Host library components are included with the ESP32 core but may require additional configuration for full functionality.

## Future Enhancements

- Add file preview functionality
- Implement write operations
- Support for multiple file systems (exFAT, NTFS)
- File transfer operations
- USB drive formatting utilities