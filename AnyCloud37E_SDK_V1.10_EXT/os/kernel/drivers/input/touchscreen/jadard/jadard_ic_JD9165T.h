/* SPDX-License-Identifier: GPL-2.0+
 *
 * Jadard Touch IC driver
 *
 * Copyright (c) 2018-2026 Jadard Technology Corporation.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation,  and
 * may be copied, distributed,  and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef JADARD_JD9165T_H
#define JADARD_JD9165T_H

static int jd9165t_EnterBackDoor(uint16_t *pRomID);
static int jd9165t_ExitBackDoor(void);

#define JD9165T_ID                          (0x9085)
#define JD9165T_OUTER_ID                    "JD9165T"
#define JD9165T_MEMORY_ADDR_PRAM            (0x00000000)
#define JD9165T_MEMORY_ADDR_DRAM            (0x20000000)
#define JD9165T_MEMORY_ADDR_ARAM            (0x20010000)
#define JD9165T_MEMORY_ADDR_ERAM            (0x20011000)
#define JD9165T_MEMORY_ARAM_L_SIZE          (320)
#define JD9165T_MEMORY_ERAM_SIZE            (4 * 1024)
#define JD9165T_MAX_DSRAM_NUM               (25)
#define JD9165T_MAX_ESRAM_NUM               (10)
#define JD9165T_SECTION_INFO_READY_VALUE    (0xA55A)
#define JD9165T_OUTPUT_DATA_HANDSHAKE_DONE (0xA55A)
#define JD9165T_SIZE                        (90 * 1024)
#define JD9165T_PRAM_MAX_SIZE               (0xE000)
/* Set BIN_FW_CID_VER & BIN_FW_VER address */
#define JD9165T_BIN_FW_HEADER_SIZE          (0x200)
/* Please reference .flash_0_fw_cfg_info of FW */
#define JD9165T_BIN_FW_CFG_INFO_ADDR        (JD9165T_BIN_FW_HEADER_SIZE + 0x10000)
#define JD9165T_BIN_FW_CID_VER              (JD9165T_BIN_FW_CFG_INFO_ADDR + 0)
#define JD9165T_BIN_FW_VER                  (JD9165T_BIN_FW_CFG_INFO_ADDR + 7)
#define JD9165T_DATA_TIMEOUT                (5000)
#define JD9165T_DDREG_MODE                  (JD_DDREG_MODE_1)

struct JD9165T_MOVE_INFO {
	uint8_t mov_mem_cmd;
	uint32_t fl_st_addr;
	uint32_t fl_len;
	uint32_t to_mem_st_addr;
	uint16_t to_mem_crc;
};

struct JD9165T_INFO_CONTENT {
	uint32_t info_content_addr;
	uint32_t info_content_len;
};

struct JD9165T_DSRAM_HOST_ADDR {
	uint32_t fw_cid_version;
	uint32_t fw_version;
	uint32_t game_mode_en;
	uint32_t usb_en;
	uint32_t gesture_en;
	uint32_t high_sensitivity_en;
	uint32_t border_en;
	uint32_t proximity_en;
	uint32_t panel_maker;
	uint32_t panel_version;
	uint32_t earphone_en;
#ifdef CONFIG_TOUCHSCREEN_JADARD_SORTING
	uint32_t mpap_pw;
	uint32_t mpap_handshake;
	uint32_t mpap_keep_frame;
	uint32_t mpap_skip_frame;
	uint32_t mpap_mux_switch;
	uint32_t fw_dbic_en;
	uint32_t mpap_diff_max;
	uint32_t mpap_diff_min;
	uint32_t mpap_pw_sync;
	uint32_t mpap_error_msg;
#endif
#ifdef JD_RAWDATA_V2
	uint32_t data_output_v2;
#endif
};

struct JD9165T_DSRAM_DEBUG_ADDR {
	uint32_t output_data_addr;
	uint32_t output_data_sel;
	uint32_t output_data_handshake;
	uint32_t freq_band;
};

struct JD9165T_ESRAM_INFO_CONTENT_ADDR {
	uint32_t coordinate_report;
};

struct JD9165T_CHIP_INFO {
	bool back_door_mode;
	struct JD9165T_DSRAM_HOST_ADDR dsram_host_addr;
	struct JD9165T_DSRAM_DEBUG_ADDR dsram_debug_addr;
	struct JD9165T_ESRAM_INFO_CONTENT_ADDR esram_info_content_addr;
} g_jd9165t_chip_info;

