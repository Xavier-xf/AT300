include_directories(
    .
)

add_library(driver STATIC 
    gui_display/driver_gui_display_${PLATFORM_TYPE}.c
    video_display/driver_video_display_${PLATFORM_TYPE}.c
    input_event/driver_input_event_${PLATFORM_TYPE}.c
    video_decoder/driver_video_decoder_${PLATFORM_TYPE}.c
    audio_decoder/driver_audio_decoder_${PLATFORM_TYPE}.c
    audio_encoder/driver_audio_encoder_${PLATFORM_TYPE}.c
    audio_output/driver_audio_output_${PLATFORM_TYPE}.c
    db_hal_driver.c
)