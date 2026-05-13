cmake_minimum_required(VERSION 3.22.1)

set(PROJECT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/)

# debug release
set(BUILD_TYPE "release") 
# t40n x86_64 ssd20x anyka
set(PLATFORM_TYPE "anyka")


if(PLATFORM_TYPE STREQUAL "x86_64")
    message(STATUS "Building in platform:x86_64-ubuntu") 
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")

    SET(CMAKE_C_FLAGS_DEBUG "-fPIE")
    SET(CMAKE_CXX_FLAGS_DEBUG "-fPIE")
    SET(CMAKE_C_FLAGS_RELEASE "-fPIE")
    SET(CMAKE_CXX_FLAGS_RELEASE "-fPIE")

    add_definitions(
        -DPLATFORM_TYPE_X86_64
        -DLV_RESOURCE_PATH="/home/wxj/doorbell_sdk/indoor/resource"
        -DTUYA_CACHE_PATH="/tmp/tuya_cache/"
    )
elseif(PLATFORM_TYPE STREQUAL "ssd20x")
    message(STATUS "Building in platform:ssd20x.") 
    set(CMAKE_C_COMPILER "arm-linux-gnueabihf-gcc")
    set(CMAKE_CXX_COMPILER "arm-linux-gnueabihf-g++")
   
    SET(CMAKE_C_FLAGS_DEBUG "-march=armv7-a")
    SET(CMAKE_CXX_FLAGS_DEBUG "-march=armv7-a")
    SET(CMAKE_C_FLAGS_RELEASE "-march=armv7-a")
    SET(CMAKE_CXX_FLAGS_RELEASE "-march=armv7-a")

    add_definitions(
        -DPLATFORM_TYPE_SSD20X
        -DLV_RESOURCE_PATH="/app/resource"
    )
elseif(PLATFORM_TYPE STREQUAL "anyka")
    message(STATUS "Building in platform:anyka.") 
    set(CMAKE_C_COMPILER "arm-anykav500-linux-uclibcgnueabi-gcc")
    set(CMAKE_CXX_COMPILER "arm-anykav500-linux-uclibcgnueabi-g++")
   
    # SET(CMAKE_C_FLAGS_DEBUG "-march=armv7-a")
    # SET(CMAKE_CXX_FLAGS_DEBUG "-march=armv7-a")
    # SET(CMAKE_C_FLAGS_RELEASE "-march=armv7-a")
    # SET(CMAKE_CXX_FLAGS_RELEASE "-march=armv7-a")

    add_definitions(
        -DPLATFORM_TYPE_ANYKA
        -DLV_RESOURCE_PATH="/tmp/tf"
    )
else()
    message(FATAL_ERROR "unsupported build type:${PLATFORM_TYPE}")

endif()

set(PROJECT_NAME DOORBELL_${PLATFORM_TYPE}_${BUILD_TYPE}.BIN)
project(${PROJECT_NAME})

if(BUILD_TYPE STREQUAL "debug")
    message(STATUS "Building in Debug mode.")  
    SET(CMAKE_BUILD_TYPE "Debug")
    #-fsanitize=address 检测各种内存访问错误
    #-fsanitize=leak 专门检测内存泄漏
    #-fsanitize=undefined 检测未定义行为
    #-fsanitize=thread 检测数据竞争
    #-fno-omit-frame-pointer 强制编译器保留帧指针，确保生成更可靠的调用堆栈信息
    #-fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free 禁用编译器优化内存接口
    SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}     -fsanitize=address -O0 -g -Wall -std=gnu11 -Werror -fno-omit-frame-pointer -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -fno-common -Wno-unused-result -Wno-format -Wno-bool-compare")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -O0 -g -Wall -std=c++1y -Werror -fno-omit-frame-pointer -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free -fno-common -Wno-unused-result")

elseif(BUILD_TYPE STREQUAL "release")
    message(STATUS "Building in Release mode.")  
    SET(CMAKE_BUILD_TYPE "Release") 
    SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}     -O2 -s -Wall -std=gnu11 -Werror -Wno-unused-result -w")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -s -Wall -std=c++1y -Werror -Wno-unused-result -w") 

else()
    message(FATAL_ERROR "unsupported build type:${BUILD_TYPE}")

endif()

add_definitions(
    -DSYSTEM_VERSION="2.1.12"
    -DAPP_TUYA_PID="egyq7uvexpiim6pd"
)

