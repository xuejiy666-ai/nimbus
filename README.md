# Nimbus

Android 加固样本的静态脱壳工具。只做一件事：识别加固、把 dex 从容器里取出来、写盘。

## 下载安装

- 已编译的 APK：[Releases](https://github.com/xuejiy666-ai/nimbus/releases/latest) → `Nimbus-1.4-debug.apk`
- 每次推送 `main` 由 GitHub Actions 自动编译，产物在 Actions 页面的 `nimbus-apk` 附件里
- 需要：Android 5.0（minSdk 21）及以上，无需 Root

## 功能范围

- 支持 epic（含 v3 抽离、epic2 配置型）、dpt-shell、百度等常见加固的**静态提取**。
- 只提取 dex 本体。

## 工程结构

```
gradlew, gradlew.bat, gradle/wrapper/   自带 Gradle 8.7 wrapper
app/src/main/
  java/com/mint/toolbox/
    MainActivity.kt     选取 APK，复制到私有目录后调用原生层
    Native.kt           声明外部函数并加载 libapp.so
  cpp/
    sc.h                公共声明（编译期通过 -include 注入，行内不再写 include）
    sc_globals.c        全局变量定义
    A.c … Z.c AA.c …    按体积装箱切分的实现文件，共 220 个
  CMakeLists.txt        自动收集 *.c，产出唯一的 libapp.so
```

构建产物只有 `libapp.so` 一个动态库：

```cmake
file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
add_library(app SHARED ${SRC})
target_compile_options(app PRIVATE -include ${CMAKE_CURRENT_SOURCE_DIR}/sc.h)
target_link_libraries(app z dl)
```

## 构建

最省事的是让 GitHub 编译：仓库已带 `.github/workflows/android.yml`，推送 `main` 或手动 Run workflow，几分钟后出 `app-debug.apk`；打 `v*` 标签会自动把 APK 附到 Release。

本地构建：

1. 用 Android Studio / Android Code Studio 打开本目录；
2. 等待 Gradle Sync（首次会下载 SDK、NDK、CMake）；
3. Build → Build APK(s)。

也可以直接用命令行（需要本机有 JDK 17）：

```bash
./gradlew assembleRelease
```

要求：minSdk 21，compileSdk 34，NDK 需带 CMake 与 zlib。

## 使用

安装后点击按钮选择 APK，处理完成后 dex 位于：

```
/sdcard/Android/data/com.mint.toolbox/files/Dex/<包名>/classes*.dex
```

## 切分规则

`tools/split.py` 按顶层定义切成单元，再按体积做装箱（LPT），使每个 `.c` 体量接近；单个超长函数无法在不改变逻辑的前提下拆分，会单独成文件。新增代码后重跑：

```bash
python3 tools/split.py <单文件源码.c> app/src/main/cpp 220
```

## 贡献

见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 许可

[MIT](LICENSE)
