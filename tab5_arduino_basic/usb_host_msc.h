/*
USB Host Mass Storage Class Support for M5Stack Tab5
Provides USB drive detection and file reading capabilities
*/

#ifndef USB_HOST_MSC_H
#define USB_HOST_MSC_H

#include <Arduino.h>
#include <vector>
#include <string>

// USB Drive Info Structure
struct USBDriveInfo {
    bool connected;
    uint32_t capacity_mb;
    uint32_t free_space_mb;
    uint32_t sector_size;
    String vendor;
    String product;
    String serial;
};

// File/Directory Entry
struct USBFileEntry {
    String name;
    bool is_directory;
    uint32_t size;
    String date_modified;
};

class USBHostMSC {
   private:
    bool initialized;
    bool drive_mounted;
    USBDriveInfo current_drive;
    String mount_point;

   public:
    // Callbacks
    void (*onDriveConnected)(USBDriveInfo info);
    void (*onDriveDisconnected)();
    USBHostMSC();
    ~USBHostMSC();

    // Initialize USB Host
    bool begin();
    void end();

    // Drive Management
    bool isDriveConnected();
    USBDriveInfo getDriveInfo();

    // File Operations
    std::vector<USBFileEntry> listDirectory(const String& path);
    String readFile(const String& path);
    bool fileExists(const String& path);
    uint32_t getFileSize(const String& path);

    // Callbacks
    void setOnDriveConnected(void (*callback)(USBDriveInfo));
    void setOnDriveDisconnected(void (*callback)());

    // Update - call in loop
    void update();
};

#endif  // USB_HOST_MSC_H