include_directories(
    # ${PROJECT_PATH}include/
    # ${PROJECT_PATH}include/anyka
    # ${PROJECT_PATH}include/common
    # ${PROJECT_PATH}include/tuya
    # ${PROJECT_PATH}share/
    # ${PROJECT_PATH}share/include
    # ${PROJECT_PATH}share/include/lvgl/drive
    # ${PROJECT_PATH}share/include/lvgl
    # ${PROJECT_PATH}share/xls
    ${PROJECT_PATH}common/
    ${PROJECT_PATH}sources/
    ${PROJECT_PATH}driver/
    ${PROJECT_PATH}3rdparty/lvgl-8.4.0/
    ${PROJECT_PATH}3rdparty/live/custom/
    ${PROJECT_PATH}3rdparty/openh264-2.4.1/custom/
    ${PROJECT_PATH}3rdparty/freetype-2.10.0/include
    ${PROJECT_PATH}3rdparty/xls/
    ${PROJECT_PATH}3rdparty/minimp4/
    ${PROJECT_PATH}3rdparty/tuya-6.2.6/
    ${PROJECT_PATH}3rdparty/C-Thread-Pool/
    ${PROJECT_PATH}3rdparty/libiconv/include/

    # ${PROJECT_PATH}includes/tuya/components/lib_tls/include/
    # ${PROJECT_PATH}includes/tuya/base/include/
    ${PROJECT_PATH}3rdparty/tuya-6.2.6/tuyaos_adapter/include/utilities/include/
    ${PROJECT_PATH}includes/tuya/components/svc_tuya_cloud/include/tls

    ${PROJECT_PATH}includes/tuya/
    ${PROJECT_PATH}includes/tuya/base/include/
    ${PROJECT_PATH}includes/tuya/components/tal_system/include/
    ${PROJECT_PATH}includes/tuya/components/tal_system_service/include/
    ${PROJECT_PATH}includes/tuya/components/svc_api_ipc/include/
    ${PROJECT_PATH}includes/tuya/components/svc_wifi/include/
    ${PROJECT_PATH}includes/tuya/components/svc_devos/include/
    ${PROJECT_PATH}includes/tuya/components/lib_cJSON/include/
    ${PROJECT_PATH}includes/tuya/components/svc_netmgr/include/
    ${PROJECT_PATH}includes/tuya/components/base_media/include/
    ${PROJECT_PATH}includes/tuya/components/svc_ipc_core/include/
    ${PROJECT_PATH}includes/tuya/components/svc_streaming_p2p/include/
    ${PROJECT_PATH}includes/tuya/components/svc_video_msg/include/
    ${PROJECT_PATH}includes/tuya/components/svc_cloud_storage/include/
    ${PROJECT_PATH}includes/tuya/components/tal_security/include/
    ${PROJECT_PATH}includes/tuya/components/svc_local_storage/include/
    ${PROJECT_PATH}includes/tuya/components/svc_ring_buffer/include/
    ${PROJECT_PATH}includes/tuya/components/svc_rule/include/
    ${PROJECT_PATH}includes/tuya/components/svc_ipc_tmm_control/include/
    ${PROJECT_PATH}includes/tuya/components/svc_api_iot/include/
    ${PROJECT_PATH}includes/tuya/components/svc_tmm_link/include/
    ${PROJECT_PATH}includes/tuya/components/svc_ptz/include/
    ${PROJECT_PATH}includes/tuya/components/tal_media/include/
    ${PROJECT_PATH}includes/tuya/components/lib_tls/include/
    ${PROJECT_PATH}includes/tuya/components/svc_api_wired/include/
    ${PROJECT_PATH}includes/tuya/components/svc_wired/include/
    ${PROJECT_PATH}includes/tuya/components/tal_wired/include/
    ${PROJECT_PATH}includes/tuya/components/tal_system_service/include/utilities/
    ${PROJECT_PATH}includes/tuya/components/svc_tuya_cloud/include/
    ${PROJECT_PATH}includes/tuya/components/svc_tuya_cloud/include/iot_dns/
    ${PROJECT_PATH}includes/tuya/components/tal_network/include/
    ${PROJECT_PATH}includes/tuya/components/tal_wifi/include/
)

if(PLATFORM_TYPE STREQUAL  "x86_64")

elseif(PLATFORM_TYPE STREQUAL  "ssd20x")

elseif(PLATFORM_TYPE STREQUAL  "anyka")
    include_directories(
        ${PROJECT_PATH}includes/
    )
endif()

