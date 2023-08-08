import os
import subprocess

CurDir = os.getcwd() + "/"
NdkRootPath = CurDir + "../../../"
GrpcBuildPath = NdkRootPath + "build/ThirdParty/grpc/"
ProtocPath = GrpcBuildPath + "third_party/protobuf/Release/protoc.exe"
CppPluginPath = GrpcBuildPath + "Release/grpc_cpp_plugin.exe"
ProtoIncludePath = NdkRootPath + "ThirdParty/grpc/third_party/protobuf/src"

def Generate(Filename):
    print("------------------------------------------")

    CmdLine = ProtocPath
    CmdLine += " --proto_path=" + CurDir
    CmdLine += " --proto_path=" + ProtoIncludePath
    #CmdLine += " --grpc_out=" + CurDir
    CmdLine += " --cpp_out=" + CurDir
    CmdLine += " --plugin=protoc-gen-grpc=" + CppPluginPath
    CmdLine += " " + CurDir + Filename
     
    print(CmdLine)
    subprocess.call(CmdLine, shell=True)
    
    print("------------------------------------------")

Generate("inworld_ndkData.proto")