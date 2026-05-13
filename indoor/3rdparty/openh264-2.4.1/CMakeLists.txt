add_compile_options(-w)

# add_definitions(
# )

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/codec/decoder/plus/inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/decoder/core/inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/common/inc/
    ${CMAKE_CURRENT_SOURCE_DIR}/codec/api/wels/
)

FILE(GLOB OPENH264_SRC
    custom/openh264_decoder.cpp

    codec/decoder/plus/src/welsDecoderExt.cpp
    codec/decoder/core/src/wels_decoder_thread.cpp
    codec/common/src/WelsThreadLib.cpp
    codec/decoder/core/src/manage_dec_ref.cpp
    codec/common/src/utils.cpp
    codec/common/src/expand_pic.cpp
    codec/decoder/core/src/pic_queue.cpp
    codec/common/src/memory_align.cpp
    codec/common/src/crt_util_safe_x.cpp
    codec/common/src/welsCodecTrace.cpp
    codec/decoder/core/src/decoder_core.cpp
    codec/decoder/core/src/error_concealment.cpp
    codec/decoder/core/src/rec_mb.cpp
    codec/decoder/core/src/decoder_data_tables.cpp
    codec/common/src/common_tables.cpp
    codec/common/src/copy_mb.cpp
    codec/decoder/core/src/au_parser.cpp
    codec/decoder/core/src/fmo.cpp
    codec/decoder/core/src/bit_stream.cpp
    codec/decoder/core/src/memmgr_nal_unit.cpp
    codec/decoder/core/src/decoder.cpp
    codec/decoder/core/src/decode_slice.cpp
    codec/common/src/deblocking_common.cpp
    codec/decoder/core/src/mv_pred.cpp
    codec/decoder/core/src/parse_mb_syn_cabac.cpp
    codec/decoder/core/src/parse_mb_syn_cavlc.cpp
    codec/decoder/core/src/deblocking.cpp
    codec/decoder/core/src/cabac_decoder.cpp
    codec/decoder/core/src/decode_mb_aux.cpp
    codec/decoder/core/src/get_intra_predictor.cpp
    codec/common/src/mc.cpp
    codec/common/src/cpu.cpp
)

add_library(openh264 STATIC ${OPENH264_SRC})