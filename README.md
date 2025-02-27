# Inworld.AI NDK

**Inworld AI NDK** integrates [Inworld.ai](https://inworld.ai) characters into C++ applications. It’s primarily built with CMake and officially supports **Windows**, **Mac**, **iOS**, and **Android**.

## Contents
- [Inworld.AI NDK](#inworldai-ndk)
  - [Contents](#contents)
  - [Prerequisites](#prerequisites)
  - [Downloading Prebuilt Binaries](#downloading-prebuilt-binaries)
  - [Building From Source](#building-from-source)
    - [Windows](#windows)
      - [Acoustic Echo Cancellation (AEC) (Windows)](#acoustic-echo-cancellation-aec-windows)
      - [Steps to build](#steps-to-build)
    - [Mac](#mac)
    - [iOS](#ios)
    - [Android](#android)

---

## Prerequisites
- **Git**
- **CMake**
- **cppcheck**
- **Visual Studio 2022 (Win)** or **Xcode (Mac/iOS)**
- [WebRTC prerequisites](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/) (required for AEC on Windows)

---

## Downloading Prebuilt Binaries
Grab the latest release from the [Releases page](https://github.com/inworld-ai/inworld-ndk/releases). Each release includes source code, headers, and platform-specific binaries.

| Platform | Built with                                      |
|----------|-------------------------------------------------|
| Win64    | Visual Studio 2022, MSVC 19.37.32826.1          |
| Mac      | Xcode 14.3.1, AppleClang 14.0.3.14030022         |
| iOS      | Xcode 14.3.1, AppleClang 14.0.3.14030022         |
| Android  | Clang 14.0.7                                    |

---

## Building From Source

1. **Clone** the repository (main branch).
2. Run `git submodule update --init --recursive`.

If you want to run the console app (`inworld-ndk-app`) on Windows or Mac, edit `Example/ConsoleApp/src/Application.cpp` to set up client options (API Key, Secret, etc.). More info on setting up an account: [Inworld Docs](https://docs.inworld.ai/docs/intro).

### Windows
#### Acoustic Echo Cancellation (AEC) (Windows)
To enable AEC support when building on Windows:
1. Ensure [WebRTC prerequisites](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/) are met.
2. Enable AEC in `win-gen.bat` by setting `-DAEC=True`.

#### Steps to build
1. Install **Visual Studio 2022** and ensure the C++ workload is enabled.
2. Open an instance of **Developer Command Prompt for VS2022** or **Developer Powershell for VS2022**. This is the terminal to use for the next steps.
3. From the terminal, run:
   - `win-gen.bat` (generates Visual Studio project files)
   - `win-build.bat` (builds the static lib, console app, and tests)
4. Binaries and libraries will appear under `build/Package`.

### Mac
1. Install **Xcode** and ensure command-line tools are set up.
2. From a terminal, run:
   - `chmod +x mac-gen.sh && ./mac-gen.sh`
   - `chmod +x mac-build.sh && ./mac-build.sh`
3. Find build outputs in `build/Package`.

### iOS
1. Install **Xcode** and command-line tools.
2. Adjust any iOS-specific flags in `ios-gen.sh` (if provided) or in your custom script (e.g., iOS deployment target).
3. From a terminal, run:
   - `chmod +x ios-gen.sh && ./ios-gen.sh`
   - `chmod +x ios-build.sh && ./ios-build.sh`
4. Output artifacts appear in `build/Package`.

### Android
> **Note:** Tested on MacOS only. 
1. Duplicate `android-gen.sh` to `android-gen-local.sh` and edit:
   - `CMAKE_SYSTEM_VERSION` (Android API level)
   - `CMAKE_ANDROID_ARCH_ABI` (e.g., `arm64-v8a`, `armeabi-v7a`)
   - `CMAKE_ANDROID_NDK` (path to your Android NDK)
2. From a terminal, run:
   - `chmod +x android-gen-local.sh && ./android-gen-local.sh`
   - `chmod +x android-build.sh && ./android-build.sh`
3. Find the build outputs in `build/Package`.

---
