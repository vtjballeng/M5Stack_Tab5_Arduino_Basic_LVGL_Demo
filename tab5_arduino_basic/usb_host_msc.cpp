/*
USB Host Mass Storage Class Implementation
Uses ESP-IDF USB Host library with Arduino wrapper
*/

#include "usb_host_msc.h"
#include <FS.h>
#include <FFat.h>
#include <SD.h>
#include <esp_vfs_fat.h>
#include <dirent.h>
#include <sys/stat.h>

// ESP-IDF includes for USB Host
extern "C" {
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <usb/usb_host.h>
}

#define TAG         "USB_HOST_MSC"
#define MOUNT_POINT "/usb"

static USBHostMSC *instance = nullptr;
static usb_host_client_handle_t client_hdl = NULL;
static uint8_t dev_addr = 0;
static bool usb_drive_connected = false;

// USB Host event handler
static void usb_host_event_handler(void *arg) {
    while (1) {
        uint32_t event_flags;
        usb_host_lib_handle_events(portMAX_DELAY, &event_flags);

        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
            ESP_LOGI(TAG, "All clients deregistered");
            break;
        }
        if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
            ESP_LOGI(TAG, "All devices freed");
            break;
        }
    }
    vTaskDelete(NULL);
}

// Client event callback
static void client_event_cb(const usb_host_client_event_msg_t *event_msg, void *arg) {
    if (event_msg->event == USB_HOST_CLIENT_EVENT_NEW_DEV) {
        ESP_LOGI(TAG, "New USB device connected");
        dev_addr = event_msg->new_dev.address;
        usb_drive_connected = true;

        if (instance && instance->onDriveConnected) {
            USBDriveInfo info = instance->getDriveInfo();
            instance->onDriveConnected(info);
        }
    } else if (event_msg->event == USB_HOST_CLIENT_EVENT_DEV_GONE) {
        ESP_LOGI(TAG, "USB device disconnected");
        usb_drive_connected = false;
        dev_addr = 0;

        if (instance && instance->onDriveDisconnected) {
            instance->onDriveDisconnected();
        }
    }
}

USBHostMSC::USBHostMSC()
    : initialized(false),
      drive_mounted(false),
      mount_point(MOUNT_POINT),
      onDriveConnected(nullptr),
      onDriveDisconnected(nullptr),
      current_drive() {
    instance = this;
}

USBHostMSC::~USBHostMSC() {
    end();
    instance = nullptr;
}

bool USBHostMSC::begin() {
    if (initialized)
        return true;

    ESP_LOGI(TAG, "Installing USB Host Library");

    // Configure USB Host
    usb_host_config_t host_config = {
        .skip_phy_setup = false,
        .intr_flags = ESP_INTR_FLAG_LEVEL1,
    };

    esp_err_t err = usb_host_install(&host_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install USB Host Library: %s", esp_err_to_name(err));
        return false;
    }

    // Register client
    usb_host_client_config_t client_config = {
        .is_synchronous = false,
        .max_num_event_msg = 5,
        .async =
            {
                .client_event_callback = client_event_cb,
                .callback_arg = this,
            },
    };

    err = usb_host_client_register(&client_config, &client_hdl);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register USB Host client: %s", esp_err_to_name(err));
        usb_host_uninstall();
        return false;
    }

    // Create USB Host event handler task
    xTaskCreatePinnedToCore(usb_host_event_handler, "usb_host_event", 4096, NULL, 2, NULL, 0);

    initialized = true;
    ESP_LOGI(TAG, "USB Host MSC initialized");
    return true;
}

void USBHostMSC::end() {
    if (!initialized)
        return;

    if (client_hdl) {
        usb_host_client_deregister(client_hdl);
        client_hdl = NULL;
    }

    usb_host_uninstall();
    initialized = false;
    drive_mounted = false;
    ESP_LOGI(TAG, "USB Host MSC ended");
}

bool USBHostMSC::isDriveConnected() {
    return usb_drive_connected && drive_mounted;
}

