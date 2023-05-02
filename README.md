# Inworld.AI NDK

The **Inworld AI NDK** enables Developers to integrate Inworld.ai characteres into a C++ application. We use **Inworld NDK** in our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) and in other higher level integrations later. 

## Platforms

The **Inworld NDK** is a library built by CMake. It contains all the source code needed to build for your specific platform. Officialy supported platforms are:

<table>
  <tr>
    <td><b>Windows</b></td>
    <td>Supported</td>
  </tr>
  <tr>
    <td><b>Mac</b></td>
    <td>Coming soon</td>
  </tr>
  <tr>
    <td><b>iOS</b></td>
    <td>Coming soon</td>
  </tr>
  <tr>
    <td><b>Android</b></td>
    <td>Coming soon</td>
  </tr>
</table>

## Windows

### Prerequisits

- [Visual Studio 2022](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/getting-started/#installing-visual-studio)
- git
- CMake
- **Inworld AI NDK** uses **Webrtc** for *Acoustic Echo Cancellation(AEC)*. See prerequisits for **Webrtc** on windows [here](https://webrtc.github.io/webrtc-org/native-code/development/prerequisite-sw/)

### Getting started

1. Clone a specific repo release, e.g. *git clone https://github.com/inworld-ai/inworld-ndk.git --branch=v1.0.0*
2. Call *git submodule update --init --recursive* to make sure all dependency submodules are initialized and updated
3. If you going to run **InworldNDKApp**(testing console application) fill out client options in *src/Application.cpp*. If you don't have an **Inworld.ai** account yet see more information [here](https://docs.inworld.ai/docs/intro)
4. Run *win-gen.bat* to generate prject files
5. Run *win-build.bat* to build **InworldNDK** static lib, **InworldNDKApp** and unit tests.

After build's succeed you have *InworldNDK/build* folder with all the source and binaries(including dependencies). You also can run *build/Release/InworldNDKApp.exe* and text message with your characters in console.

#### Acoustic Echo Cancellation(AEC)

*Inworld NDK* supports *AEC*. We use *Webrtc* for it so make sure you've looked into *Webrtc* prerequisits for Windows before trying to build *Inworld NDK* with *AEC*. To enable *AEC* edit *win-gen.bat* cmake call before run to *cmake .. -DAEC=True*. You'll also need to call *git config --system core.longpaths true* to elable long paths for git and avoid errors during *AEC* build. A specific *webrtc-aec-plugin* will be built during project generation process. **Note!** Building *Webrtc* takes a lot of time and disc space(>20Gb).

#### Inworld NDK integration examples

This repo contains **InworldNDKApp** a console application to demonstrate a basic integraion. Take our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/) as more advanced and complicated example.

## Inworld NDK structure

#### Client

In order to run a session with Inworld server you need to use either an object of [Client](https://github.com/inworld-ai/inworld-ndk/blob/main/InworldNDK/src/Client.h#:~:text=class%20INWORLDAINDK_API-,Client,-%3A%20public%20ClientBase)([example](https://github.com/inworld-ai/inworld-ndk/blob/main/src/Application.h#:~:text=Inworld%3A%3AClient%20_Client%3B)) or your own class inherited from [ClientBase](https://github.com/inworld-ai/inworld-ndk/blob/main/InworldNDK/src/Client.h#:~:text=class%20INWORLDAINDK_API-,ClientBase,-%7B), see example in our [**Unreal Engine SDK**](https://docs.inworld.ai/docs/tutorial-integrations/unreal-engine/).

#### Packets

*Inworld NDK* converts *GRPC* packets to own format. Use [PacketVisitor](https://github.com/inworld-ai/inworld-ndk/blob/main/src/Application.h#:~:text=Inworld%3A%3AClient%20_Client%3B) to [handle](https://github.com/inworld-ai/inworld-ndk/blob/main/src/PacketHandler.h#:~:text=class-,PacketHandler,-%3A%20public%20Inworld) different types of Packets.

#### Logging

*Inworld NDK* uses [spdlog](https://github.com/gabime/spdlog) as a default logging library. See [Log.h file](https://github.com/inworld-ai/inworld-ndk/blob/main/InworldNDK/src/Utils/Log.h#:~:text=/-,Log.h,-Go%20to%20file) to implement your own or disable logging.

#### InworldNDKApp

See [App](https://github.com/inworld-ai/inworld-ndk/blob/main/src/Application.h#:~:text=class-,App,-%7B) as a basic *InworldNDK* usage example. 
