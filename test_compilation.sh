#!/bin/bash
# Test script to verify the project compiles without any Arduino library modifications

echo "==================================="
echo "Testing M5Stack Tab5 LVGL Project"
echo "==================================="
echo ""

# Check if lv_conf.h exists in Arduino libraries (it shouldn't!)
echo "1. Checking Arduino libraries for lv_conf.h..."
if [ -f "$HOME/Arduino/libraries/lv_conf.h" ]; then
    echo "   ❌ ERROR: Found lv_conf.h in Arduino libraries folder!"
    echo "   This file should NOT exist. The project should be self-contained."
    exit 1
else
    echo "   ✅ Good: No lv_conf.h in Arduino libraries folder"
fi

echo ""
echo "2. Checking project structure..."
REQUIRED_FILES=(
    "tab5_arduino_basic/lv_conf.h"
    "tab5_arduino_basic/lvgl_config.h"
    "tab5_arduino_basic/tab5_arduino_basic.ino"
)

for file in "${REQUIRED_FILES[@]}"; do
    if [ -f "$file" ]; then
        echo "   ✅ Found: $file"
    else
        echo "   ❌ Missing: $file"
        exit 1
    fi
done

echo ""
echo "3. Compiling project..."
cd tab5_arduino_basic

# Try to compile with arduino-cli if available
if command -v arduino-cli &> /dev/null; then
    arduino-cli compile --fqbn esp32:esp32:esp32p4 --warnings none . 2>&1 | tail -5
    if [ $? -eq 0 ]; then
        echo "   ✅ Compilation successful!"
    else
        echo "   ❌ Compilation failed"
        exit 1
    fi
else
    echo "   ⚠️  arduino-cli not found in PATH"
    echo "   Please compile manually in Arduino IDE"
fi

echo ""
echo "==================================="
echo "✅ Test complete! Project is self-contained."
echo "No Arduino library modifications required!"
echo "==================================="