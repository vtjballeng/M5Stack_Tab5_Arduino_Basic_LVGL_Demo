/*
USB Drive UI Component
Displays USB drive status and file browser
*/

#include <stdio.h>
#include "ui.h"
#include "ui_helpers.h"

// USB Panel UI objects
lv_obj_t * ui_USBPanel;
lv_obj_t * ui_USBStatusLabel;
lv_obj_t * ui_USBInfoLabel;
lv_obj_t * ui_USBFileList;
lv_obj_t * ui_USBPathLabel;
lv_obj_t * ui_USBRefreshBtn;
lv_obj_t * ui_USBBackBtn;

// Create USB UI components
void ui_usb_panel_init(lv_obj_t * parent)
{
    // Create USB Panel
    ui_USBPanel = lv_obj_create(parent);
    lv_obj_set_width(ui_USBPanel, 400);
    lv_obj_set_height(ui_USBPanel, 500);
    lv_obj_set_x(ui_USBPanel, 380);
    lv_obj_set_y(ui_USBPanel, 50);
    lv_obj_set_align(ui_USBPanel, LV_ALIGN_TOP_LEFT);
    lv_obj_clear_flag(ui_USBPanel, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(ui_USBPanel, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_USBPanel, lv_color_hex(0x2B2B2B), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_USBPanel, lv_color_hex(0x4A4A4A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui_USBPanel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

    // USB Status Label
    ui_USBStatusLabel = lv_label_create(ui_USBPanel);
    lv_obj_set_width(ui_USBStatusLabel, LV_SIZE_CONTENT);
    lv_obj_set_height(ui_USBStatusLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_USBStatusLabel, 10);
    lv_obj_set_y(ui_USBStatusLabel, 10);
    lv_obj_set_align(ui_USBStatusLabel, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_USBStatusLabel, "USB: Not Connected");
    lv_obj_set_style_text_color(ui_USBStatusLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_USBStatusLabel, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // USB Info Label (capacity, etc)
    ui_USBInfoLabel = lv_label_create(ui_USBPanel);
    lv_obj_set_width(ui_USBInfoLabel, 380);
    lv_obj_set_height(ui_USBInfoLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_USBInfoLabel, 10);
    lv_obj_set_y(ui_USBInfoLabel, 35);
    lv_obj_set_align(ui_USBInfoLabel, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_USBInfoLabel, "Insert USB drive to view contents");
    lv_obj_set_style_text_color(ui_USBInfoLabel, lv_color_hex(0xAAAAAA), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_USBInfoLabel, &lv_font_montserrat_12, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Current Path Label
    ui_USBPathLabel = lv_label_create(ui_USBPanel);
    lv_obj_set_width(ui_USBPathLabel, 380);
    lv_obj_set_height(ui_USBPathLabel, LV_SIZE_CONTENT);
    lv_obj_set_x(ui_USBPathLabel, 10);
    lv_obj_set_y(ui_USBPathLabel, 60);
    lv_obj_set_align(ui_USBPathLabel, LV_ALIGN_TOP_LEFT);
    lv_label_set_text(ui_USBPathLabel, "Path: /");
    lv_obj_set_style_text_color(ui_USBPathLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_USBPathLabel, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    // File List
    ui_USBFileList = lv_list_create(ui_USBPanel);
    lv_obj_set_width(ui_USBFileList, 380);
    lv_obj_set_height(ui_USBFileList, 350);
    lv_obj_set_x(ui_USBFileList, 10);
    lv_obj_set_y(ui_USBFileList, 85);
    lv_obj_set_align(ui_USBFileList, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(ui_USBFileList, lv_color_hex(0x1A1A1A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui_USBFileList, lv_color_hex(0x3A3A3A), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui_USBFileList, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);

    // Back Button
    ui_USBBackBtn = lv_btn_create(ui_USBPanel);
    lv_obj_set_width(ui_USBBackBtn, 80);
    lv_obj_set_height(ui_USBBackBtn, 35);
    lv_obj_set_x(ui_USBBackBtn, 10);
    lv_obj_set_y(ui_USBBackBtn, 445);
    lv_obj_set_align(ui_USBBackBtn, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(ui_USBBackBtn, lv_color_hex(0x4A4A4A), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_t * back_label = lv_label_create(ui_USBBackBtn);
    lv_label_set_text(back_label, "Back");
    lv_obj_center(back_label);

    // Refresh Button
    ui_USBRefreshBtn = lv_btn_create(ui_USBPanel);
    lv_obj_set_width(ui_USBRefreshBtn, 80);
    lv_obj_set_height(ui_USBRefreshBtn, 35);
    lv_obj_set_x(ui_USBRefreshBtn, 100);
    lv_obj_set_y(ui_USBRefreshBtn, 445);
    lv_obj_set_align(ui_USBRefreshBtn, LV_ALIGN_TOP_LEFT);
    lv_obj_set_style_bg_color(ui_USBRefreshBtn, lv_color_hex(0x0066CC), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    lv_obj_t * refresh_label = lv_label_create(ui_USBRefreshBtn);
    lv_label_set_text(refresh_label, "Refresh");
    lv_obj_center(refresh_label);
}

// Update USB status display
void ui_usb_update_status(bool connected, const char* info)
{
    if(connected) {
        lv_label_set_text(ui_USBStatusLabel, "USB: Connected");
        lv_obj_set_style_text_color(ui_USBStatusLabel, lv_color_hex(0x00FF00), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_USBInfoLabel, info);
    } else {
        lv_label_set_text(ui_USBStatusLabel, "USB: Not Connected");
        lv_obj_set_style_text_color(ui_USBStatusLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text(ui_USBInfoLabel, "Insert USB drive to view contents");
        lv_obj_clean(ui_USBFileList);
    }
}

// Clear file list
void ui_usb_clear_file_list()
{
    lv_obj_clean(ui_USBFileList);
}

// Add file to list
void ui_usb_add_file_entry(const char* name, bool is_dir, uint32_t size)
{
    char entry_text[128];
    if(is_dir) {
        snprintf(entry_text, sizeof(entry_text), "[DIR] %s", name);
    } else {
        if(size < 1024) {
            snprintf(entry_text, sizeof(entry_text), "%s (%u B)", name, size);
        } else if(size < 1024*1024) {
            snprintf(entry_text, sizeof(entry_text), "%s (%.1f KB)", name, size/1024.0);
        } else {
            snprintf(entry_text, sizeof(entry_text), "%s (%.1f MB)", name, size/(1024.0*1024.0));
        }
    }
    
    lv_obj_t * btn = lv_list_add_btn(ui_USBFileList, is_dir ? LV_SYMBOL_DIRECTORY : LV_SYMBOL_FILE, entry_text);
    lv_obj_set_style_text_font(btn, &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);
}

// Update current path display
void ui_usb_update_path(const char* path)
{
    char path_text[256];
    snprintf(path_text, sizeof(path_text), "Path: %s", path);
    lv_label_set_text(ui_USBPathLabel, path_text);
}