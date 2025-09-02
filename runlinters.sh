#!/bin/bash

echo "Running all linters for M5Stack Tab5 Arduino Basic LVGL Demo..."
echo ""

# Markdown linting
echo "=== Running Markdown Linter ==="
npx markdownlint "**/*.md"
echo ""

# C++ formatting check
echo "=== Running clang-format (check mode) ==="
clang-format --dry-run -Werror tab5_arduino_basic/*.ino tab5_arduino_basic/*.cpp tab5_arduino_basic/*.c tab5_arduino_basic/*.h 2>&1 | head -20
echo ""

# C++ static analysis
echo "=== Running cppcheck ==="
cppcheck --enable=warning,style,performance --suppress=missingIncludeSystem --suppress=syntaxError tab5_arduino_basic/*.cpp tab5_arduino_basic/*.h 2>&1
echo ""

echo "Linting complete!"