# 添加src目录
aux_source_directory(${PROJECT_PATH}sources/ DIR_SRCS)
aux_source_directory(${PROJECT_PATH}sources/user/ DIR_SRCS)
# aux_source_directory(${PROJECT_PATH}sources/layout/ DIR_SRCS)
aux_source_directory(${PROJECT_PATH}sources/platform/ DIR_SRCS)

add_subdirectory(common)
add_subdirectory(3rdparty)
add_subdirectory(driver)

set(VAR ${VAR} lvgl)
set(VAR ${VAR} lvgl_demos)
set(VAR ${VAR} common)
set(VAR ${VAR} driver)
set(VAR ${VAR} live555)
set(VAR ${VAR} openh264)
set(VAR ${VAR} minimp4)
set(VAR ${VAR} freetype)
set(VAR ${VAR} xls)
set(VAR ${VAR} iconv)
set(VAR ${VAR} tuya)
set(VAR ${VAR} thpool)
message("VAR : ${VAR}")
add_executable(${PROJECT_NAME} ${DIR_SRCS})

target_link_libraries(${PROJECT_NAME} ${VAR})

if(PLATFORM_TYPE STREQUAL  "x86_64")

    target_link_libraries(${PROJECT_NAME} -Wl,--start-group
    ${VAR} 
    m
	pthread
	dl
    stdc++
	rt 
    atomic
    ${PROJECT_PATH}librarys/tuya/${PLATFORM_TYPE}/libtuyaos.a
    -lSDL2
    -lasound
    -lx264
    -lx265
    -lswscale
    -lavutil
    -lavcodec
    -lavformat
    -lswresample
    -ldrm
    -lz

    -Wl,--gc-sections
    -Wl,--end-group)

elseif(PLATFORM_TYPE STREQUAL  "ssd20x")

    target_link_libraries(${PROJECT_NAME} -Wl,--start-group
    ${VAR} 
    m
	pthread
	dl
    stdc++
	rt 
    ${PROJECT_PATH}librarys/ssd20x/libmi_sys.so
	${PROJECT_PATH}librarys/ssd20x/libmi_gfx.so
	${PROJECT_PATH}librarys/ssd20x/libmi_panel.so
	${PROJECT_PATH}librarys/ssd20x/libmi_disp.so
	${PROJECT_PATH}librarys/ssd20x/libmi_ao.so
	${PROJECT_PATH}librarys/ssd20x/libmi_ai.so
	${PROJECT_PATH}librarys/ssd20x/libmi_common.so
	${PROJECT_PATH}librarys/ssd20x/libmi_vdec.so
	${PROJECT_PATH}librarys/ssd20x/libmi_divp.so
	${PROJECT_PATH}librarys/ssd20x/libmi_venc.so

    ${PROJECT_PATH}librarys/tuya/${PLATFORM_TYPE}/libtuyaos.a
    -Wl,--gc-sections
    -Wl,--end-group)

elseif(PLATFORM_TYPE STREQUAL  "anyka")

    target_link_libraries(${PROJECT_NAME} -Wl,--start-group
    ${VAR} 
    m
	pthread
	dl
    stdc++
	rt 
    ${PROJECT_PATH}librarys/anyka/libplat_tde.so
    ${PROJECT_PATH}librarys/anyka/libplat_common.so
    ${PROJECT_PATH}librarys/anyka/libplat_thread.so
    ${PROJECT_PATH}librarys/anyka/libplat_mem.so
    ${PROJECT_PATH}librarys/anyka/libplat_osal.so
    ${PROJECT_PATH}librarys/anyka/libplat_log.so
    ${PROJECT_PATH}librarys/anyka/libplat_dbg.so
    ${PROJECT_PATH}librarys/anyka/libmpi_vdec.so
    ${PROJECT_PATH}librarys/anyka/libakv_decode.so
    ${PROJECT_PATH}librarys/anyka/libmpi_venc.so
    ${PROJECT_PATH}librarys/anyka/libakv_encode.so
    ${PROJECT_PATH}librarys/anyka/libplat_ao.so
    ${PROJECT_PATH}librarys/anyka/libplat_vqe.so
    ${PROJECT_PATH}librarys/anyka/libakaudiofilter.so
    ${PROJECT_PATH}librarys/anyka/libplat_ai.so

    ${PROJECT_PATH}librarys/tuya/${PLATFORM_TYPE}/libtuyaos.a
    -Wl,--gc-sections
    -Wl,--end-group)

endif()
