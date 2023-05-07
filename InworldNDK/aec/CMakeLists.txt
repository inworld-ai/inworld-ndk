cmake_minimum_required (VERSION 3.23)

project(webrtc-aec)

function(CmdExec CmdLine WorkDir)
    message("CmdExec ${CmdLine} in ${WorkDir}")
    if (WIN32)
        execute_process(
            COMMAND cmd /C ${CmdLine}
            WORKING_DIRECTORY ${WorkDir}
            RESULT_VARIABLE ErrorCode
        )
    elseif (APPLE)
        set(CmdLineList ${CmdLine})
        separate_arguments(CmdLineList)
        list(POP_FRONT CmdLineList CmdName)
        execute_process(
            COMMAND ${CmdName} ${CmdLineList}
            WORKING_DIRECTORY ${WorkDir}
            RESULT_VARIABLE ErrorCode
        )
    endif()
    if(ErrorCode)
        message(FATAL_ERROR "${CmdLine}. Error: ${ErrorCode}")
    endif()
endfunction()

set(RootDir ${CMAKE_CURRENT_SOURCE_DIR})
set(ThirdPartyDir ${RootDir}/../ThirdParty)
set(DepotToolsDir ${ThirdPartyDir}/depot-tools)
set(CheckoutDir ${RootDir}/webrtc-checkout)
set(CheckoutSrcDir ${CheckoutDir}/src)
set(NinjaDir ${CheckoutSrcDir}/third_party/ninja)

if (WIN32)
    set(ScriptExt ".bat")
    set(ExeExt ".exe")
elseif (APPLE)
    set(ScriptExt "")
    set(ExeExt "")
endif()

if (EXISTS ${CheckoutSrcDir}/out/webrtc_aec_plugin.${DynLibExt})
    message("Webrtc aec plugin exists, skip build")
    return()
endif()

message("Building webrtc aec plugin...")

#CmdExec("git config --system core.longpaths true" ${DepotToolsDir})
CmdExec("git restore ." ${DepotToolsDir})
CmdExec("${DepotToolsDir}/gclient${ScriptExt}" ${DepotToolsDir})
if (NOT EXISTS ${CheckoutDir})
    CmdExec("mkdir webrtc-checkout" ${RootDir})
    CmdExec("${DepotToolsDir}/fetch${ScriptExt} --nohooks webrtc" ${CheckoutDir})
endif()

CmdExec("git clean -xdf" ${CheckoutSrcDir})
CmdExec("git restore ." ${CheckoutSrcDir})
CmdExec("git checkout a261e72bc08539d8e1975b036d1b3c1e56ce2ce9" ${CheckoutSrcDir})

file(COPY ${RootDir}/Build.gn DESTINATION ${CheckoutSrcDir}/examples)
file(COPY ${RootDir}/aecplugin DESTINATION ${CheckoutSrcDir}/examples)

CmdExec("${DepotToolsDir}/gclient${ScriptExt} sync" ${CheckoutSrcDir})

CmdExec("${DepotToolsDir}/gn${ScriptExt} gen out" ${CheckoutSrcDir})
CmdExec("${NinjaDir}/ninja${ExeExt} -C out" ${CheckoutSrcDir})
