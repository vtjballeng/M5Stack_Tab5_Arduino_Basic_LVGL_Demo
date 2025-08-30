/*
USB Drive UI Component Header
*/

#ifndef UI_USB_H
#define UI_USB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// USB UI objects
extern lv_obj_t * ui_USBPanel;
extern lv_obj_t * ui_USBStatusLabel;
extern lv_obj_t * ui_USBInfoLabel;
extern lv_obj_t * ui_USBFileList;
extern lv_obj_t * ui_USBPathLabel;
extern lv_obj_t * ui_USBRefreshBtn;
extern lv_obj_t * ui_USBBackBtn;

// Functions
void ui_usb_panel_init(lv_obj_t * parent);
void ui_usb_update_status(bool connected, const char* info);
void ui_usb_clear_file_list(void);
void ui_usb_add_file_entry(const char* name, bool is_dir, uint32_t size);
void ui_usb_update_path(const char* path);

#ifdef __cplusplus
}
#endif

#endif // UI_USB_H