struct JD9165T_SECTION_INFO {
	bool section_info_ready;
	uint32_t section_info_ready_addr;
	/* Dsram section */
	uint32_t dsram_num_start_addr;
	uint32_t dsram_section_info_start_addr;
	/* Esram section */
	uint32_t esram_num_start_addr;
	uint32_t esram_section_info_start_addr;
} g_jd9165t_section_info;

enum JD9165T_DSRAM_SECTION_INFO_ORDER {
	JD9165T_DSRAM_FW_CFG_INFO = 0,
	JD9165T_DSRAM_TOUCH_CFG,
	JD9165T_DSRAM_SORTING_CFG,
	JD9165T_DSRAM_HOST,
	JD9165T_DSRAM_DEBUG,
	JD9165T_DSRAM_ADC_MAPPING_TABLE,
	JD9165T_DSRAM_AFE_CONFIG,
	JD9165T_DSRAM_FW_PAYLOAD,
	JD9165T_DSRAM_INTERNAL_INFO,
	JD9165T_DSRAM_CB_1CYC,
	JD9165T_DSRAM_CB_F0,
	JD9165T_DSRAM_CB_F1,
	JD9165T_DSRAM_CB_F2,
	JD9165T_DSRAM_CB_LPWUG_1CYC,
	JD9165T_DSRAM_CB_LPWUG_F0,
	JD9165T_DSRAM_MP_DIFF_MAX,
	JD9165T_DSRAM_MP_DIFF_MIN,
	JD9165T_REPAIR_MODE_SWITCH = JD9165T_DSRAM_MP_DIFF_MIN,
};

enum JD9165T_ESRAM_SECTION_INFO_ORDER {
	JD9165T_ESRAM_ALL_SECTION_INFO = 0,
	JD9165T_ESRAM_COORDINATE_REPORT,
	JD9165T_ESRAM_OUTPUT_BUF,
	JD9165T_ESRAM_DEBUG_BUF,
	JD9165T_ESRAM_FLASH_DATA_BUF,
	JD9165T_ESRAM_DDREG_DATA_BUF,
};

enum JD9165T_HEADER_INFO {
	JD9165T_HW_HEADER_MAX_SIZE          = JD9165T_BIN_FW_HEADER_SIZE,
	JD9165T_HW_HEADER_CRC_NUMBER        = 2,
	JD9165T_HW_HEADER_INITIAL_NUMBER    = 3,
	JD9165T_HW_HEADER_MOVE_INFO_NUMBER = 12,
	JD9165T_HW_HEADER_PSRAM_START_ADDR = JD9165T_MEMORY_ADDR_PRAM,
};

enum JD9165T_HEADER_POSITION {
	JD9165T_HW_HEADER_CRC_H = 0,
	JD9165T_HW_HEADER_CRC_L,
	JD9165T_CRC_FAIL_RETRY_TIMES,
	JD9165T_HW_HEADER_LEN,
	JD9165T_LOAD_CODE_SPI_FREQ,
};

enum JD9165T_FLASH_PACK {
	JD9165T_FLASH_PACK_READ_SIZE  = 2048,
	JD9165T_FLASH_PACK_WRITE_SIZE = 2048,
};

enum JD9165T_TIMER_REG_ADDR {
	JD9165T_TIMER_BASE_ADDR           = 0x400000,
	JD9165T_TIMER_REG_ADDR_WDT_CONFIG = (JD9165T_TIMER_BASE_ADDR << 8) + 0x32,
	JD9165T_TIMER_REG_ADDR_RTC_CONFIG = (JD9165T_TIMER_BASE_ADDR << 8) + 0x42,
};

enum JD9165T_TIMER_RELATED_SETTING {
	/* 0x32[0] */
	JD9165T_TIMER_RELATED_SETTING_WDT_STOP = 0x02,
	/* 0x42[0] */
	JD9165T_TIMER_RELATED_SETTING_DISABLE_RTC_RUN = 0x00,
	JD9165T_TIMER_RELATED_SETTING_ENABLE_RTC_RUN = 0x01,
};

