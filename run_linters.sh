#!/bin/bash
# Run all linters for the M5Stack Tab5 project

echo "================================"
echo "Running All Linters"
echo "================================"
echo ""

# 1. Markdown Linter
echo "1. MARKDOWN LINTER"
echo "-----------------"
npm run lint
echo ""

# 2. C++ Formatter (clang-format)
echo "2. C++ FORMATTER (clang-format)"
echo "-------------------------------"
if command -v clang-format &> /dev/null; then
    echo "Running clang-format on all C/C++ files..."
    find tab5_arduino_basic -name '*.ino' -o -name '*.cpp' -o -name '*.c' -o -name '*.h' | \
    while read file; do
        echo "Formatting: $file"
        clang-format -i "$file"
    done
    echo "✅ C++ formatting complete!"
else
    echo "⚠️  clang-format not installed. Install with: sudo apt-get install clang-format"
fi
echo ""

# 3. Arduino CLI Compilation with Warnings
echo "3. ARDUINO COMPILATION WARNINGS"
echo "--------------------------------"
cd tab5_arduino_basic
echo "Compiling with all warnings enabled..."
../bin/arduino-cli compile --fqbn esp32:esp32:esp32p4 --warnings all . 2>&1 | \
    grep -E "warning:|error:" | head -20
if [ $? -eq 0 ]; then
    echo "✅ Compilation check complete!"
else
    echo "✅ No warnings found!"
fi
cd ..
echo ""

# 4. Optional: cppcheck (if installed)
echo "4. STATIC ANALYSIS (cppcheck)"
echo "-----------------------------"
if command -v cppcheck &> /dev/null; then
    echo "Running cppcheck..."
    cppcheck --enable=warning,style --inconclusive --std=c++11 \
        --suppress=missingIncludeSystem \
        tab5_arduino_basic/*.cpp tab5_arduino_basic/*.h \
        tab5_arduino_basic/*.ino 2>&1 | grep -v "^Checking"
    echo "✅ Static analysis complete!"
else
    echo "⚠️  cppcheck not installed. Install with: sudo apt-get install cppcheck"
fi
echo ""

echo "================================"
echo "Linting Complete!"
echo "================================"