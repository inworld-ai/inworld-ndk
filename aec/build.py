import os
import sys
import shutil
import subprocess

def CmdExec(cmdline, workdir):
    print("Cmd: ", cmdline, " in ", workdir)
    tokens = cmdline.split()
    process = subprocess.Popen(tokens, cwd=workdir, shell=True, stdout=sys.stdout, stderr=subprocess.STDOUT)
    process.wait()

def CopyFile(src, dst):
    os.chmod(dst, 777)
    shutil.copy(src, dst)

def CopyFolder(src, dst):
    if not os.path.exists(dst):
        os.mkdir(dst)
    shutil.copytree(src, dst, dirs_exist_ok=True)

RootDir = os.getcwd() + "/"
ThirdPartyDir = RootDir + "../ThirdParty/"
DepotToolsDir = ThirdPartyDir + "depot-tools/"
CheckoutDir = RootDir + "webrtc-checkout/"
CheckoutSrcDir = CheckoutDir + "src/"
NinjaDir = CheckoutSrcDir + "third_party/ninja/"

CmdExec("git restore .", DepotToolsDir)
CmdExec(DepotToolsDir + "gclient.bat", DepotToolsDir)
if not os.path.exists(CheckoutDir):
    os.mkdir(CheckoutDir)
    CmdExec(DepotToolsDir + "fetch.bat --nohooks webrtc", CheckoutDir)

CmdExec("git clean -xdf", CheckoutSrcDir)
CmdExec("git restore .", CheckoutSrcDir)
CmdExec("git checkout a261e72bc08539d8e1975b036d1b3c1e56ce2ce9", CheckoutSrcDir)

CopyFile(RootDir + "Build.gn", CheckoutSrcDir + "examples")
CopyFolder(RootDir + "aecplugin", CheckoutSrcDir + "examples/aecplugin")

CmdExec(DepotToolsDir + "gclient.bat sync", CheckoutSrcDir)

CmdExec(DepotToolsDir + "gn.bat gen out", CheckoutSrcDir)
CmdExec(NinjaDir + "ninja.exe -C out", CheckoutSrcDir)


