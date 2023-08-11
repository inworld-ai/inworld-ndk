import os
import sys
import shutil
import subprocess

Platform = sys.argv[1]
ScriptExt = ""
ExeExt = ""
if Platform == "WINDOWS":
    ScriptExt = ".bat"
    ExeExt = ".exe"
elif Platform == "MAC":
    ScriptExt = ""
    ExeExt = ""
else:
    print("Unknown platform " + Platform)
    sys.exit()

def CmdExec(cmdline, workdir):
    print("Cmd: ", cmdline, " in ", workdir)
    tokens = cmdline.split()
    process = subprocess.Popen(tokens, cwd=workdir, stdout=sys.stdout, stderr=subprocess.STDOUT)
    res = process.communicate()
    if tokens[0] != "git" and process.returncode != 0:
        print("Error: ", res)
        raise subprocess.CalledProcessError(process.returncode, process.args)

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
CmdExec("%sgclient%s"%(DepotToolsDir, ScriptExt), DepotToolsDir)
if not os.path.exists(CheckoutDir):
    os.mkdir(CheckoutDir)

if not os.path.exists(CheckoutSrcDir):
    CmdExec("%sfetch%s --nohooks webrtc"%(DepotToolsDir,ScriptExt), CheckoutDir)

CmdExec("git clean -xdf", CheckoutSrcDir)
CmdExec("git restore .", CheckoutSrcDir)
CmdExec("git checkout a261e72bc08539d8e1975b036d1b3c1e56ce2ce9", CheckoutSrcDir)

CopyFile("%sBuild.gn"%RootDir, "%sexamples"%CheckoutSrcDir)
CopyFolder("%saecplugin"%RootDir, "%sexamples/aecplugin"%CheckoutSrcDir)

CmdExec("%sgclient%s sync" % (DepotToolsDir, ScriptExt), CheckoutSrcDir)

CmdExec("%sgn%s gen out"%(DepotToolsDir, ScriptExt), CheckoutSrcDir)
CmdExec("%sninja%s -C out"%(NinjaDir, ExeExt), CheckoutSrcDir)


