/*
M5Stack Tab5 LVGL / SLS Initial Test - 23.05.25 - nikthefix

This preliminary demo just uses the display and touch features.
Peripherals like audio, gyro, camera etc will be added soon.

Dependencies:

ESP-Arduino >= V3.2 (tested also working with 3.3.0-alpha1)
M5GFX >= V0.2.8
LVGL = V8.3.11

lv_conf.h:

#define LV_COLOR_DEPTH 16
#define LV_COLOR_16_SWAP 1
#define LV_MEM_CUSTOM 1
#define LV_TICK_CUSTOM 1

Build Options:

Board: "ESP32P4 Dev Module"
USB CDC on boot: "Enabled"
Flash Size: "16MB (128Mb)"
Partition Scheme: "Custom" (the supplied partitions.csv file allows almost full use of the flash for the main app)
PSRAM: "Enabled"
Upload Mode: "UART / Hardware CDC"
USB Mode: "Hardware CDC and JTAG"

Notes:

This demo uses a software rotate in order to give us landscape mode (disp_drv.rotated = LV_DISP_ROT_90;)
I've not been able to find the make and model of the display unit used in the Tab5 so I can't be sure if it supports a hardware accelerated rotation.
The display driver chip is ili9881c with native portrait orientation.

A Square Line Studio V1.5.1 project is included so that you can experiment with your own exported UI files.

*/



#include <M5GFX.h>
#include <M5Unified.h>
#include <lvgl.h>
#include "ui.h"
#include "pins_config.h"

M5GFX display;

uint16_t count = 0;
bool automate = false;

// Clock variables
uint8_t hours = 0;
uint8_t minutes = 0;
uint8_t seconds = 0;
unsigned long lastSecondUpdate = 0;
bool timeSettingPanelVisible = false;

// Alarm variables
uint8_t alarmHours = 0;
uint8_t alarmMinutes = 0;
bool alarmEnabled = false;
bool alarmTriggered = false;
bool alarmSettingPanelVisible = false;
uint8_t alarmVolume = 75;
bool alarmFlashState = false;
unsigned long lastFlashTime = 0;
uint8_t snoozeMinutes = 10;
bool alarmSnoozed = false;
uint8_t snoozeTargetHour = 0;
uint8_t snoozeTargetMinute = 0;
unsigned long alarmStartTime = 0;
const unsigned long alarmTimeout = 600000; // 10 minutes timeout
unsigned long lastBeepTime = 0;
bool beepState = false;
 
static lv_disp_draw_buf_t draw_buf;
static lv_color_t *buf;



void lv_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t *)&color_p->full); 
    lv_disp_flush_ready(disp);
}



void my_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area)
{
    if (area->x1 % 2 != 0)
        area->x1 += 1;
    if (area->y1 % 2 != 0)
        area->y1 += 1;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    if (w % 2 != 0)
        area->x2 -= 1;
    if (h % 2 != 0)
        area->y2 -= 1;
}



static void lv_indev_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{  
     lgfx::touch_point_t tp[3];
     uint8_t touchpad = display.getTouchRaw(tp,3);
       if (touchpad > 0)
       {
          data->state = LV_INDEV_STATE_PR;
          data->point.x = tp[0].x;
          data->point.y = tp[0].y;
          //Serial.printf("X: %d   Y: %d\n", tp[0].x, tp[0].y); //for testing
       }
       else
       {
        data->state = LV_INDEV_STATE_REL;
       }
}


void updateClockDisplay()
{
    char timeStr[12];  // Increased buffer size to handle max values safely
    sprintf(timeStr, "%02d:%02d:%02d", hours, minutes, seconds);
    lv_label_set_text(ui_ClockLabel, timeStr);
}


void setTimeButtonClicked(lv_event_t * e)
{
    timeSettingPanelVisible = !timeSettingPanelVisible;
    if(timeSettingPanelVisible) {
        lv_obj_clear_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN);
        lv_spinbox_set_value(ui_HourSpinbox, hours);
        lv_spinbox_set_value(ui_MinuteSpinbox, minutes);
        lv_spinbox_set_value(ui_SecondSpinbox, seconds);
    } else {
        lv_obj_add_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN);
    }
}


void applyTimeButtonClicked(lv_event_t * e)
{
    hours = lv_spinbox_get_value(ui_HourSpinbox);
    minutes = lv_spinbox_get_value(ui_MinuteSpinbox);
    seconds = lv_spinbox_get_value(ui_SecondSpinbox);
    updateClockDisplay();
    lv_obj_add_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN);
    timeSettingPanelVisible = false;
}


void updateAlarmDisplay()
{
    char alarmStr[8];  // Increased buffer size for safety
    sprintf(alarmStr, "%02d:%02d", alarmHours, alarmMinutes);
    lv_label_set_text(ui_AlarmTimeLabel, alarmStr);
}


void setAlarmButtonClicked(lv_event_t * e)
{
    alarmSettingPanelVisible = !alarmSettingPanelVisible;
    if(alarmSettingPanelVisible) {
        lv_obj_clear_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN);
        lv_spinbox_set_value(ui_AlarmHourSpinbox, alarmHours);
        lv_spinbox_set_value(ui_AlarmMinuteSpinbox, alarmMinutes);
    } else {
        lv_obj_add_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN);
    }
}


