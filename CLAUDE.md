# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

QtSkia is a 2D graphics library that integrates Google's Skia graphics engine with Qt's rendering framework. It provides Qt developers with easy access to Skia's powerful 2D graphics capabilities through familiar Qt classes like QWidget, QOpenGLWidget, QQuickWindow, and QQuickItem.

## Build Commands

### Initial Setup

1. **Clone the repository:**
   ```bash
   git clone https://github.com/QtSkia/QtSkia.git
   # or for China users:
   git clone https://gitee.com/QtSkia/QtSkia.git
   ```

2. **Download Skia and dependencies:**
   - Windows: Run `syncSkia.bat` or `syncSkia-gitee.bat` (for China users)
   - macOS/Linux: Run `chmod a+x syncSkia.sh && ./syncSkia.sh` or `./syncSkia-gitee.sh`
   - This fetches Skia and 28+ third-party dependencies from mirrors

### Building

Using QtCreator:
```bash
# Open QtSkia.pro in QtCreator and build
```

Using command line:
```bash
qmake
make
```

**Important:** The first build will take a long time as it compiles Skia using GN/Ninja. Subsequent builds will be faster.

### Build Output

Build artifacts are organized in `bin/` with the following structure:
```
bin/[compiler]/[static|shared]/[debug|release]/
```

- `compiler`: msvc, clang, or gcc
- `static|shared`: Controlled by `.qmake.conf` (`QtSkia_Static_Build`)
- `debug|release`: Based on CONFIG

### Static vs Shared Build

Edit `.qmake.conf` to change between static and shared linking:
```qmake
# false for shared build, true for static build
QtSkia_Static_Build = false
```

## Architecture

### Module Structure

QtSkia is organized into three main Qt framework integration modules:

1. **QtSkiaWidget** (`QtSkia/QtSkiaWidget/`)
   - `QSkiaWidget`: CPU raster rendering using QWidget
   - `QSkiaOpenGLWidget`: Hardware-accelerated rendering using QOpenGLWidget
   - Both provide `draw(SkCanvas* canvas, int elapsed)` virtual function
   - Requires: `Qt += core gui widgets`

2. **QtSkiaGui** (`QtSkia/QtSkiaGui/`)
   - `QSkiaOpenGLWindow`: Pure OpenGL window integration (only Qt Gui dependency)
   - `QSkiaVulkanWindow`: Vulkan window integration (stub/not fully implemented)
   - Provides `onInit()`, `onResize()`, `draw()` virtual functions
   - Requires: `Qt += core gui`

3. **QtSkiaQuick** (`QtSkia/QtSkiaQuick/`)
   - `QSkiaQuickWindow`: Integrates with QQuickWindow
     - `drawBeforeSG()`: Draw before Qt Quick SceneGraph
     - `drawAfterSG()`: Draw after Qt Quick SceneGraph
   - `QSkiaQuickItem`: Integrates with QQuickItem for custom rendering items
   - Both work in Skia Renderer Thread
   - Requires: `Qt += core gui quick`

### Build System

QtSkia uses a **hybrid build system**:

- **qmake** (root level): Orchestrates the entire build
- **GN/Ninja** (Skia level): Compiles Skia and its dependencies
- Platform configs in `skiaBuild/buildConfig/` contain platform-specific GN args

Key build files:
- `QtSkia.pro`: Root qmake project
- `skiaCommon.pri`: Common Skia configuration (include paths, library linking)
- `uniqueDestdir.pri`: Output directory organization
- `skiaBuild/skiaBuild.pro`: Template=aux, runs GN to generate Ninja build files

### Skia Integration

- Skia source is located in `3rdparty/skia/`
- Skia's 28+ dependencies are in `3rdparty/skia/third_party/externals/`
- All dependencies are mirrored on GitHub (https://github.com/QtSkia) and Gitee
- Auto-sync from upstream via GitHub Actions
- QtSkia does not modify Skia code, only adds mirror support and build configuration

## Dependencies

### Required Tools

- **Python 2**: Required by Skia's `git-sync-deps` script
- **Qt 5.12.x**: 64-bit version
- **Windows**: Visual Studio 2017+ (clang-cl recommended)
- **Build Tools**: GN and Ninja (included in `skiaBuild/buildTool/`)

### Key Third-Party Libraries (in `3rdparty/skia/third_party/externals/`)

- `libjpeg-turbo`, `libpng`, `libwebp`: Image codecs
- `freetype`, `harfbuzz`: Font rendering and text shaping
- `spirv-tools`, `dawn`, `swiftshader`: Vulkan/WebGPU tools
- `imgui`, `sdl`: UI and media libraries
- And 20+ more...

## Using QtSkia in Applications

### Basic Usage Pattern

1. Inherit from the appropriate QtSkia class (QSkiaWidget, QSkiaOpenGLWidget, etc.)
2. Override the virtual draw function
3. Use SkCanvas API to draw graphics
4. Always call `canvas->clear()` at the start and `canvas->flush()` at the end

### Example (QWidget):

```cpp
#include "QSkiaWidget.h"

class MySkiaWidget : public QSkiaWidget {
    void draw(SkCanvas *canvas) override {
        canvas->clear(SK_ColorWHITE);

        SkPaint paint;
        paint.setColor(SK_ColorRED);
        canvas->drawString("Hello Skia", 50, 50, SkFont(), paint);

        canvas->flush();
    }
};
```

See `doc/Examples.md` for comprehensive examples of all widget types.

## Examples

The `examples/` directory contains working demonstrations:

- `HelloSkiaWidget/`: Basic QWidget example
- `HelloSkiaOpenGLWidget/`: QOpenGLWidget example
- `HelloSkiaOpenGLWindow/`: QOpenGLWindow example
- `HelloSkiaQuickWindow/`: QQuickWindow example
- `HelloSkiaQuickItem/`: QQuickItem example
- `FeatureShow/`: Comprehensive feature showcase

Each example demonstrates different Skia features: shapes, Bezier curves, text, path effects, shaders, etc.

## Common Patterns

### Rendering Loop

For animations, use QTimer to trigger updates:
```cpp
QTimer* timer = new QTimer(this);
connect(timer, &QTimer::timeout, this, QOverload<>::of(&QWidget::update));
timer->start(16); // ~60 FPS
```

### Quick Item Registration

To use QSkiaQuickItem in QML:
```cpp
qmlRegisterType<MySkiaItem>("SkiaQuickItem", 1, 0, "MySkiaItem");
```

## Important Notes

- **C++17 Standard**: Required by Skia
- **Character Encoding**: MSVC builds use UTF-8 source encoding
- **Qt Macros**: `QT_NO_FOREACH` is defined (avoid Qt's foreach, use range-based for)
- **SKIA_DLL**: Defined for shared library builds
- **First Build**: Compiling Skia from source takes significant time (10+ minutes)
- **Platform Support**: Windows and macOS are fully supported; Linux, Android, iOS are in progress

## Testing

No formal unit test framework is currently set up. The `examples/` directory serves as integration tests/demos.

## CI/CD

GitHub Actions workflows in `.github/workflows/`:
- `windows.yml`: Windows builds
- `macos.yml`: macOS builds
- `ubuntu.yml`: Ubuntu builds
- `android.yml`: Android builds
- iOS builds (workflow name varies)
