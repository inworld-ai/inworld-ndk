# Inworld.AI NDK

The **Inworld AI NDK** enables Developers to integrate Inworld.ai characters into a C++ application. We use **Inworld NDK** in our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) and in other higher level integrations. 

## Platforms

The **Inworld NDK** is a library built by CMake. It contains all the source code needed to build for your specific platform. Officially supported platforms are:

<table>
  <tr>
    <td><b>Windows</b></td>
  </tr>
  <tr>
    <td><b>Mac</b></td>
  </tr>
  <tr>
    <td><b>iOS</b></td>
  </tr>
  <tr>
    <td><b>Android</b></td>
  </tr>
</table>

### Prerequisites

- [Visual Studio 2022(Win)](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/getting-started/#installing-visual-studio)
- Xcode(MacOS)
- git
- CMake
- **Inworld AI NDK** uses **Webrtc** for *Acoustic Echo Cancellation(AEC)*(Build supported for Windows only now). See prerequisites for **Webrtc** [here](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/)

### Getting started

Download latest release from [releases page](https://github.com/inworld-ai/inworld-ndk/releases). A release contains source code, headers and binaries for supported platforms.

#### Release prebuilt binaries

| Platform    | Built with                                |
| ----------- | --------                                  |
| Win64       | Visual Studio 2022, MSVC 19.37.32826.1    |
| Mac         | Xcode 14.3.1, AppleClang 14.0.3.14030022  |
| iOS         | Xcode 14.3.1, AppleClang 14.0.3.14030022  |
| Android     | Clang 14.0.7                              |

#### Build from source

1. Clone *Main* branch
2. Call *git submodule update --init --recursive* to make sure all dependency submodules are initialized and updated
3. If you going to run **inworld-ndk-app**(testing console application for Windows and Mac) fill out client options in *Example/ConsoleApp/src/Application.cpp*. If you don't have an **Inworld.ai** account yet see more information [here](https://docs.inworld.ai/docs/intro)
4. Run *gen* script for your platform (*win-gen.bat*, *mac-gen.sh* etc..) to generate project files
5. Run *build* script for your platform (*win-build.bat*, *mac-build.sh* etc..) to build **inworld-ndk** static lib, **inworld-ndk-app**(Win, Mac) and unit tests.

After build's succeed you have *build/Package* folder with all the source and binaries(including dependencies). You can run *build/Release/inworld-ndk-app* executable(Win, Mac) and text message with your characters in console.

### Android

> **_NOTE:_**  Tested on MacOS only

To build for Android: 
- Duplicate *android-gen.sh* script and rename it to *android-gen-local.sh*
- Open *android-gen-local.sh* with a text editor and configure the flags *CMAKE_SYSTEM_VERSION*(target device Android API), *CMAKE_ANDROID_ARCH_ABI*(target devide architecture) and *CMAKE_ANDROID_NDK*(path to Android NDK on your machine)
- Run *android-gen-local.sh*
- Run *android-build.sh*

#### Android Example Application

Find Android Example App project in Exaple/AndroidApp. To run the App:
- Build Android binaries(see section above)
- Open Example/AndroidApp project in *Android Studio*(tested on Android Studio Ladybug | 2024.2.1 Patch 3)
- Fill *Options.Resource*, *Options.ApiKey*, *Options.ApiSecret*, and *SceneID* in app/cpp/cpp/native-lib.cpp. If you don't have an **Inworld.ai** account yet see more information [here](https://docs.inworld.ai/docs/intro)
- Build and run on device or emulator
- Inworld Client is created and started on MainActivity's onCreate, a greeting to a character is sent when Inworld session is started. See *Logcat* for logs, filter by *inworld-ndk*
- See app/cpp/cpp/CMakeLists.txt as an example on how to link with Inworld
- See app/cpp/cpp/native-lib.cpp as an example on how to create and start Inworld Client

#### Acoustic Echo Cancellation, *AEC* (Windows)

*Inworld NDK* supports *AEC*. We use *Webrtc* for it so make sure you've looked into *Webrtc* prerequisites for Windows before trying to build *Inworld NDK* with *AEC*. To enable *AEC* edit *win-gen.bat* cmake call before run to *cmake .. -DAEC=True*. You'll also need to call *git config --system core.longpaths true* to elable long paths for git and avoid errors during *AEC* build. A specific *webrtc-aec-plugin* will be built during project generation process. **Note!** Building *Webrtc* takes a lot of time and disc space.

#### Inworld NDK integration examples

This repo contains **inworld-ndk-app** a console application to demonstrate a basic integration. Take our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) as more advanced and complicated example.
