import os
import sys
import subprocess

ProtoRepo = "C:/Projects/inworld/inworld-proto/"

CurDir = os.getcwd() + "/"
ProtocPath = CurDir + "build/ThirdParty/grpc/third_party/protobuf/Release/protoc.exe"
CppPluginPath = CurDir + "build/ThirdParty/grpc/Release/grpc_cpp_plugin.exe"
CommonProtoPath = CurDir + "ThirdParty/api-common-protos/"

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
    
    CmdLine += " --proto_path=" + CommonProtoPath
    CmdLine += " --proto_path=" + CurDir + "ThirdParty/grpc/third_party/protobuf/src"
    CmdLine += " --proto_path=" + Path

    OutPath = CurDir + "src/proto"

    CmdLine += " --grpc_out=" + OutPath
    CmdLine += " --cpp_out=" + OutPath
    CmdLine += " --plugin=protoc-gen-grpc=" + CppPluginPath

    CmdLine += " " + Path + "/" + Filepath
     
    print(CmdLine)
    tokens = CmdLine.split()
    process = subprocess.Popen(tokens, stdout=sys.stdout, stderr=subprocess.STDOUT)
    res = process.communicate()
    if process.returncode != 0:
        raise subprocess.CalledProcessError(process.returncode, process.args)

    OutFilename = OutPath + "/" + Filepath
    OutFilename = OutFilename[0:OutFilename.rfind('.')]
    AddLineToFileStart(OutFilename + ".pb.cc", "#include \"ProtoDisableWarning.h\"")
    AddLineToFileStart(OutFilename + ".grpc.pb.cc", "#include \"ProtoDisableWarning.h\"")
    
    print("------------------------------------------")

ProtoPath = ProtoRepo + "grpc-stub/world-engine/src/main/proto/"
Generate(ProtoPath, "world-engine.proto")
Generate(ProtoPath, "packets.proto")
Generate(ProtoPath, "voices.proto")
Generate(ProtoPath, "options.proto")

Generate(ProtoPath, "ai/inworld/studio/v1alpha/apikeys.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/behavioral_contexts.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/characters.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/errors.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/scenes.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/tokens.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/users.proto")
Generate(ProtoPath, "ai/inworld/studio/v1alpha/workspaces.proto")

Generate(ProtoRepo, "grpc-stub/platform-public/src/main/proto/ai/inworld/engine/v1/state_serialization.proto")

Generate(CommonProtoPath, "google/api/annotations.proto")
Generate(CommonProtoPath, "google/api/client.proto")
Generate(CommonProtoPath, "google/api/field_behavior.proto")
Generate(CommonProtoPath, "google/api/http.proto")
Generate(CommonProtoPath, "google/api/resource.proto")
Generate(CommonProtoPath, "google/longrunning/operations.proto")
Generate(CommonProtoPath, "google/rpc/status.proto")

print("Great success!")