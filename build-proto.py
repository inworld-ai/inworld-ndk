import os
import subprocess

InworldRepo = "C:/Projects/inworld/inworld/"
CurDir = os.getcwd() + "/"
ProtocPath = CurDir + "/build/ThirdParty/grpc/third_party/protobuf/Release/protoc.exe"
CppPluginPath = CurDir + "/build/ThirdParty/grpc/Release/grpc_cpp_plugin.exe"

def AddLineToFileStart(Filename, Line):
    print("Adding " + Line + " to " + Filename)
    with open(Filename, 'r+') as File:
        Content = File.read()
        File.seek(0, 0)
        File.write(Line.rstrip('\r\n') + '\n' + Content)

def Generate(File, OutDir):
    print("------------------------------------------")

    CmdLine = ProtocPath
    
    LastSlashIdx = File.rfind('/')
    CmdLine += " --proto_path=" + File[0:LastSlashIdx]
      
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/api-common-protos"
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/grpc/third_party/googleapis"
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/grpc/third_party/protobuf/src"
    CmdLine += " --proto_path=" + InworldRepo + "grpc-stub/world-engine/src/main/proto"

    OutPath = CurDir + OutDir

    CmdLine += " --grpc_out=" + OutPath
    CmdLine += " --cpp_out=" + OutPath
    CmdLine += " --plugin=protoc-gen-grpc=" + CppPluginPath

    CmdLine += " " + File
     
    print(CmdLine)
    subprocess.call(CmdLine, shell=True)

    DotIdx = File.rfind('.')
    Filename = File[LastSlashIdx+1:DotIdx]

    OutPath2 = OutPath + "/"
    AddLineToFileStart(OutPath2 + Filename + ".pb.cc", "#include \"ProtoDisableWarning.h\"")
    AddLineToFileStart(OutPath2 + Filename + ".grpc.pb.cc", "#include \"ProtoDisableWarning.h\"")
    
    print("------------------------------------------")

Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/world-engine.proto", "src/proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/packets.proto", "src/proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/voices.proto", "src/proto")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/options.proto", "src/proto")

Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/apikeys.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/behavioral_contexts.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/characters.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/errors.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/scenes.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/tokens.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/users.proto", "src/proto/ai/inworld/studio/v1alpha")
Generate(InworldRepo + "grpc-stub/world-engine/src/main/proto/ai/inworld/studio/v1alpha/workspaces.proto", "src/proto/ai/inworld/studio/v1alpha")

Generate(CurDir + "ThirdParty/api-common-protos/google/api/annotations.proto", "src/proto/google/api")
Generate(CurDir + "ThirdParty/api-common-protos/google/api/client.proto", "src/proto/google/api")
Generate(CurDir + "ThirdParty/api-common-protos/google/api/field_behavior.proto", "src/proto/google/api")
Generate(CurDir + "ThirdParty/api-common-protos/google/api/http.proto", "src/proto/google/api")
Generate(CurDir + "ThirdParty/api-common-protos/google/api/resource.proto", "src/proto/google/api")