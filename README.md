# Nimbus

Android 加固样本的静态脱壳工具。只做一件事：识别加固、把 dex 从容器里取出来、写盘。

## 下载安装

正式版在 [Releases](https://github.com/xuejiy666-ai/nimbus/releases/latest)，按 ABI 拆成 5 个包，同一份证书签名，可互相覆盖安装：

| 文件 | 适用 |
| --- | --- |
| `Nimbus-1.4-arm64-v8a.apk` | 绝大多数现在的安卓手机（推荐） |
| `Nimbus-1.4-armeabi-v7a.apk` | 老机型 / 32 位系统 |
| `Nimbus-1.4-x86.apk` | x86 模拟器、部分平板 |
| `Nimbus-1.4-x86_64.apk` | x86_64 模拟器、Chromebook |
| `Nimbus-1.4-universal.apk` | 不确定设备架构时装它，一定跑得起来，体积最大 |

需要 Android 5.0（minSdk 21）及以上，无需 Root。每次推送 `main` 由 GitHub Actions 自动编译，产物在 Actions 页面 `nimbus-release-apks` 附件里；打 `v*` 标签会把 5 个包一起挂到 Release。

## 功能范围

- 支持 epic（含 v3 抽离、epic2 配置型）、dpt-shell、百度等常见加固的**静态提取**。
- 只提取 dex 本体。

## 工程结构

```
gradlew, gradlew.bat, gradle/wrapper/   自带 Gradle 8.7 wrapper
app/src/main/
  java/com/mint/toolbox/
    MainActivity.kt     选包、复制到私有目录、调原生层、写运行日志、显示输出路径
    Native.kt           声明外部函数并加载 libapp.so
  res/
    layout/activity_main.xml   单页浅色界面（输入 / 处理 / 日志 / 输出 四张卡片）
    values/colors.xml, styles.xml   主题与配色（iOS 浅色）
    drawable/                    圆角卡片与按钮背景
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

最省事的是让 GitHub 编译：仓库已带 `.github/workflows/android.yml`，推送 `main` 或手动 Run workflow，几分钟后出 5 个 release 包；打 `v*` 标签会全部挂到 Release。

签名密钥不进仓库，放在 Actions Secrets：`ANDROID_KEYSTORE_BASE64`、`ANDROID_KEYSTORE_PASSWORD`、`ANDROID_KEY_ALIAS`、`ANDROID_KEY_PASSWORD`。没有这些变量时，`release` 会退回 debug 签名，保证本地照样能出包。

本地构建：

1. 用 Android Studio / Android Code Studio 打开本目录；
2. 等待 Gradle Sync（首次会下载 SDK、NDK、CMake）；
3. Build → Build APK(s)，或命令行：

```bash
./gradlew assembleRelease    # 输出 app/build/outputs/apk/release/，含 4 个单 ABI 包 + universal 包
```

要求：minSdk 21，compileSdk 34，NDK 需带 CMake 与 zlib。

## 使用

界面是浅色 iOS 风格的单页：顶部标题，下面依次是「安装包」卡片（选中的文件名与大小）、「处理」卡片（开始处理按钮与进度）、「运行日志」卡片（带时间戳的实时输出）、「输出位置」卡片（处理完成后出现，可复制路径）。

流程：

1. 点「选择安装包」挑一个加固过的 APK；
2. 点「开始处理」，日志会依次打出复制、调用原生层、返回结果、耗时；
3. 处理完成后 dex 位于：

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
