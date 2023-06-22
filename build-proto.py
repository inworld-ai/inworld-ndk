import os
import subprocess

InworldRepo = "C:/Projects/inworld/inworld/"
CurDir = os.getcwd() + "/"
ProtocPath = CurDir + "build/ThirdParty/grpc/third_party/protobuf/Release/protoc.exe"
CppPluginPath = CurDir + "build/ThirdParty/grpc/Release/grpc_cpp_plugin.exe"

def AddLineToFileStart(Filename, Line):
    print("Adding " + Line + " to " + Filename)
    with open(Filename, 'r+') as File:
        Content = File.read()
        ContentStart = Content[0:len(Line)]
        if Line != ContentStart:
            File.seek(0, 0)
            File.write(Line.rstrip('\r\n') + '\n' + Content)

def Generate(Path, Filepath):
    print("------------------------------------------")

    CmdLine = ProtocPath
    
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/api-common-protos"
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/grpc/third_party/protobuf/src"
    CmdLine += " --proto_path=" + Path

    OutPath = CurDir + "src/proto"

    CmdLine += " --grpc_out=" + OutPath
    CmdLine += " --cpp_out=" + OutPath
    CmdLine += " --plugin=protoc-gen-grpc=" + CppPluginPath

    CmdLine += " " + Path + "/" + Filepath
     
    print(CmdLine)
    subprocess.call(CmdLine, shell=True)

    OutFilename = OutPath + "/" + Filepath
    OutFilename = OutFilename[0:OutFilename.rfind('.')]
    AddLineToFileStart(OutFilename + ".pb.cc", "#include \"ProtoDisableWarning.h\"")
    AddLineToFileStart(OutFilename + ".grpc.pb.cc", "#include \"ProtoDisableWarning.h\"")
    
    print("------------------------------------------")

Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "world-engine.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "packets.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "voices.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "options.proto")

Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/apikeys.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/behavioral_contexts.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/characters.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/errors.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/scenes.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/tokens.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/users.proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto", "ai/inworld/studio/v1alpha/workspaces.proto")

Generate(CurDir + "ThirdParty/api-common-protos", "google/api/annotations.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/api/client.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/api/field_behavior.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/api/http.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/api/resource.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/longrunning/operations.proto")
Generate(CurDir + "ThirdParty/api-common-protos", "google/rpc/status.proto")