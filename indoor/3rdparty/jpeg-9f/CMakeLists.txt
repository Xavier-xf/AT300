add_compile_options(-w)

# add_definitions(
# )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/
)

FILE(GLOB JPEG_SRC
    jpeg_codec.c
    jdapimin.c
    jcomapi.c
    jdinput.c
    jutils.c
    jdmarker.c
    jmemmgr.c
    jmemansi.c
    jcapimin.c
    jcmarker.c
    jcapistd.c
    jcinit.c
    jcmainct.c
    jccoefct.c
    jchuff.c
    jcarith.c
    jaricom.c
    jcdctmgr.c
    jfdctflt.c
    jfdctfst.c
    jfdctint.c
    jcprepct.c
    jcsample.c
    jccolor.c
    jcmaster.c
    jcparam.c
    jdatadst.c
    jerror.c
    jdapistd.c
    jdmaster.c
    jdmainct.c
    jdcoefct.c
    jdhuff.c
    jdarith.c
    jddctmgr.c
    jidctflt.c
    jidctfst.c
    jidctint.c
    jdpostct.c
    jdsample.c
    jdcolor.c
    jdmerge.c
    jquant2.c
    jquant1.c
    jdatasrc.c
)

add_library(jpeg STATIC ${JPEG_SRC})