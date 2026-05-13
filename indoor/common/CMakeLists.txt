include_directories(
    ${PROJECT_PATH}3rdparty/libonvif-3.1.10/libonvif/include/
)

file(GLOB_RECURSE SOURCES ./*.c)

add_library(common ${SOURCES})