USBDriveInfo USBHostMSC::getDriveInfo() {
    USBDriveInfo info = {};
    info.connected = usb_drive_connected;
    info.capacity_mb = 0;
    info.free_space_mb = 0;
    info.sector_size = 0;

    if (usb_drive_connected && dev_addr != 0) {
        // Get device descriptor
        usb_device_handle_t dev_hdl;
        if (usb_host_device_open(client_hdl, dev_addr, &dev_hdl) == ESP_OK) {
            const usb_device_desc_t *dev_desc;
            if (usb_host_get_device_descriptor(dev_hdl, &dev_desc) == ESP_OK) {
                // Get vendor and product strings
                // Note: String descriptor functions may not be available in Arduino ESP32 core
                // For now, use placeholder values
                info.vendor = "USB Device";
                info.product = "Mass Storage";
                info.serial = "123456";
            }
            usb_host_device_close(client_hdl, dev_hdl);
        }

        // Get storage capacity (simplified - would need MSC class driver for actual values)
        info.capacity_mb = 8192;    // Default 8GB
        info.free_space_mb = 4096;  // Default 4GB free
        info.sector_size = 512;
    }

    current_drive = info;
    return info;
}

std::vector<USBFileEntry> USBHostMSC::listDirectory(const String &path) {
    std::vector<USBFileEntry> entries;

    if (!isDriveConnected()) {
        return entries;
    }

    String full_path = mount_point + path;
    DIR *dir = opendir(full_path.c_str());

    if (dir != NULL) {
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL) {
            USBFileEntry entry = {};
            entry.name = String(ent->d_name);
            entry.is_directory = false;
            entry.size = 0;

            // Skip . and ..
            if (entry.name == "." || entry.name == "..") {
                continue;
            }

            // Get file info
            String file_path = full_path + "/" + entry.name;
            struct stat st;
            if (stat(file_path.c_str(), &st) == 0) {
                entry.is_directory = S_ISDIR(st.st_mode);
                entry.size = st.st_size;

                // Format date
                struct tm *timeinfo = localtime(&st.st_mtime);
                char date_buf[32];
                strftime(date_buf, sizeof(date_buf), "%Y-%m-%d %H:%M", timeinfo);
                entry.date_modified = String(date_buf);
            }

            entries.push_back(entry);
        }
        closedir(dir);
    }

    return entries;
}

String USBHostMSC::readFile(const String &path) {
    if (!isDriveConnected()) {
        return "";
    }

    String full_path = mount_point + path;
    File file = SD.open(full_path, FILE_READ);

    if (!file) {
        return "";
    }

    String content = "";
    while (file.available()) {
        content += (char) file.read();
    }

    file.close();
    return content;
}

bool USBHostMSC::fileExists(const String &path) {
    if (!isDriveConnected()) {
        return false;
    }

    String full_path = mount_point + path;
    struct stat st;
    return (stat(full_path.c_str(), &st) == 0);
}

uint32_t USBHostMSC::getFileSize(const String &path) {
    if (!isDriveConnected()) {
        return 0;
    }

    String full_path = mount_point + path;
    struct stat st;
    if (stat(full_path.c_str(), &st) == 0) {
        return st.st_size;
    }

    return 0;
}

void USBHostMSC::setOnDriveConnected(void (*callback)(USBDriveInfo)) {
    onDriveConnected = callback;
}

void USBHostMSC::setOnDriveDisconnected(void (*callback)()) {
    onDriveDisconnected = callback;
}

void USBHostMSC::update() {
    if (!initialized)
        return;

    // Process USB Host client events
    if (client_hdl) {
        usb_host_client_handle_events(client_hdl, 0);
    }

    // Check if we need to mount the drive
    if (usb_drive_connected && !drive_mounted) {
        // Attempt to mount (simplified - actual implementation would use MSC driver)
        ESP_LOGI(TAG, "Attempting to mount USB drive at %s", mount_point.c_str());
        drive_mounted = true;  // Assume success for now
    } else if (!usb_drive_connected && drive_mounted) {
        ESP_LOGI(TAG, "Unmounting USB drive");
        drive_mounted = false;
    }
}