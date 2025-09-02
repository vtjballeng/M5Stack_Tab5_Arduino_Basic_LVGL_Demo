# Code Quality Tools for M5Stack Tab5 Project

This project includes configuration for various code quality tools to maintain
consistent, clean code.

## Available Linters

### 1. Markdown Linting (Installed via npm)

**Tool**: markdownlint-cli

**Usage**:

```bash
# Check all markdown files
npm run lint:md

# Auto-fix markdown issues
npm run format:md
```

**Configuration**: `.markdownlint.json`

### 2. C/C++ Formatting (Requires separate installation)

**Tool**: clang-format

**Installation**:

```bash
# Ubuntu/WSL
sudo apt-get install clang-format

# macOS
brew install clang-format

# Windows
# Install LLVM from https://llvm.org/
```

**Usage**:

```bash
# Format a single file
clang-format -i tab5_arduino_basic/tab5_arduino_basic.ino

# Format all C/C++ files
find tab5_arduino_basic -name '*.ino' -o -name '*.cpp' -o -name '*.c' \
  -o -name '*.h' | xargs clang-format -i
```

**Configuration**: `.clang-format`

### 3. Arduino CLI Built-in Checks

**Tool**: arduino-cli (already in use)

**Usage**:

```bash
# Compile with all warnings
arduino-cli compile --fqbn esp32:esp32:esp32p4 --warnings all .
```

## Alternative C++ Linters

### Option A: Arduino Lint (Official Arduino tool)

```bash
# Install
npm install -g arduino-lint

# Run
arduino-lint --compliance strict
```

### Option B: PlatformIO Check

If using PlatformIO:

```bash
# Static code analysis
pio check

# With specific checks
pio check --flags="--enable=all"
```

### Option C: Cppcheck (Lightweight)

```bash
# Install
sudo apt-get install cppcheck

# Run
cppcheck --enable=all --inconclusive --std=c++11 \
  tab5_arduino_basic/*.cpp tab5_arduino_basic/*.h
```

## Recommended Workflow

1. **Before committing**: Run `npm run lint` to check markdown
2. **For C++ changes**: Use clang-format to maintain consistent style
3. **During compilation**: Use `--warnings all` flag with arduino-cli

## VS Code Integration

Install these extensions for automatic linting:

- **Markdown**: "markdownlint" by David Anson
- **C/C++**: "C/C++" by Microsoft (includes clang-format)
- **Arduino**: "Arduino" by Microsoft

Add to `.vscode/settings.json`:

```json
{
  "editor.formatOnSave": true,
  "C_Cpp.clang_format_style": "file",
  "markdownlint.config": {
    "extends": ".markdownlint.json"
  }
}
```

## GitHub Actions (Optional)

Create `.github/workflows/lint.yml`:

```yaml
name: Lint

on: [push, pull_request]

jobs:
  markdown:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - uses: actions/setup-node@v2
      - run: npm install
      - run: npm run lint:md
```

## Quick Commands

```bash
# Check everything
npm run lint

# Format markdown
npm run format:md

# Format C++ (requires clang-format)
find tab5_arduino_basic -name '*.ino' -o -name '*.cpp' -o -name '*.c' \
  -o -name '*.h' | xargs clang-format -i
```
