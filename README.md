# Inworld.AI NDK

The **Inworld AI NDK** enables Developers to integrate Inworld.ai characters into a C++ application. We use **Inworld NDK** in our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) and in other higher level integrations later. 

## Platforms

The **Inworld NDK** is a library built by CMake. It contains all the source code needed to build for your specific platform. Officially supported platforms are:

<table>
  <tr>
    <td><b>Windows</b></td>
    <td>Supported</td>
  </tr>
  <tr>
    <td><b>Mac</b></td>
    <td>Supported</td>
  </tr>
  <tr>
    <td><b>iOS</b></td>
    <td>Supported</td>
  </tr>
  <tr>
    <td><b>Android</b></td>
    <td>Coming soon</td>
  </tr>
</table>

### Prerequisites

- [Visual Studio 2022(Win)](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/getting-started/#installing-visual-studio)
- Xcode(MacOS)
- git
- CMake
- **Inworld AI NDK** uses **Webrtc** for *Acoustic Echo Cancellation(AEC)*(Build supported for Windows only now). See prerequisites for **Webrtc** [here](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/)

### Getting started

1. Clone *Main* branch or a specific repo release, e.g. *git clone https://github.com/inworld-ai/inworld-ndk.git --branch=v1.0.0*
2. Call *git submodule update --init --recursive* to make sure all dependency submodules are initialized and updated
3. If you going to run **InworldNDKApp**(testing console application) fill out client options in *Example/ConsoleApp/src/Application.cpp*. If you don't have an **Inworld.ai** account yet see more information [here](https://docs.inworld.ai/docs/intro)
4. Run *gen* script for your platform (*win-gen.bat*, *mac-gen.sh*) to generate project files
5. Run *build* script for your platform (*win-build.bat*, *mac-build.sh*) to build **InworldNDK** static lib, **InworldNDKApp** and unit tests.

After build's succeed you have *build/Package* folder with all the source and binaries(including dependencies). You can run *build/Release/InworldNDKApp* executable and text message with your characters in console.

#### Acoustic Echo Cancellation, *AEC* (Windows)

*Inworld NDK* supports *AEC*. We use *Webrtc* for it so make sure you've looked into *Webrtc* prerequisites for Windows before trying to build *Inworld NDK* with *AEC*. To enable *AEC* edit *win-gen.bat* cmake call before run to *cmake .. -DAEC=True*. You'll also need to call *git config --system core.longpaths true* to elable long paths for git and avoid errors during *AEC* build. A specific *webrtc-aec-plugin* will be built during project generation process. **Note!** Building *Webrtc* takes a lot of time and disc space.

#### Inworld NDK integration examples

This repo contains **InworldNDKApp** a console application to demonstrate a basic integration. Take our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) as more advanced and complicated example.
