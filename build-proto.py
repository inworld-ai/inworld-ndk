import os
import sys
import subprocess

PROTO_REPO = "C:/Projects/inworld/inworld-proto/"
CURRENT_DIR = os.getcwd()
PROTOC_PATH = os.path.join(CURRENT_DIR, "build/ThirdParty/grpc/third_party/protobuf/Release/protoc.exe")
CPP_PLUGIN_PATH = os.path.join(CURRENT_DIR, "build/ThirdParty/grpc/Release/grpc_cpp_plugin.exe")
COMMON_PROTO_PATH = os.path.join(CURRENT_DIR, "ThirdParty/api-common-protos/")

def add_line_to_file_start(filename, line):
    print(f"Adding {line} to {filename}")
    with open(filename, 'r+') as file:
        content = file.read()
        content_start = content[:len(line)]
        if line != content_start:
            file.seek(0, 0)
            file.write(line.rstrip('\r\n') + '\n' + content)

def replace_protobuf_namespace(filename):
    print(f"Replacing protobuf namespace in file {filename}")
    with open(filename, "r+") as file:
        data = file.read()
        data = data.replace('protobuf::', 'protobuf_inworld::')
        file.seek(0)
        file.write(data)

def generate(path, filepath):
    print("------------------------------------------")

    cmd_line = PROTOC_PATH
    cmd_line += f" --proto_path={COMMON_PROTO_PATH}"
    cmd_line += f" --proto_path={os.path.join(CURRENT_DIR, 'ThirdParty/grpc/third_party/protobuf/src')}"
    cmd_line += f" --proto_path={path}"

    out_path = os.path.join(CURRENT_DIR, "src/proto")

    cmd_line += f" --grpc_out={out_path}"
    cmd_line += f" --cpp_out={out_path}"
    cmd_line += f" --plugin=protoc-gen-grpc={CPP_PLUGIN_PATH}"
    cmd_line += f" {os.path.join(path, filepath)}"
    cmd_line += f" --experimental_allow_proto3_optional"

    print(cmd_line)
    tokens = cmd_line.split()
    process = subprocess.Popen(tokens, stdout=sys.stdout, stderr=subprocess.STDOUT)
    res = process.communicate()
    if process.returncode != 0:
        raise subprocess.CalledProcessError(process.returncode, process.args)

    out_filename = os.path.join(out_path, os.path.splitext(filepath)[0])
    add_line_to_file_start(out_filename + ".pb.cc", "#include \"ProtoDisableWarning.h\"")
    add_line_to_file_start(out_filename + ".grpc.pb.cc", "#include \"ProtoDisableWarning.h\"")

    for file_extension in [".pb.h", ".pb.cc", ".grpc.pb.h", ".grpc.pb.cc"]:
        replace_protobuf_namespace(out_filename + file_extension)

    print("------------------------------------------")

proto_path = os.path.join(PROTO_REPO, "proto/")
for file_name in [
    "ai/inworld/engine/world-engine.proto",
    "ai/inworld/engine/v1/state_serialization.proto",
    "ai/inworld/packets/packets.proto",
    "ai/inworld/voices/voices.proto",
    "ai/inworld/voices/base_voice.proto",
    "ai/inworld/options/options.proto",
    "ai/inworld/engine/configuration/configuration.proto",
    "ai/inworld/language_codes/language_codes.proto",
    "ai/inworld/studio/v1alpha/apikeys.proto",
    "ai/inworld/studio/v1alpha/behavioral_contexts.proto",
    "ai/inworld/studio/v1alpha/characters.proto",
    "ai/inworld/studio/v1alpha/errors.proto",
    "ai/inworld/studio/v1alpha/scenes.proto",
    "ai/inworld/studio/v1alpha/tokens.proto",
    "ai/inworld/studio/v1alpha/users.proto",
    "ai/inworld/studio/v1alpha/impression_event_data.proto",
    "ai/inworld/studio/v1alpha/language_code.proto",
    "ai/inworld/studio/v1alpha/workspaces.proto",
    "nvidia/a2x-interface-main-proto/proto/nvidia_ace.a2x.v1.proto",
    "nvidia/a2x-interface-main-proto/proto/nvidia_ace.services.a2x.v1.proto",
    "nvidia/animation-common-main-proto/proto/nvidia_ace.animation_data.v1.proto",
    "nvidia/animation-common-main-proto/proto/nvidia_ace.animation_id.v1.proto",
    "nvidia/animation-common-main-proto/proto/nvidia_ace.audio.v1.proto",
    "nvidia/animation-common-main-proto/proto/nvidia_ace.status.v1.proto",
    "nvidia/animation-controller-interface-main-proto/proto/nvidia_ace.controller.v1.proto",
    "nvidia/animation-controller-interface-main-proto/proto/nvidia_ace.services.a2x_controller.v1.proto",
    "nvidia/animation-controller-interface-main-proto/proto/nvidia_ace.services.animation_controller.v1.proto",
    "nvidia/animation-data-interface-main-proto/proto/nvidia_ace.services.animation_data.v1.proto",
    "nvidia/ace/animation/a2x_service.proto",
    "nvidia/old/anim_controller.proto",
    "nvidia/old/anim_data.proto",
    "nvidia/old/audio_data.proto"
]:
    generate(proto_path, file_name)

for file_name in [
    "google/api/annotations.proto",
    "google/api/client.proto",
    "google/api/field_behavior.proto",
    "google/api/http.proto",
    "google/api/resource.proto",
    "google/longrunning/operations.proto",
    "google/rpc/status.proto"
]:
    generate(COMMON_PROTO_PATH, file_name)

print("Great success!")