enum JD9165T_DMA_RELATED_SETTING {
	/* 0x0C[4:0] */
	JD9165T_DMA_RELATED_SETTING_READ_FROM_FLASH           = 0x01,
	JD9165T_DMA_RELATED_SETTING_WRITE_TO_PRAM             = 0x03,
	JD9165T_DMA_RELATED_SETTING_WRITE_TO_FLASH            = 0x07,
	JD9165T_DMA_RELATED_SETTING_READ_FLASH_STATUS         = 0x08,
	JD9165T_DMA_RELATED_SETTING_PAGE_PROGRAM_FLASH        = 0x10,
	/* 0x0D[1] */
	JD9165T_DMA_RELATED_SETTING_DMA_ABORT                 = 0x01,
	/* 0x0C[4] */
	JD9165T_DMA_RELATED_SETTING_PAGE_PROGRAM_DONE         = 0x00,
	/* 0x0E[1:0] */
	JD9165T_DMA_RELATED_SETTING_TRANSFER_DATA_1_BYTE_MODE = 0x00,
	JD9165T_DMA_RELATED_SETTING_TRANSFER_DATA_2_BYTE_MODE = 0x01,
	JD9165T_DMA_RELATED_SETTING_TRANSFER_DATA_4_BYTE_MODE = 0x02,
	/* 0x1D[0] */
	JD9165T_DMA_RELATED_SETTING_DMA_DONE                  = 0x00,
	JD9165T_DMA_RELATED_SETTING_DMA_BUSY                  = 0x01,
};

enum JD9165T_SOC_REG_ADDR {
	JD9165T_SOC_BASE_ADDR                = 0x400080,
	JD9165T_SOC_REG_ADDR_RGU_0           = (JD9165T_SOC_BASE_ADDR << 8) + 0x04,
	JD9165T_SOC_REG_ADDR_CPU_SOFT_RESET  = (JD9165T_SOC_BASE_ADDR << 8) + 0x12,
	JD9165T_SOC_REG_ADDR_CPU_STATUS      = (JD9165T_SOC_BASE_ADDR << 8) + 0x13,
	JD9165T_SOC_REG_ADDR_PRAM_PROG       = (JD9165T_SOC_BASE_ADDR << 8) + 0x14,
	JD9165T_SOC_REG_ADDR_CPU_CLK_STOP    = (JD9165T_SOC_BASE_ADDR << 8) + 0x16,
	JD9165T_SOC_REG_ADDR_SLPOUT_INT_EN   = (JD9165T_SOC_BASE_ADDR << 8) + 0x44,
	JD9165T_SOC_REG_ADDR_SLPOUT          = (JD9165T_SOC_BASE_ADDR << 8) + 0x45,
	JD9165T_SOC_REG_ADDR_CHIP_ID0        = (JD9165T_SOC_BASE_ADDR << 8) + 0x74,
	JD9165T_SOC_REG_ADDR_CHIP_ID1        = (JD9165T_SOC_BASE_ADDR << 8) + 0x75,
	JD9165T_SOC_REG_ADDR_CHIP_ID2        = (JD9165T_SOC_BASE_ADDR << 8) + 0x76,
	JD9165T_SOC_REG_ADDR_CHIP_ID3        = (JD9165T_SOC_BASE_ADDR << 8) + 0x77,
	JD9165T_SOC_REG_ADDR_POR_INIT        = (JD9165T_SOC_BASE_ADDR << 8) + 0x81,
};

enum JD9165T_SOC_PASSWORD {
	JD9165T_SOC_PASSWORD_SOC_RESET       = 0xA5,
	JD9165T_SOC_PASSWORD_MCU_RESET       = 0xA5,
	JD9165T_SOC_PASSWORD_STOP_MCU        = 0x00,
	JD9165T_SOC_PASSWORD_START_MCU       = 0xF1,
	JD9165T_SOC_PASSWORD_STOP_MCU_CLOCK  = 0x5A,
	JD9165T_SOC_PASSWORD_START_MCU_CLOCK = 0xA5,
	JD9165T_SOC_PASSWORD_POR_CLEAR       = 0x00,
};

enum JD9165T_STC1_REG_ADDR {
	JD9165T_STC1_BASE_ADDR              = 0x400081,
	JD9165T_STC1_REG_ADDR_AFE_SCAN_CTRL = (JD9165T_STC1_BASE_ADDR << 8) + 0x1A,
	JD9165T_STC1_REG_ADDR_SCAN_STATUS   = (JD9165T_STC1_BASE_ADDR << 8) + 0x87,
};

enum JD9165T_STC1_RELATED_SETTING {
	/* 0x1A[0] */
	JD9165T_STC1_RELATED_SETTING_STC_SCAN_DISABLE   = 0x00,
	JD9165T_STC1_RELATED_SETTING_STC_SCAN_ENABLE    = 0x01,
	/* 0x87[0] */
	JD9165T_STC1_RELATED_SETTING_SCAN_DONE          = 0x00,
	JD9165T_STC1_RELATED_SETTING_SCAN_BUSY          = 0x01,
};

#endif