void applyAlarmButtonClicked(lv_event_t * e)
{
    alarmHours = lv_spinbox_get_value(ui_AlarmHourSpinbox);
    alarmMinutes = lv_spinbox_get_value(ui_AlarmMinuteSpinbox);
    updateAlarmDisplay();
    lv_obj_add_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN);
    alarmSettingPanelVisible = false;
    alarmTriggered = false; // Reset alarm trigger when setting new time
}


void checkAlarm()
{
    if(alarmEnabled) {
        // Check for regular alarm time or snooze time
        bool shouldTrigger = false;
        
        if(alarmSnoozed && !alarmTriggered) {
            // Check if snooze time has been reached
            if(hours == snoozeTargetHour && minutes == snoozeTargetMinute && seconds == 0) {
                shouldTrigger = true;
                alarmSnoozed = false; // Reset snooze
            }
        } else if(!alarmTriggered && !alarmSnoozed) {
            // Check regular alarm time (only trigger once when we hit the exact time)
            if(hours == alarmHours && minutes == alarmMinutes && seconds == 0) {
                shouldTrigger = true;
            }
        }
        
        // Reset alarm trigger state when we're past the alarm time (for next day)
        if(alarmTriggered && !alarmSnoozed) {
            // If we're past the alarm time by at least 1 minute, reset for tomorrow
            if(hours != alarmHours || minutes != alarmMinutes) {
                // Only reset if we're not in the same minute as the alarm
                alarmTriggered = false;
                lv_obj_set_style_text_color(ui_AlarmLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
                // Also hide the wake up panel if it's still visible
                lv_obj_add_flag(ui_WakeUpPanel, LV_OBJ_FLAG_HIDDEN);
            }
        }
        
        if(shouldTrigger) {
            alarmTriggered = true;
            alarmStartTime = millis();  // Record when alarm started
            lastBeepTime = millis();
            beepState = false;
            // Show wake up panel
            lv_obj_clear_flag(ui_WakeUpPanel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_color(ui_AlarmLabel, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lastFlashTime = millis();
            alarmFlashState = false;
        }
    }
    
    // Handle flashing effect when alarm is triggered
    if(alarmTriggered) {
        unsigned long currentTime = millis();
        
        // Check for timeout (10 minutes)
        if(currentTime - alarmStartTime >= alarmTimeout) {
            // Auto-stop the alarm after timeout
            alarmTriggered = false;
            alarmSnoozed = false;
            M5.Speaker.stop();  // Stop sound
            lv_obj_add_flag(ui_WakeUpPanel, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_style_text_color(ui_AlarmLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            // Continue flashing
            if(currentTime - lastFlashTime >= 500) { // Flash every 500ms
                lastFlashTime = currentTime;
                alarmFlashState = !alarmFlashState;
                
                if(alarmFlashState) {
                    lv_obj_set_style_bg_color(ui_WakeUpPanel, lv_color_hex(0xFF0000), LV_PART_MAIN | LV_STATE_DEFAULT);
                } else {
                    lv_obj_set_style_bg_color(ui_WakeUpPanel, lv_color_hex(0xFFFF00), LV_PART_MAIN | LV_STATE_DEFAULT);
                }
            }
            
            // Play alarm sound
            if(currentTime - lastBeepTime >= 500) { // Beep cycle every 500ms
                lastBeepTime = currentTime;
                beepState = !beepState;
                if(beepState) {
                    M5.Speaker.tone(1000, 400);  // 1000Hz for 400ms
                } else {
                    M5.Speaker.stop();
                }
            }
        }
    }
}


void timePanelClickOutside(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * current = lv_event_get_current_target(e);
    
    // If clicked on the panel background (not a child element)
    if(target == current) {
        lv_obj_add_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN);
        timeSettingPanelVisible = false;
    }
}


void alarmPanelClickOutside(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_target(e);
    lv_obj_t * current = lv_event_get_current_target(e);
    
    // If clicked on the panel background (not a child element)
    if(target == current) {
        lv_obj_add_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN);
        alarmSettingPanelVisible = false;
    }
}


bool isChildOf(lv_obj_t * child, lv_obj_t * parent)
{
    lv_obj_t * p = lv_obj_get_parent(child);
    while(p != NULL) {
        if(p == parent) return true;
        p = lv_obj_get_parent(p);
    }
    return false;
}

void screenClickHandler(lv_event_t * e)
{
    // Close time setting panel if it's open
    if(timeSettingPanelVisible) {
        lv_obj_t * target = lv_event_get_target(e);
        // Check if click is outside the panel and not on the Set Time button
        if(!lv_obj_has_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN) &&
           !isChildOf(target, ui_TimeSettingPanel) &&
           target != ui_TimeSettingPanel &&
           target != ui_SetTimeBtn &&
           !isChildOf(target, ui_SetTimeBtn)) {
            lv_obj_add_flag(ui_TimeSettingPanel, LV_OBJ_FLAG_HIDDEN);
            timeSettingPanelVisible = false;
        }
    }
    
    // Close alarm setting panel if it's open
    if(alarmSettingPanelVisible) {
        lv_obj_t * target = lv_event_get_target(e);
        // Check if click is outside the panel and not on the Set Alarm button
        if(!lv_obj_has_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN) &&
           !isChildOf(target, ui_AlarmSettingPanel) &&
           target != ui_AlarmSettingPanel &&
           target != ui_SetAlarmBtn &&
           !isChildOf(target, ui_SetAlarmBtn)) {
            lv_obj_add_flag(ui_AlarmSettingPanel, LV_OBJ_FLAG_HIDDEN);
            alarmSettingPanelVisible = false;
        }
    }
}


void snoozeButtonClicked(lv_event_t * e)
{
    if(alarmTriggered) {
        // Calculate snooze time (current time + 10 minutes)
        snoozeTargetMinute = minutes + snoozeMinutes;
        snoozeTargetHour = hours;
        
        if(snoozeTargetMinute >= 60) {
            snoozeTargetMinute -= 60;
            snoozeTargetHour++;
            if(snoozeTargetHour >= 24) {
                snoozeTargetHour = 0;
            }
        }
        
        alarmSnoozed = true;
        alarmTriggered = false;
        
        // Hide wake up panel
        lv_obj_add_flag(ui_WakeUpPanel, LV_OBJ_FLAG_HIDDEN);
        
        // Stop sound
        M5.Speaker.stop();
        
        // Reset alarm label color to orange
        lv_obj_set_style_text_color(ui_AlarmLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}


void stopButtonClicked(lv_event_t * e)
{
    if(alarmTriggered) {
        alarmTriggered = false;
        alarmSnoozed = false;
        
        // Hide wake up panel
        lv_obj_add_flag(ui_WakeUpPanel, LV_OBJ_FLAG_HIDDEN);
        
        // Stop sound
        M5.Speaker.stop();
        
        // Reset alarm label color to orange
        lv_obj_set_style_text_color(ui_AlarmLabel, lv_color_hex(0xFF6600), LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}



void setup()
{
    /*Initialize M5Unified (display and speaker)*/
    auto cfg = M5.config();
    cfg.internal_spk = true;  // Enable internal speaker
    cfg.internal_mic = false; // Disable microphone
    M5.begin(cfg);
    
    // Use M5's display instance for LVGL
    display = M5.Display;
    
    // Set speaker volume
    M5.Speaker.setVolume(128);  // 50% volume

    Serial.begin(115200);//for debug

    /*Initialize LVGL*/
    lv_init();
    buf = (lv_color_t *)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LVGL_LCD_BUF_SIZE);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    //disp_drv.rounder_cb = my_rounder; 
    disp_drv.flush_cb = lv_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.sw_rotate = 1;  
    disp_drv.rotated = LV_DISP_ROT_90; 
    lv_disp_drv_register(&disp_drv);



    /*Initialize touch*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = lv_indev_read;
    lv_indev_drv_register(&indev_drv);     



    /*Start UI*/
    ui_init();   
    display.setBrightness(255);
    
    /*Setup clock event handlers*/
    lv_obj_add_event_cb(ui_SetTimeBtn, setTimeButtonClicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_ApplyTimeBtn, applyTimeButtonClicked, LV_EVENT_CLICKED, NULL);
    
    /*Setup alarm event handlers*/
    lv_obj_add_event_cb(ui_SetAlarmBtn, setAlarmButtonClicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_ApplyAlarmBtn, applyAlarmButtonClicked, LV_EVENT_CLICKED, NULL);
    
    /*Setup click-outside handlers*/
    lv_obj_add_event_cb(ui_Screen1, screenClickHandler, LV_EVENT_CLICKED, NULL);
    
    /*Setup wake up screen event handlers*/
    lv_obj_add_event_cb(ui_SnoozeBtn, snoozeButtonClicked, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_StopBtn, stopButtonClicked, LV_EVENT_CLICKED, NULL);
    
    /*Initialize displays*/
    updateClockDisplay();
    updateAlarmDisplay();
}



void loop()
{
  lv_timer_handler();
  delay(1);  

    if(lv_obj_has_state(ui_Button1, LV_STATE_CHECKED)) automate = true;
    else automate = false;
    
    if(automate == true)
    {
    lv_arc_set_value(ui_Arc1, count);
    lv_label_set_text_fmt(ui_Label1, "%d", count);
    count++;
    if (count == 1000) count = 0; 
    }

    uint8_t brightness = lv_slider_get_value(ui_Slider1) ;
    display.setBrightness(brightness);
    
    // Update clock every second
    unsigned long currentMillis = millis();
    if (currentMillis - lastSecondUpdate >= 1000) {
        lastSecondUpdate = currentMillis;
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) {
                    hours = 0;
                }
            }
        }
        updateClockDisplay();
        checkAlarm();
    }
    
    // Handle alarm switch state
    alarmEnabled = lv_obj_has_state(ui_AlarmSwitch, LV_STATE_CHECKED);
    
    // Handle alarm volume
    alarmVolume = lv_slider_get_value(ui_AlarmVolumeSlider);

}
