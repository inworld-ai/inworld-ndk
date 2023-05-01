cmake_minimum_required (VERSION 3.23)

project(webrtc-aec)

function(CmdExec CmdLine WorkDir)
    message("CmdExec ${CmdLine} in ${WorkDir}")
    execute_process(
        COMMAND cmd /C ${CmdLine}
        WORKING_DIRECTORY ${WorkDir}
        RESULT_VARIABLE error_code
    )
    if(error_code)
        message(FATAL_ERROR ${CmdLine})
    endif()
endfunction()

set(RootDir ${CMAKE_CURRENT_SOURCE_DIR})
set(DepotToolsDir ${RootDir}/../depot-tools)
set(CheckoutDir ${RootDir}/webrtc-checkout)
set(CheckoutSrcDir ${CheckoutDir}/src)
set(NinjaDir ${CheckoutSrcDir}/third_party/ninja)

if (EXISTS ${CheckoutSrcDir}/out/webrtc_aec_plugin.dll)
    message("Webrtc aec plugin exists, skip build")
    return()
endif()

message("Building webrtc aec plugin...")

CmdExec("git config --system core.longpaths true" ${DepotToolsDir})
CmdExec("git restore ." ${DepotToolsDir})
CmdExec("${DepotToolsDir}/gclient.bat" ${DepotToolsDir})
if (NOT EXISTS ${CheckoutDir})
    CmdExec("mkdir webrtc-checkout" ${RootDir})
    CmdExec("${DepotToolsDir}/fetch.bat --nohooks webrtc" ${CheckoutDir})
endif()

CmdExec("git clean -xdf" ${CheckoutSrcDir})
CmdExec("git restore ." ${CheckoutSrcDir})
CmdExec("git checkout a261e72bc08539d8e1975b036d1b3c1e56ce2ce9" ${CheckoutSrcDir})

file(COPY ${RootDir}/Build.gn DESTINATION ${CheckoutSrcDir}/examples)
file(COPY ${RootDir}/aecplugin DESTINATION ${CheckoutSrcDir}/examples)

CmdExec("${DepotToolsDir}/gclient.bat sync" ${CheckoutSrcDir})

CmdExec("${DepotToolsDir}/gn.bat gen out" ${CheckoutSrcDir})
CmdExec("${NinjaDir}/ninja.exe -C out" ${CheckoutSrcDir})
