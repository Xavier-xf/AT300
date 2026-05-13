/*
 * Copyright (c) Hisilicon Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: firmware����
 * Author: Hisilicon
 * Create: 2018-08-04
 */

/*****************************************************************************
  1 ͷ�ļ�����
*****************************************************************************/
#include "plat_firmware.h"
#include "oal_file.h"
#include "oal_sdio_host_if.h"
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
#include "plat_pm.h"
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
#include "plat_sdio.h"
#include "los_event.h"
#include "los_typedef.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "fcntl.h"
#include "los_exc.h"
#include "oal_util.h"
#include "oal_time.h"
#include "plat_wifi_cfg.h"
#ifndef _PRE_WLAN_FEATURE_MFG_FW
#include "plat_rw.h"
#else
#include "plat_rw_mfg.h"
#endif
#endif
#include "oal_channel_host_if.h"
#include "wal_customize.h"
#include "wal_hipriv.h"
#include "oam_ext_if.h"
#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
#include "data_process.h"
#include "plat_data_backup.h"
#endif

/*****************************************************************************
  2 ȫ�ֱ���
*****************************************************************************/
#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
extern unsigned int g_ul_wlan_resume_wifi_init_flag;
#endif

#ifdef _PRE_HI113X_FS_DISABLE
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
static hi_u8 g_firmware_array_wifi_cfg_c01[] = {
    #include "c01/plat_wifi_cfg.h"
};

static hi_u8 g_firmware_array_wifi_cfg_c02[] = {
    #include "c02/plat_wifi_cfg.h"
};

static hi_u8 g_firmware_array_rw_bin_c01[] = {
    #include "c01/plat_rw.h"
};

static hi_u8 g_firmware_array_rw_bin_c02[] = {
    #include "c02/plat_rw.h"
};
#endif

DECLARE_FIRMWARE_FILE(wifi_cfg_c01);
DECLARE_FIRMWARE_FILE(wifi_cfg_c02);
DECLARE_FIRMWARE_FILE(rw_bin_c01);
DECLARE_FIRMWARE_FILE(rw_bin_c02);

static firmware_file_stru *g_st_wifi_cfg[SOFT_VER_BUTT] = {
    &firmware_file_wifi_cfg_c01,
    &firmware_file_wifi_cfg_c02
};

static firmware_file_stru *g_st_rw_bin[SOFT_VER_BUTT] = {
    &firmware_file_rw_bin_c01,
    &firmware_file_rw_bin_c02
};
#endif

#define WIFI_FIRMWARE_FILE_BIN "/usr/share/firmware/hisilicon/hi1131h_demo_non_rom.bin"

/*****************************************************************************
  2 �궨��
*****************************************************************************/
#define WIFI_CFG_C01_PATH   "/usr/share/firmware/hisilicon/wifi_cfg"
#define WIFI_CFG_C02_PATH   HI_NULL
#define RAM_CHECK_CFG_PATH  HI_NULL
#define STORE_WIFI_MEM      HI_NULL

#define FILE_COUNT_PER_SEND             1
#define MIN_FIRMWARE_FILE_TX_BUF_LEN    4096

#define DEVICE_EFUSE_ADDR               0x50000764
#define DEVICE_EFUSE_LENGTH             16

#define CFG_CMD_NUM_MAX                 10    /* ֧�����õ����������������� */

/*****************************************************************************
  3 ȫ�ֱ�������
*****************************************************************************/
hi_u8 *g_auc_cfg_path[SOFT_VER_BUTT] = {
    (hi_u8 *)WIFI_CFG_C01_PATH,
    WIFI_CFG_C02_PATH,
};

/* �洢cfg�ļ���Ϣ������cfg�ļ�ʱ��ֵ�����ص�ʱ��ʹ�øñ��� */
firmware_globals_struct  g_st_cfg_info;
cfg_cmd_struct  g_cus_cfg_cmd[CFG_CMD_NUM_MAX];      /* �洢ÿ��cfg�ļ��Ĳ���У׼���� */
hi_u32 g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_SUCC;
efuse_info_stru g_st_efuse_info = {
    .soft_ver = SOFT_VER_CO1,
    .mac_h = 0x0,
    .mac_m = 0x0,
    .mac_l = 0x0,
};

/*****************************************************************************
  4 ����ʵ��
*****************************************************************************/
extern hi_u32 usb_max_req_size(void);
static void firmware_mem_free(firmware_mem_stru *firmware_mem);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
extern int usleep(unsigned useconds);
extern int32_t plat_usb_init(void);
extern void plat_usb_destory(void);
#endif

/*****************************************************************************
 ��������  : host����device��������Ϣ
 �������  : data: ������Ϣ��buffer
             len : ����buffer�ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʽ��յĳ���
*****************************************************************************/
static hi_s32 firmware_read_msg(hi_u8 *data, hi_s32 len)
{
    hi_s32  l_len;

    if (oal_unlikely((data == HI_NULL))) {
        oam_error_log0(0, 0, "data is HI_NULL\n ");
        return -OAL_EFAIL;
    }

    l_len = oal_channel_patch_readsb(data, len, READ_MEG_TIMEOUT);
    oam_warning_log2(0, 0, "Receive l_len=[%d], data = [%s]\n", l_len, data);

    return l_len;
}

static hi_s32 firmware_read_msg_timeout(hi_u8 *data, hi_s32 len, hi_u32 timeout)
{
    hi_s32  l_len;

    if (oal_unlikely((data == HI_NULL))) {
        oam_error_log0(0, 0, "data is HI_NULL\n ");
        return -OAL_EFAIL;
    }

    l_len = oal_channel_patch_readsb(data, len, timeout);
    oam_warning_log2(0, 0, "Receive l_len=[%d], data = [%s]\n", l_len, data);

    return l_len;
}

/*****************************************************************************
 ��������  : host��device������Ϣ
 �������  : data: ����buffer
             len : �������ݵĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʷ��͵ĳ���
*****************************************************************************/
static hi_s32 firmware_send_msg(hi_u8 *data, hi_s32 len)
{
    hi_s32   l_ret;

    oam_info_log1(0, 0, "len = %d\n", len);
#ifdef HW_DEBUG
    print_hex_dump_bytes("firmware_send_msg :", DUMP_PREFIX_ADDRESS, data, (len < 128 ? len : 128)); /* len 128 */
#endif

    l_ret = oal_channel_patch_writesb(data, len);
    return l_ret;
}

static firmware_mem_stru* firmware_mem_request(void)
{
    firmware_mem_stru *firmware_mem = oal_kzalloc(sizeof(firmware_mem_stru), OAL_GFP_KERNEL);
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "g_st_firmware_mem KMALLOC failed\n");
        goto nomem;
    }

#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE)
#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    if (plat_usb_init()) {
        oam_warning_log0(0, 0, "plat_usb_init failed\n");
        goto nomem;
    }
#endif
    firmware_mem->ul_data_buf_len = usb_max_req_size();
#elif (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    firmware_mem->ul_data_buf_len = oal_sdio_func_max_req_size(oal_get_sdio_default_handler());
    oam_warning_log1(0, 0, "sdio max transmit size is [%d]\n", firmware_mem->ul_data_buf_len);
    if (firmware_mem->ul_data_buf_len < HISDIO_BLOCK_SIZE) {
        oam_warning_log1(0, 0, "sdio max transmit size [%d] is error!\n", firmware_mem->ul_data_buf_len);
        goto nomem;
    }
#endif

    do {
        firmware_mem->puc_data_buf = (hi_u8 *)OS_KMALLOC_GFP(firmware_mem->ul_data_buf_len);
        if (firmware_mem->puc_data_buf == HI_NULL) {
            oam_warning_log1(0, 0, "malloc mem len [%d] fail, continue to try in a smaller size\n",
                             firmware_mem->ul_data_buf_len);
            firmware_mem->ul_data_buf_len = firmware_mem->ul_data_buf_len >> 1;
        }
    } while ((firmware_mem->puc_data_buf == HI_NULL) && (firmware_mem->ul_data_buf_len >= MIN_FIRMWARE_FILE_TX_BUF_LEN));

    if (firmware_mem->puc_data_buf == HI_NULL) {
        oam_info_log0(0, 0, "puc_data_buf KMALLOC failed\n");
        goto nomem;
    }

    firmware_mem->puc_recv_cmd_buff = (hi_u8 *)OS_KMALLOC_GFP(CMD_BUFF_LEN);
    if (firmware_mem->puc_recv_cmd_buff == HI_NULL) {
        oam_info_log0(0, 0, "puc_recv_cmd_buff KMALLOC failed\n");
        goto nomem;
    }

    firmware_mem->puc_send_cmd_buff = (hi_u8 *)OS_KMALLOC_GFP(CMD_BUFF_LEN);
    if (firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_info_log0(0, 0, "puc_recv_cmd_buff KMALLOC failed\n");
        goto nomem;
    }

    return firmware_mem;

nomem:
    firmware_mem_free(firmware_mem);
    return HI_NULL;
}

static void firmware_mem_free(firmware_mem_stru *firmware_mem)
{
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "g_firmware_mem_mutex is null\n");
        return;
    }
    if (firmware_mem->puc_send_cmd_buff != HI_NULL) {
        oal_free(firmware_mem->puc_send_cmd_buff);
    }
    if (firmware_mem->puc_recv_cmd_buff != HI_NULL) {
        oal_free(firmware_mem->puc_recv_cmd_buff);
    }
    if (firmware_mem->puc_data_buf != HI_NULL) {
        oal_free(firmware_mem->puc_data_buf);
    }
    oal_free(firmware_mem);
    firmware_mem = HI_NULL;
#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE) && (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
    plat_usb_destory();
#endif
}

/*****************************************************************************
 ��������  : ����host����device��ȷ���ص�����
 �������  : expect: ����device��ȷ���ص�����
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
static hi_s32 recv_expect_result(const hi_u8 *expect, const firmware_mem_stru *firmware_mem)
{
    hi_s32 l_len;
    hi_s32 i;
    if (expect == HI_NULL) {
        oam_error_log0(0, 0, "recv_expect_result:expect = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (!strlen((const hi_char *)expect)) {
        oam_info_log0(0, 0, "not wait device to respond!\n");
        return HI_SUCCESS;
    }
    if (firmware_mem == HI_NULL || firmware_mem->puc_recv_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_recv_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }
    memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);
    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        l_len = firmware_read_msg(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN);
        if (l_len < 0) {
            oam_error_log0(0, 0, "recv result fail\n");
            return -OAL_EFAIL;
        }
        if (!memcmp(firmware_mem->puc_recv_cmd_buff, expect, strlen((const hi_char *)expect))) {
            oam_info_log1(0, 0, " send HI_SUCCESS, expect [%s] ok\n", expect);
            return HI_SUCCESS;
        } else {
            oam_error_log2(0, 0, " error result[%s], expect [%s], read result again\n",
                           firmware_mem->puc_recv_cmd_buff, expect);
        }
    }

    return -OAL_EFAIL;
}

static hi_s32 recv_expect_result_timeout(const hi_u8 *expect, const firmware_mem_stru *firmware_mem, hi_u32 timeout)
{
    hi_s32 l_len;

    if (expect == HI_NULL) {
        oam_error_log0(0, 0, "recv_expect_result_timeout:expect = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (!strlen((const hi_char *)expect)) {
        oam_info_log0(0, 0, "not wait device to respond!\n");
        return HI_SUCCESS;
    }

    memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);
    l_len = firmware_read_msg_timeout(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, timeout);
    if (l_len < 0) {
        oam_error_log0(0, 0, "recv result fail\n");
        return -OAL_EFAIL;
    }

    if (!memcmp(firmware_mem->puc_recv_cmd_buff, expect, strlen((const hi_char *)expect))) {
        oam_info_log1(0, 0, " send HI_SUCCESS, expect [%s] ok\n", expect);
        return HI_SUCCESS;
    } else {
        oam_error_log2(0, 0, " error result[%s], expect [%s], read result again\n",
            firmware_mem->puc_recv_cmd_buff, expect);
    }
    return -OAL_EFAIL;
}

/*****************************************************************************
 ��������  : host��device������Ϣ���ȴ�device������Ϣ
 �������  : data  : ����buffer
             len   : �������ݵĳ���
             expect: ����device�ظ�������
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*****************************************************************************/
static hi_s32 msg_send_and_recv_except(hi_u8 *data, hi_s32 len, const hi_u8 *expect,
    const firmware_mem_stru *firmware_mem)
{
    hi_s32  i;
    hi_s32  l_ret;

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        l_ret = firmware_send_msg(data, len);
        if (l_ret < 0) {
            continue;
        }

        l_ret = recv_expect_result(expect, firmware_mem);
        if (l_ret == 0) {
            return HI_SUCCESS;
        }
    }
    return -OAL_EFAIL;
}

/*****************************************************************************
 ��������  : ����cfg�ļ����������Ľ��������g_st_cfg_infoȫ�ֱ�����
 �������  : puc_cfg_info_buf: ������cfg�ļ����ݵ�buffer
             ul_index        : ��������������������ֵ
 �������  : ��
 �� �� ֵ  : NULL��ʾ�����ڴ�ʧ�ܣ����򷵻�ָ�򱣴����cfg�ļ�����������׵�ַ
*****************************************************************************/
static void *malloc_cmd_buf(hi_u8 *puc_cfg_info_buf, hi_u32 ul_index)
{
    hi_u32           l_len;
    hi_u8          *flag = HI_NULL;
    hi_u8          *p_buf = HI_NULL;

    if (puc_cfg_info_buf == HI_NULL) {
        oam_error_log0(0, 0, "malloc_cmd_buf: buf is HI_NULL!\n");
        return HI_NULL;
    }

    /* ͳ��������� */
    flag = puc_cfg_info_buf;
    g_st_cfg_info.al_count[ul_index] = 0;
    while (flag != HI_NULL) {
        /* һ����ȷ�������н�����Ϊ ; */
        flag = (hi_u8 *)strchr((const hi_char *)flag, CMD_LINE_SIGN);
        if (flag == HI_NULL) {
            break;
        }
        g_st_cfg_info.al_count[ul_index]++;
        flag++;
    }
    oam_info_log2(0, 0, "cfg file cmd count: al_count[%d] = %d\n", ul_index, g_st_cfg_info.al_count[ul_index]);

    /* ����洢����ռ� */
    l_len = ((g_st_cfg_info.al_count[ul_index]) + CFG_INFO_RESERVE_LEN) * sizeof(struct cmd_type_st);

    p_buf = OS_KMALLOC_GFP(l_len);
    if (p_buf == HI_NULL) {
        oam_error_log0(0, 0, "kmalloc cmd_type_st fail\n");
        return HI_NULL;
    }
    memset_s((void *)p_buf, l_len, 0, l_len);

    return p_buf;
}

/*****************************************************************************
 ��������  : ɾ���ַ������߶���Ŀո�
 �������  : string: ԭʼ�ַ���
             len   : �ַ����ĳ���
 �������  : ��
 �� �� ֵ  : ���󷵻�NULL�����򷵻�ɾ�����߿ո��Ժ��ַ������׵�ַ
*****************************************************************************/
static hi_u8 *delete_space(hi_u8 *string, hi_s32 *len)
{
    int i;

    if ((string == HI_NULL) || (len == HI_NULL)) {
        return HI_NULL;
    }

    /* ɾ��β���Ŀո� */
    for (i = *len - 1; i >= 0; i--) {
        if (string[i] != COMPART_KEYWORD) {
            break;
        }
        string[i] = '\0';
    }
    /* ���� */
    if (i < 0) {
        oam_error_log0(0, 0, " string is Space bar\n");
        return HI_NULL;
    }
    /* ��for����м�ȥ1���������1 */
    *len = i + 1;

    /* ɾ��ͷ���Ŀո� */
    for (i = 0; i < *len; i++) {
        if (string[i] != COMPART_KEYWORD) {
            /* ��ȥ�ո�ĸ��� */
            *len = *len - i;
            return &string[i];
        }
    }

    return HI_NULL;
}

/*************************************************************************************
 ��������  : ���ļ�������read mem������������
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���ش��ļ���������
*************************************************************************************/
static oal_file_stru* open_file_to_readm(hi_u8 *name)
{
    oal_file_stru *fp = HI_NULL;
    hi_u8 *file_name = HI_NULL;

    if (OAL_WARN_ON(name == HI_NULL)) {
        file_name = (hi_u8 *)"/data/memdump/readm_wifi";
    } else {
        file_name = name;
    }
    fp = oal_file_open((const hi_char *)file_name, (OAL_O_CREAT | OAL_O_RDWR | OAL_O_TRUNC), 0);

    return fp;
}

/*************************************************************************************
 ��������  : ����device�����������ڴ棬���浽ָ�����ļ���
 �������  : fp : �����ڴ���ļ�ָ��
             len: ��Ҫ������ڴ�ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ����򷵻�ʵ�ʱ�����ڴ�ĳ���
*************************************************************************************/
static hi_s32 recv_device_mem(oal_file_stru *fp, hi_u8 *puc_data_buf, hi_s32 len)
{
    hi_s32 l_ret = 0;
    hi_u8 retry = 3;
    hi_s32 lenbuf = 0;

    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log1(0, 0, "fp is error,fp = 0x%p\n", (uintptr_t)fp);
        return -OAL_EFAIL;
    }

    if (puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "puc_data_buf is HI_NULL\n");
        return -OAL_EFAIL;
    }

    oam_info_log1(0, 0, "expect recv len is [%d]\n", len);

    while (len > lenbuf) {
        l_ret = firmware_read_msg(puc_data_buf + lenbuf, len - lenbuf);
        if (l_ret > 0) {
            lenbuf += l_ret;
        } else {
            retry--;
            lenbuf = 0;
            if (0 == retry) {
                l_ret = -OAL_EFAIL;
                oam_error_log0(0, 0, "time out\n");
                break;
            }
        }
    }

    if (len <= lenbuf) {
        oal_file_write(fp, (hi_char *)puc_data_buf, len);
    }

    return l_ret;
}

/*************************************************************************************
 ��������  : ���������device�汾�ţ������device�ϱ��İ汾�ź�host�İ汾���Ƿ�ƥ��
 �������  : ��
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*************************************************************************************/
static hi_s32 check_version(const firmware_mem_stru *firmware_mem)
{
    hi_s32   l_ret;
    size_t   l_len;
    hi_s32   i;

    oam_info_log0(0, 0, "enter\n");

    if (firmware_mem == HI_NULL ||
        firmware_mem->puc_recv_cmd_buff == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    for (i = 0; i < HOST_DEV_TIMEOUT; i++) {
        oam_info_log1(0, 0, "loop index %d\n", i);
        if ((memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) ||
            (memcpy_s(firmware_mem->puc_send_cmd_buff, strlen(VER_CMD_KEYWORD), (hi_u8 *)VER_CMD_KEYWORD,
                strlen(VER_CMD_KEYWORD)) != EOK)) {
            continue;
        }
        l_len = strlen(VER_CMD_KEYWORD);

        firmware_mem->puc_send_cmd_buff[l_len] = COMPART_KEYWORD;
        l_len++;

        l_len = HISDIO_ALIGN_4_OR_BLK(l_len + 1);

        l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff, l_len);
        if (l_ret < 0) {
            oam_warning_log1(0, 0, "send version fail![%d]\n", i);
            continue;
        }
        if (memset_s(g_st_cfg_info.auc_DevVersion, VERSION_LEN, 0, VERSION_LEN) != EOK ||
            memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) {
            continue;
        }

        oam_info_log0(0, 0, "read msg start\n");
        l_ret = firmware_read_msg(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN);

        oam_info_log0(0, 0, "read msg over\n");
        if (l_ret < 0) {
            oam_warning_log1(0, 0, "read version fail![%d]\n", i);
            continue;
        }
        if (memcpy_s(g_st_cfg_info.auc_DevVersion, VERSION_LEN, firmware_mem->puc_recv_cmd_buff, VERSION_LEN) != EOK) {
            continue;
        }
        if (!memcmp((hi_char *)g_st_cfg_info.auc_DevVersion,
            (hi_char *)g_st_cfg_info.auc_CfgVersion, strlen((const hi_char *)g_st_cfg_info.auc_CfgVersion))) {
            oam_warning_log2(0, 0, "HI_SUCCESS: Device Version = [%s], CfgVersion = [%s].\n",
                g_st_cfg_info.auc_DevVersion, g_st_cfg_info.auc_CfgVersion);
            return HI_SUCCESS;
        } else {
            oam_error_log2(0, 0, "ERROR version,Device Version = [%s], CfgVersion = [%s].\n",
                g_st_cfg_info.auc_DevVersion, g_st_cfg_info.auc_CfgVersion);
            return -OAL_EFAIL;
        }
    }

    return -OAL_EFAIL;
}

/*****************************************************************************
 ��������  : ����number���͵���������͵�device
 �������  : key  : ����Ĺؼ���
             val: ����Ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ������ʾ�ɹ�
*****************************************************************************/
static hi_s32 number_type_cmd_send(hi_u8 *key, hi_u8 *val, const firmware_mem_stru *firmware_mem)
{
    hi_s32       l_ret;
    hi_u32       data_len;
    hi_u32       value_len;
    hi_u32       i;
    hi_u32       n;
#define RESERVED_BYTES 10

    if (firmware_mem == HI_NULL ||
        firmware_mem->puc_recv_cmd_buff == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (CMD_BUFF_LEN < strlen((const hi_char *)key) + strlen((const hi_char *)val) + RESERVED_BYTES) {
        oam_error_log2(0, 0, "the cmd string must be error, key=%s, vlaue=%s \n", key, val);
        return -OAL_EFAIL;
    }

    value_len = strlen((hi_char *)val);

    if (memset_s(firmware_mem->puc_recv_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK ||
        memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN) != EOK) {
        return -OAL_EFAIL;
    }

    data_len = (hi_s32)strlen((const hi_char *)key);
    if (memcpy_s(firmware_mem->puc_send_cmd_buff, data_len, key, data_len) != EOK) {
        return -OAL_EFAIL;
    }

    firmware_mem->puc_send_cmd_buff[data_len] = COMPART_KEYWORD;
    data_len = data_len + 1;

    for (i = 0, n = 0; (i <= value_len) && (n < INT32_STR_LEN); i++) {
        if ((val[i] == ',') || (i == value_len)) {
            if (n == 0) {
                continue;
            }
            if (memcpy_s((hi_u8 *)&firmware_mem->puc_send_cmd_buff[data_len], n,
                firmware_mem->puc_recv_cmd_buff, n) != EOK) {
                return -OAL_EFAIL;
            }
            data_len = data_len + n;

            firmware_mem->puc_send_cmd_buff[data_len] = COMPART_KEYWORD;
            data_len = data_len + 1;

            memset_s(firmware_mem->puc_recv_cmd_buff, INT32_STR_LEN, 0, INT32_STR_LEN);
            n = 0;
        } else if (val[i] == COMPART_KEYWORD) {
            continue;
        } else {
            firmware_mem->puc_recv_cmd_buff[n] = val[i];
            n++;
        }
    }
    firmware_mem->puc_send_cmd_buff[data_len + 1] = '\0';
    oam_info_log1(0, 0, "cmd=%s\r\n", firmware_mem->puc_send_cmd_buff);
    l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff, data_len);

    return l_ret;
}

/*****************************************************************************
 ��������  : ����file�������
 �������  : string   : file����Ĳ���
             addr     : ���͵����ݵ�ַ
             file_path: �����ļ���·��
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*****************************************************************************/
static hi_s32 parse_file_cmd(hi_u8 *string, unsigned long *addr, hi_char **file_path)
{
    hi_u8 *tmp = HI_NULL;
    hi_size_t count;
    hi_u8 *after = HI_NULL;
#define  DECIMAL_ALGORITHM 10

    if (string == HI_NULL || addr == HI_NULL || file_path == HI_NULL) {
        oam_error_log0(0, 0, "param is error!\n");
        return -OAL_EFAIL;
    }

    /* ��÷��͵��ļ��ĸ������˴�����Ϊ1��string�ַ����ĸ�ʽ������"1,0xXXXXX,file_path" */
    tmp = string;
    while (*tmp == COMPART_KEYWORD) {
        tmp++;
    }
    count = oal_simple_strtoul((const hi_char *)tmp, HI_NULL, DECIMAL_ALGORITHM);
    if (count != FILE_COUNT_PER_SEND) {
        oam_error_log1(0, 0, "the count of send file must be 1, count = [%d]\n", count);
        return -OAL_EFAIL;
    }

    /* ��tmpָ���ַ������ĸ */
    tmp = (hi_u8 *)strchr((const hi_char *)string, ',');
    if (tmp == HI_NULL) {
        oam_error_log0(0, 0, "param string is err!\n");
        return -OAL_EFAIL;
    } else {
        tmp++;
        while (*tmp == COMPART_KEYWORD) {
            tmp++;
        }
    }

    *addr = oal_simple_strtoul((hi_char *)tmp, (char **)(&after), 16);  /* base 16 */
    oam_info_log1(0, 0, "file to send addr:[0x%lx]\n", *addr);

    /* "1,0xXXXX,file_path" */
    /*         ^          */
    /*       after        */
    after++;
    while (COMPART_KEYWORD == *after) {
        after++;
    }

    oam_info_log1(0, 0, "after:[%s]\n", after);

    *file_path = (hi_char *)after;

    return HI_SUCCESS;
}


/*****************************************************************************
 ��������  : ��device����bootloaderʱ��DEVICE��ȡ�ڴ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : С��0��ʾʧ��
*****************************************************************************/
static hi_s32 read_device_mem(const wifi_dump_mem_info_stru *mem_dump_info, oal_file_stru *fp,
    const firmware_mem_stru *firmware_mem)
{
    hi_s32 ret = 0;
    hi_u32 size = 0;
    hi_u32 remainder = mem_dump_info->size;

    if (firmware_mem == HI_NULL || firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_send_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }
    while (remainder > 0) {
        memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, CMD_BUFF_LEN);

        size = oal_min(remainder, firmware_mem->ul_data_buf_len);

        oam_info_log1(0, 0, "read mem cmd:[%s]\n", firmware_mem->puc_send_cmd_buff);
        ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff,
                                strlen((const hi_char *)firmware_mem->puc_send_cmd_buff));
        if (ret < 0) {
            oam_warning_log2(0, 0, "wifi mem dump fail, mem_addr is [0x%lx],ret=%d\n",
                             mem_dump_info->mem_addr, ret);
            break;
        }

        ret = recv_device_mem(fp, firmware_mem->puc_data_buf, size);
        if (ret < 0) {
            oam_warning_log2(0, 0, "wifi mem dump fail, mem_addr is [0x%lx],ret=%d\n",
                             mem_dump_info->mem_addr, ret);
            break;
        }

        remainder -= size;
    }

    return ret;
}

static hi_s32 read_mem(hi_u8 *key, const hi_u8 *val, const firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    hi_s32 l_ret;
    hi_size_t size;
    hi_u8 *flag = HI_NULL;
    oal_file_stru *fp = HI_NULL;
    struct wifi_dump_mem_info_stru read_memory;
    memset_s(&read_memory, sizeof(struct wifi_dump_mem_info_stru), 0, sizeof(struct wifi_dump_mem_info_stru));

    flag = (hi_u8 *)strchr((const hi_char *)val, ',');
    if (flag == HI_NULL) {
        oam_error_log0(0, 0, "RECV LEN ERROR..\n");
        return -OAL_EFAIL;
    }
    if (firmware_mem == HI_NULL ||
        firmware_mem->puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "MEM IS HI_NULL \n");
        return -OAL_EFAIL;
    }

    flag++;
    oam_info_log1(0, 0, "recv len [%s]\n", flag);
    while (*flag == COMPART_KEYWORD) {
        flag++;
    }
    size = oal_simple_strtoul((const hi_char *)flag, HI_NULL, 10); /* base 10: DEC */

    fp = open_file_to_readm(HI_NULL);
    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log1(0, 0, "create file error,fp = 0x%p\n", (uintptr_t)fp);
        return -OAL_EFAIL;
    }

    read_memory.mem_addr = oal_simple_strtoul((const hi_char *)val, HI_NULL, 16);  /* base 16 */
    read_memory.size = (hi_u32)size;
    l_ret = read_device_mem(&read_memory, fp, firmware_mem);

    oal_file_close(fp);

    return l_ret;
}

/*****************************************************************************
 ��������  : ִ��number���͵�����
 �������  : key  : ����Ĺؼ���
             val: ����Ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*****************************************************************************/
static hi_s32 exec_number_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_s32       l_ret = -OAL_EFAIL;

    if (key == HI_NULL) {
        oam_error_log0(0, 0, "exec_number_type_cmd key null\n");
        return -OAL_EFAIL;
    }

    if (!memcmp(key, VER_CMD_KEYWORD, strlen(VER_CMD_KEYWORD))) {
        l_ret = check_version(firmware_mem);
        if (l_ret < 0) {
            oam_error_log1(0, 0, "check version FAIL [%d]\n", l_ret);
            return -OAL_EFAIL;
        }
    }

    if (!strcmp((hi_char *)key, WMEM_CMD_KEYWORD)) {
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        l_ret = recv_expect_result((const hi_u8 *)MSG_FROM_DEV_WRITEM_OK, firmware_mem);
        if (l_ret < 0) {
            oam_error_log0(0, 0, "recv expect result fail!\n");
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, CONFIG_CMD_KEYWORD)) {
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        l_ret = recv_expect_result((const hi_u8 *)MSG_FROM_DEV_CONFIG_OK, firmware_mem);
        if (l_ret < 0) {
            oam_print_err("recv expect result fail!\n");
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, JUMP_CMD_KEYWORD)) {
        g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_SUCC;
        l_ret = number_type_cmd_send(key, val, firmware_mem);
        if (l_ret < 0) {
            goto ret_err;
        }

        /* 100000ms timeout */
        l_ret = recv_expect_result_timeout((const hi_u8 *)MSG_FROM_DEV_JUMP_OK, firmware_mem, READ_MEG_JUMP_TIMEOUT);
        if (l_ret >= 0) {
            return l_ret;
        } else {
            g_ul_jump_cmd_result = CMD_JUMP_EXEC_RESULT_FAIL;
            return l_ret;
        }
    } else if (!strcmp((hi_char *)key, RMEM_CMD_KEYWORD)) {
        l_ret = read_mem(key, val, firmware_mem);
    }
    return l_ret;
ret_err:
    oam_error_log2(0, 0, "send key=[%s],value=[%s] fail\n", key, val);
    return l_ret;
}


/*****************************************************************************
 ��������  : ִ��quit���͵�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*****************************************************************************/
static hi_s32 exec_quit_type_cmd(firmware_mem_stru *firmware_mem)
{
    hi_s32   l_ret;
    hi_u32   l_len;

    if (firmware_mem == HI_NULL || firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "puc_send_cmd_buff = HI_NULL \n");
        return -OAL_EFAIL;
    }

    if (memset_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, 0, 8) != EOK) { /* 8: ��8������ */
        return -OAL_EFAIL;
    }  /* buffer len 8 */

    if (memcpy_s(firmware_mem->puc_send_cmd_buff, strlen(QUIT_CMD_KEYWORD),
        (hi_u8 *)QUIT_CMD_KEYWORD, strlen(QUIT_CMD_KEYWORD)) != EOK) {
        return -OAL_EFAIL;
    }
    l_len = strlen(QUIT_CMD_KEYWORD);

    firmware_mem->puc_send_cmd_buff[l_len] = COMPART_KEYWORD;
    l_len++;

    l_ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff, l_len, (const hi_u8 *)MSG_FROM_DEV_QUIT_OK,
        firmware_mem);

    return l_ret;
}

#ifndef _PRE_HI113X_FS_DISABLE
int g_fw_mode = FIRMWARE_BIN; /* Ĭ��ҵ��bin */
module_param(g_fw_mode, int, 0644);
#ifdef _PRE_LINUX_BUILTIN
void firmware_set_fw_mode(int mode)
{
    g_fw_mode = mode;
}
int firmware_get_fw_mode(hi_void)
{
    return g_fw_mode;
}
#endif
/*****************************************************************************
 ��������  : ִ��file���͵�����
 �������  : key  : ����Ĺؼ���
             val: ����Ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ�0��ʾ�ɹ�
*****************************************************************************/
static hi_s32 exec_file_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    unsigned long addr = 0;
    hi_u32 addr_send;
    hi_char *path = HI_NULL;
    hi_s32 ret;
    hi_u32 file_len;
    hi_u32 per_send_len;
    hi_u32 send_count;
    hi_s32 rdlen;
    hi_u32 i;
    hi_u32 offset = 0;
    oal_file_stru *fp = HI_NULL;
    hi_s32 verify_stage = 0;

    if (firmware_mem == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL ||
        firmware_mem->puc_data_buf == HI_NULL) {
        oam_error_log0(0, 0, "exec_file_type_cmd:: mem is HI_NULL");
        return -OAL_EFAIL;
    }

    ret = parse_file_cmd(val, &addr, &path);
    if (ret < 0) {
        oam_error_log0(0, 0, "exec_file_type_cmd:: parse file cmd fail");
        return ret;
    }
    if (g_fw_mode == MFG_FIRMWARE_BIN) {
        path = "/usr/share/firmware/hisilicon/hi3881_mfg_fw.bin";
    }
    fp = oal_file_open(path, (OAL_O_RDONLY), 0);
    if (IS_ERR_OR_NULL(fp)) {
        oam_error_log2(0, 0, "exec_file_type_cmd:: filp_open [%s] fail!!, fp=%p", path, fp);
        return -OAL_EFAIL;
    }
    set_under_mfg(g_fw_mode);    /* ���浱ǰfirmwareģʽ */
    /* ��ȡfile�ļ���С */
    file_len = oal_file_lseek(fp, 0, OAL_SEEK_END);

    /* �ָ�fp->f_pos���ļ���ͷ */
    oal_file_lseek(fp, 0, OAL_SEEK_SET);

    oam_info_log2(0, 0, "exec_file_type_cmd:: file len is [%d],firmware_mem->ul_data_buf_len=%d",
                  file_len, firmware_mem->ul_data_buf_len);

    per_send_len = (firmware_mem->ul_data_buf_len > file_len) ? file_len : firmware_mem->ul_data_buf_len;
    if (per_send_len == 0) {
        oam_error_log0(0, 0, "per_send_len == 0");
        return -OAL_EFAIL;
    }
    send_count = (file_len + per_send_len - 1) / per_send_len;
    oam_info_log1(0, 0, "exec_file_type_cmd:: send_count=%d", send_count);

    for (i = 0; i < send_count; i++) {
        rdlen = oal_file_read(fp, firmware_mem->puc_data_buf, per_send_len);
        if (rdlen > 0) {
            oam_info_log2(0, 0, "exec_file_type_cmd:: len of kernel_read is [%d], i=%d", rdlen, i);
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
            OAL_FILE_POS(fp) += rdlen;
#endif
        } else {
            oam_error_log2(0, 0, "exec_file_type_cmd:: len of kernel_read is error! ret=[%d], i=%d", rdlen, i);
            oal_file_close(fp);
            return -OAL_EFAIL;
        }

        addr_send = (hi_u32)(addr + offset);
        oam_info_log2(0, 0, "exec_file_type_cmd:: send addr is [0x%x], i=%d", addr_send, i);

        if (offset == 0 && offset + rdlen == file_len) {
            verify_stage = 3; /* 3:start and end. */
        } else if (offset == 0) {
            verify_stage = 0; /* start trans. */
        } else if (offset + rdlen == file_len) {
            verify_stage = 2; /* 2:end trans. */
        } else {
            verify_stage = 1; /* transferring. */
        }

        if (snprintf_s(firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1, "%s%c0x%x%c0x%x%c%d%c",
            FILES_CMD_KEYWORD, COMPART_KEYWORD, addr_send, COMPART_KEYWORD, rdlen, COMPART_KEYWORD,
            verify_stage, COMPART_KEYWORD) == -1) {
            return -OAL_EFAIL;
        }

        /* ���͵�ַ */
        oam_info_log1(0, 0, "exec_file_type_cmd:: send file addr cmd is [%s]", firmware_mem->puc_send_cmd_buff);
        ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff,
            strlen(firmware_mem->puc_send_cmd_buff), (const hi_u8 *)MSG_FROM_DEV_READY_OK, firmware_mem);
        if (ret < 0) {
            oam_error_log1(0, 0, "exec_file_type_cmd:: SEND [%s] error", firmware_mem->puc_send_cmd_buff);
            oal_file_close(fp);
            return -OAL_EFAIL;
        }
        /* Wait at least 5 ms */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        usleep(FILE_CMD_WAIT_TIME_MIN);
#endif
        /* �����ļ����� */
        ret = msg_send_and_recv_except(firmware_mem->puc_data_buf, rdlen,
            (const hi_u8 *)MSG_FROM_DEV_FILES_OK, firmware_mem);
        if (ret < 0) {
            oam_error_log0(0, 0, "exec_file_type_cmd:: send data fail");
            oal_file_close(fp);
            return -OAL_EFAIL;
        }

        offset += rdlen;
    }
    oal_file_close(fp);
    /* ���͵ĳ���Ҫ���ļ��ĳ���һ�� */
    if (offset != file_len) {
        oam_error_log2(0, 0, "exec_file_type_cmd:: send len[%d] is different with file_len[%d]", offset, file_len);
        return -OAL_EFAIL;
    }

    return HI_SUCCESS;
}
#else
static hi_s32 exec_file_type_cmd(hi_u8 *key, hi_u8 *val, firmware_mem_stru *firmware_mem)
{
    hi_unref_param(key);
    unsigned long addr;
    hi_u32 addr_send;
    hi_char *path = HI_NULL;
    hi_s32 ret;
    hi_u32 file_len;
    hi_u32 per_send_len;
    hi_s32 rdlen;
    hi_u32 i = 0;
    hi_u32 offset = 0;
    hi_u32 file_len_count;
    hi_u32  ul_soft_ver;
    hi_s32 verify_stage = 0;

    if (firmware_mem == HI_NULL ||
        firmware_mem->puc_data_buf == HI_NULL ||
        firmware_mem->puc_send_cmd_buff == HI_NULL) {
        oam_error_log0(0, 0, "mem is HI_NULL \n");
        return -OAL_EFAIL;
    }

    ul_soft_ver = get_device_soft_version();
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        oam_error_log0(0, 0, "device soft version is invalid!\n");
        return -OAL_EFAIL;
    }

    ret = parse_file_cmd(val, &addr, &path);
    if (ret < 0) {
        oam_error_log0(0, 0, "parse file cmd fail!\n");
        return ret;
    }

    oam_info_log1(0, 0, "download firmware:%s\n", path);

    file_len = g_st_rw_bin[ul_soft_ver]->len;
    file_len_count = file_len;

    oam_info_log1(0, 0, "file len is [%d]\n", file_len);

    per_send_len = (firmware_mem->ul_data_buf_len > file_len) ? file_len : firmware_mem->ul_data_buf_len;

    while (file_len_count > 0) {
        rdlen = per_send_len < file_len_count ? per_send_len : file_len_count;
        if (memcpy_s(firmware_mem->puc_data_buf, rdlen, g_st_rw_bin[ul_soft_ver]->addr + offset, rdlen) != EOK) {
            return -OAL_EFAIL;
        };

        addr_send = (hi_u32)(addr + offset);
        oam_info_log2(0, 0, "send addr is [0x%x], i=%d\n", addr_send, i);

#if (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        if (offset == 0 && offset + rdlen == file_len) {
            verify_stage = 3; /* start and end, verify_stage 3. */
        } else if (offset == 0) {
            verify_stage = 0; /* start trans. */
        } else if (offset + rdlen == file_len) {
            verify_stage = 2; /* end trans, verify_stage 2. */
        } else {
            verify_stage = 1; /* transferring. */
        }

        ret = snprintf_s((hi_char *)firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1,
            "%s%c0x%x%c0x%x%c%d%c", FILES_CMD_KEYWORD, COMPART_KEYWORD, addr_send, COMPART_KEYWORD,
            rdlen, COMPART_KEYWORD, verify_stage, COMPART_KEYWORD);
        if (ret < 0) {
            return -OAL_EFAIL;
        }
#endif
        /* ���͵�ַ */
        oam_info_log1(0, 0, "send file addr cmd is [%s]\n", firmware_mem->puc_send_cmd_buff);
        ret = msg_send_and_recv_except(firmware_mem->puc_send_cmd_buff,
            strlen((const hi_char *)firmware_mem->puc_send_cmd_buff),
            (const hi_u8 *)MSG_FROM_DEV_READY_OK, firmware_mem);
        if (ret < 0) {
            oam_error_log1(0, 0, "SEND [%s] error\n", firmware_mem->puc_send_cmd_buff);
            return -OAL_EFAIL;
        }
        /* Wait at least 5 ms */
#if (_PRE_OS_VERSION_LINUX == _PRE_OS_VERSION)
        usleep_range(FILE_CMD_WAIT_TIME_MIN, FILE_CMD_WAIT_TIME_MAX);
#elif (_PRE_OS_VERSION_LITEOS == _PRE_OS_VERSION)
        usleep(FILE_CMD_WAIT_TIME_MIN);
#endif
        /* �����ļ����� */
        ret = msg_send_and_recv_except(firmware_mem->puc_data_buf, rdlen, (const hi_u8 *)MSG_FROM_DEV_FILES_OK,
                                       firmware_mem);
        if (ret < 0) {
            oam_error_log0(0, 0, " send data fail\n");
            return -OAL_EFAIL;
        }

        offset += rdlen;
        file_len_count -= rdlen;
    }

    /* ���͵ĳ���Ҫ���ļ��ĳ���һ�� */
    if (offset != file_len) {
        oam_error_log2(0, 0, "file send len is err! send len is [%d], file len is [%d]\n", offset, file_len);
        return -OAL_EFAIL;
    }

    return HI_SUCCESS;
}
#endif

/*****************************************************************************
 ��������  : ִ��firmware download������
 �������  : cmd_type: �������������
             cmd_name: ����Ĺؼ���
             cmd_para: ����Ĳ���
 �������  : ��
 �� �� ֵ  : -1��ʾʧ�ܣ��Ǹ�����ʾ�ɹ�
*****************************************************************************/
static hi_s32 execute_download_cmd(hi_s32 cmd_type, hi_u8 *cmd_name, hi_u8 *cmd_para, firmware_mem_stru *firmware_mem)
{
    hi_s32 l_ret;

    switch (cmd_type) {
        case FILE_TYPE_CMD:
            oam_info_log0(0, 0, " command type FILE_TYPE_CMD\n");
            l_ret = exec_file_type_cmd(cmd_name, cmd_para, firmware_mem);
            break;
        case NUM_TYPE_CMD:
            oam_info_log0(0, 0, " command type NUM_TYPE_CMD\n");
            l_ret = exec_number_type_cmd(cmd_name, cmd_para, firmware_mem);
            break;
        case QUIT_TYPE_CMD:
            oam_info_log0(0, 0, " command type QUIT_TYPE_CMD\n");
            l_ret = exec_quit_type_cmd(firmware_mem);
            break;
        default:
            oam_error_log1(0, 0, "command type error[%d]\n", cmd_type);
            l_ret = -OAL_EFAIL;
            break;
    }

    return l_ret;
}

hi_s32 firmware_write_cfg(hi_u8 *key, hi_u8 *new_val, hi_u8 len)
{
    oal_file_stru    *fp;
    hi_s32           idx;
    hi_s64           l_ret;
    hi_u8            *cfg_patch = (hi_u8 *)WIFI_CFG_C01_PATH;

    /* ���Ҳ������Ƿ񻺴� */
    for (idx = 0; idx < CFG_CMD_NUM_MAX; ++idx) {
        if (strcmp((const hi_char *)key, (const hi_char *)g_cus_cfg_cmd[idx].cmd_name) == 0) {
            break;
        }
    }
    if (idx == CFG_CMD_NUM_MAX) {
        printk("cfg [%s] to write not found\n", key);
        return -OAL_EFAIL;
    }
    /* У����ֵ�ĳ���        */
    if (len > g_cus_cfg_cmd[idx].val_len - 3) { /* 3:Ԥ���� ��;���͡�\r\n���Ĵ�С */
        printk("new val [%s] length exceeds old val [%s]\n", new_val, g_cus_cfg_cmd[idx].cmd_para);
        return -OAL_EFAIL;
    }
    fp = oal_file_open((const hi_char *)cfg_patch, (OAL_O_RDWR), 0);
    if (IS_ERR_OR_NULL(fp)) {
        printk("open file %s fail, fp=%p\n", cfg_patch, fp);
        return -OAL_EFAIL;
    }
    /* ��λ��Ŀ��λ�� */
    l_ret = oal_file_lseek(fp, g_cus_cfg_cmd[idx].val_offset, OAL_SEEK_SET);
    hi_u8 *buf = (hi_u8 *)OS_KMALLOC_GFP(g_cus_cfg_cmd[idx].val_len);
    if (buf == HI_NULL) {
        printk("firmware_write_cfg:alloc buf failed\n");
        oal_file_close(fp);
        return -OAL_EFAIL;
    }
    memset_s(buf, g_cus_cfg_cmd[idx].val_len, ' ', g_cus_cfg_cmd[idx].val_len);
    memcpy_s(buf, g_cus_cfg_cmd[idx].val_len, new_val, len + 1);
    if (len < g_cus_cfg_cmd[idx].val_len - 2) { /* 2:�س�����Ԥ�� */
        buf[len] = ';'; /* ���ý����� */
    }
    buf[g_cus_cfg_cmd[idx].val_len - 2] = '\r'; /* ���ӻس�����.2:����ֵ */
    buf[g_cus_cfg_cmd[idx].val_len - 1] = '\n';
    l_ret = oal_file_write(fp, (hi_char *)buf,  g_cus_cfg_cmd[idx].val_len);
    oal_file_close(fp);

    if (l_ret == g_cus_cfg_cmd[idx].val_len) { /* ����ֵΪд���ֽ�����ʾ�ɹ� */
        /* ���»��� */
        memcpy_s(g_cus_cfg_cmd[idx].cmd_para, g_cus_cfg_cmd[idx].val_len, buf, g_cus_cfg_cmd[idx].val_len);
        oal_free(buf);
        return HI_SUCCESS;
    }
    oal_free(buf);
    return HI_FAIL;
}

/*****************************************************************************
 ��������  : ��ȡcfg�ļ������ݣ��ŵ�������̬�����buffer��
 �������  : cfg_patch    : cfg�ļ���·��
             puc_read_buffer : ����cfg�ļ����ݵ�buffer
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
hi_s32 firmware_read_cfg(const hi_u8 *cfg_patch, hi_u8 *puc_read_buffer)
{
    oal_file_stru    *fp = HI_NULL;
    hi_s32                   l_ret;

    if ((cfg_patch == HI_NULL) || (puc_read_buffer == HI_NULL)) {
        printk("para is HI_NULL\n");
        return -OAL_EFAIL;
    }

    fp = oal_file_open((const hi_char *)cfg_patch, (OAL_O_RDONLY), 0);
    if (IS_ERR_OR_NULL(fp)) {
        printk("open file %s fail, fp=%p\n", cfg_patch, fp);
        return -OAL_EFAIL;
    }

    memset_s(puc_read_buffer, READ_CFG_BUF_LEN, 0, READ_CFG_BUF_LEN);
    l_ret = oal_file_read(fp, puc_read_buffer, READ_CFG_BUF_LEN);
    *(puc_read_buffer + READ_CFG_BUF_LEN - 1) = '\0';
    oal_file_close(fp);
    fp = HI_NULL;

    return l_ret;
}

hi_u8 firmware_param_check(hi_u8 **begin, hi_u8 **end, hi_u8 **link, hi_u8 *puc_cfg_buffer, hi_s32* l_ret)
{
    *begin = puc_cfg_buffer;

    /* ע���� */
    if (puc_cfg_buffer[0] == '@') {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    /* �����У������˳������� */
    *link = (hi_u8 *)strchr((const hi_char *)(*begin), '=');
    if (*link == HI_NULL) {
        /* �˳������� */
        if (strstr((hi_char *)puc_cfg_buffer, QUIT_CMD_KEYWORD) != HI_NULL) {
            *l_ret = QUIT_TYPE_CMD;
            return HI_SUCCESS;
        }
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }
    /* �����У�û�н����� */
    *end = (hi_u8 *)strchr((const hi_char *)(*link), ';');
    if (*end == HI_NULL) {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    return HI_CONTINUE;
}

hi_u8 firmware_parse_cmd_type(hi_s32 *l_cmdlen, hi_u8 **handle, hi_s32* l_ret)
{
    hi_u8 *handle_temp = HI_NULL;

    /* �ж��������� */
    if (!memcmp(*handle, (hi_u8 *)FILE_TYPE_CMD_KEY, strlen((const hi_char *)FILE_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)FILE_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            oam_error_log1(0, 0, "'ADDR_FILE_'is not handle child string, handle=%s", *handle);
            *l_ret = ERROR_TYPE_CMD;
            return HI_FAIL;
        }
        *handle = handle_temp + strlen(FILE_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(FILE_TYPE_CMD_KEY);
        *l_ret = FILE_TYPE_CMD;
    } else if (!memcmp(*handle, (hi_u8 *)NUM_TYPE_CMD_KEY, strlen(NUM_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)NUM_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            oam_error_log1(0, 0, "'PARA_' is not handle child string, handle=%s", *handle);
            *l_ret = ERROR_TYPE_CMD;
            return HI_FAIL;
        }
        *handle = handle_temp + strlen(NUM_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(NUM_TYPE_CMD_KEY);
        *l_ret = NUM_TYPE_CMD;
    } else if (!memcmp(*handle, (hi_u8 *)CFG_TYPE_CMD_KEY, strlen(CFG_TYPE_CMD_KEY))) {
        handle_temp = (hi_u8 *)strstr((const hi_char *)(*handle), (const hi_char *)CFG_TYPE_CMD_KEY);
        if (handle_temp == HI_NULL) {
            *l_ret = CFG_TYPE_CMD;
            return HI_SUCCESS;
        }
        *handle = handle_temp + strlen(CFG_TYPE_CMD_KEY);
        *l_cmdlen = *l_cmdlen - strlen(CFG_TYPE_CMD_KEY);
        *l_ret = CFG_TYPE_CMD;
    } else {
        *l_ret = ERROR_TYPE_CMD;
        return HI_FAIL;
    }

    return HI_CONTINUE;
}

/*****************************************************************************
 ��������  : ����cfg�ļ��е�����
 �������  : puc_cfg_buffer: ����cfg�ļ����ݵ�buffer
             puc_cmd_name  : ��������Ժ�����ؼ��ֵ�buffer
             puc_cmd_para  : ��������Ժ����������buffer
 �������  : ��
 �� �� ֵ  : �������������
*****************************************************************************/
static hi_s32 firmware_parse_cmd(hi_u8 *puc_cfg_buffer, hi_u8 *puc_cmd_name, hi_u8 cmd_len,
    hi_u8 *puc_cmd_para, hi_u32 *val_begin)
{
    hi_s32       l_ret;
    hi_s32       l_cmdlen;
    hi_s32       l_paralen;
    hi_u8      *begin = HI_NULL;
    hi_u8      *end = HI_NULL;
    hi_u8      *link = HI_NULL;
    hi_u8      *handle = HI_NULL;

    hi_unref_param(cmd_len);

    if ((puc_cfg_buffer == HI_NULL) || (puc_cmd_name == HI_NULL) || (puc_cmd_para == HI_NULL)) {
        oam_error_log0(0, 0, "para is HI_NULL\n");
        return ERROR_TYPE_CMD;
    }

    if (firmware_param_check(&begin, &end, &link, puc_cfg_buffer, &l_ret) != HI_CONTINUE) {
        return l_ret;
    }

    *val_begin = (hi_u32)(link + 1);

    l_cmdlen = link - begin;

    /* ɾ���ؼ��ֵ����߿ո� */
    handle = delete_space((hi_u8 *)begin, &l_cmdlen);
    if (handle == HI_NULL) {
        return ERROR_TYPE_CMD;
    }

    if (firmware_parse_cmd_type(&l_cmdlen, &handle, &l_ret) != HI_CONTINUE) {
        return l_ret;
    }

    if (l_cmdlen > DOWNLOAD_CMD_LEN || l_cmdlen < 0) {
        oam_error_log0(0, 0, "cmd len out of range!\n");
        return ERROR_TYPE_CMD;
    }
    memcpy_s(puc_cmd_name, l_cmdlen, handle, l_cmdlen);

    /* ɾ��ֵ���߿ո� */
    begin = link + 1;
    l_paralen = end - begin;
    if (l_paralen > DOWNLOAD_CMD_PARA_LEN || l_paralen < 0) {
        oam_error_log0(0, 0, "para len out of range!\n");
        return ERROR_TYPE_CMD;
    }

    handle = delete_space((hi_u8 *)begin, &l_paralen);
    if (handle == HI_NULL) {
        return ERROR_TYPE_CMD;
    }
    memcpy_s(puc_cmd_para, l_paralen, handle, l_paralen);

    return l_ret;
}

/*****************************************************************************
 ��������  : ����cfg�ļ����������Ľ��������g_st_cfg_infoȫ�ֱ�����
 �������  : puc_cfg_info_buf: ������cfg�ļ����ݵ�buffer
             l_buf_len       : puc_cfg_info_buf�ĳ���
             ul_index        : ��������������������ֵ
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
static hi_s32 firmware_parse_cfg(hi_u8 *puc_cfg_info_buf, hi_s32 l_buf_len, hi_u32 ul_index)
{
    hi_u32          cfg_idx = 0;
    hi_s32          i, l_len, cmd_type;
    hi_u8          *flag = HI_NULL;
    hi_u8          *begin = HI_NULL;
    hi_u8          *end = HI_NULL;
    hi_u8           cmd_name[DOWNLOAD_CMD_LEN];
    hi_u8           cmd_para[DOWNLOAD_CMD_PARA_LEN];
    hi_u32          cmd_para_len = 0;
    hi_u32          val_offset = 0;
    if (puc_cfg_info_buf == HI_NULL) {
        oam_error_log0(0, 0, "firmware_parse_cfg:: puc_cfg_info_buf is HI_NULL");
        return -OAL_EFAIL;
    }

    g_st_cfg_info.apst_cmd[ul_index] = (struct cmd_type_st *)malloc_cmd_buf(puc_cfg_info_buf, ul_index);
    if (g_st_cfg_info.apst_cmd[ul_index] == HI_NULL) {
        oam_error_log0(0, 0, "firmware_parse_cfg:: malloc_cmd_buf fail");
        return -OAL_EFAIL;
    }

    /* ����CMD BUF */
    flag = puc_cfg_info_buf;
    l_len = l_buf_len;
    i = 0;
    while ((i < g_st_cfg_info.al_count[ul_index]) && (flag < &puc_cfg_info_buf[l_len])) {
        /*
         * ��ȡ�����ļ��е�һ��,�����ļ�������unix��ʽ.
         * �����ļ��е�ĳһ�к����ַ� @ ����Ϊ����Ϊע����
         */
        begin = flag;
        end   = (hi_u8 *)strchr((const hi_char *)flag, '\n');
        if (end == HI_NULL) {           /* �ļ������һ�У�û�л��з� */
            end = &puc_cfg_info_buf[l_len];
        } else if (end == begin) {     /* ����ֻ��һ�����з� */
            oam_error_log0(0, 0, "blank line\n");
            flag = end + 1;
            continue;
        }
        *end = '\0';

        oam_info_log1(0, 0, "operation string is [%s]\n", begin);

        memset_s(cmd_name, DOWNLOAD_CMD_LEN, 0, DOWNLOAD_CMD_LEN);
        memset_s(cmd_para, DOWNLOAD_CMD_PARA_LEN, 0, DOWNLOAD_CMD_PARA_LEN);

        cmd_type = firmware_parse_cmd(begin, cmd_name, DOWNLOAD_CMD_LEN, cmd_para, &val_offset);
        oam_info_log2(0, 0, "----[%s] = [%s].----", cmd_name, cmd_para);

        if (cmd_type == CFG_TYPE_CMD) { /* ���������������ͣ����ӵ����� */
            if (cfg_idx == CFG_CMD_NUM_MAX) {
                oam_error_log1(0, 0, "firmware_parse_cfg:: Cus cfg items exceed limit %d, will omit", CFG_CMD_NUM_MAX);
            }
            memcpy_s(g_cus_cfg_cmd[cfg_idx].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN);
            memcpy_s(g_cus_cfg_cmd[cfg_idx].cmd_para, DOWNLOAD_CMD_PARA_LEN, cmd_para, DOWNLOAD_CMD_PARA_LEN);
            g_cus_cfg_cmd[cfg_idx].cmd_name[DOWNLOAD_CMD_LEN - 1] = '\0';
            g_cus_cfg_cmd[cfg_idx].cmd_para[DOWNLOAD_CMD_PARA_LEN - 1] = '\0';
            g_cus_cfg_cmd[cfg_idx].val_offset = val_offset - (hi_u32)puc_cfg_info_buf;
            g_cus_cfg_cmd[cfg_idx].val_len = (hi_u16)((uintptr_t)end - val_offset + 1);
#ifdef CUSTOM_DBG
            oam_info_log0(0, 0, "firmware_parse_cfg:: cmd type=[%d],cmd_name=[%s],cmd_para=[%s], line len %d",
                          cmd_type, cmd_name, cmd_para, g_cus_cfg_cmd[cfg_idx].val_len);
#endif
            ++cfg_idx;
        } else if (cmd_type != ERROR_TYPE_CMD) {    /* ��ȷ���������ͣ����� */
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_type = cmd_type;
            if (memcpy_s(g_st_cfg_info.apst_cmd[ul_index][i].cmd_name, DOWNLOAD_CMD_LEN, cmd_name, DOWNLOAD_CMD_LEN) !=
                EOK ||  memcpy_s(g_st_cfg_info.apst_cmd[ul_index][i].cmd_para, DOWNLOAD_CMD_PARA_LEN, cmd_para,
                DOWNLOAD_CMD_PARA_LEN) != EOK) {
                return -OAL_EFAIL;
            }
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_name[DOWNLOAD_CMD_LEN - 1] = '\0';
            g_st_cfg_info.apst_cmd[ul_index][i].cmd_para[DOWNLOAD_CMD_PARA_LEN - 1] = '\0';
            if (g_st_cfg_info.apst_cmd[ul_index][i].cmd_name == HI_NULL) {
                return -OAL_EFAIL;
            }
            /* ��ȡ���ð汾�� */
            if (!memcmp(g_st_cfg_info.apst_cmd[ul_index][i].cmd_name, VER_CMD_KEYWORD, strlen(VER_CMD_KEYWORD))) {
                cmd_para_len = strlen((const hi_char *)g_st_cfg_info.apst_cmd[ul_index][i].cmd_para);
                if (cmd_para_len <= VERSION_LEN) {
                    memcpy_s(g_st_cfg_info.auc_CfgVersion, cmd_para_len, g_st_cfg_info.apst_cmd[ul_index][i].cmd_para,
                        cmd_para_len);
                } else {
                    oam_error_log1(0, 0, "firmware_parse_cfg:: cmd_para_len = %d over auc_CfgVersion length",
                        cmd_para_len);
                    return -OAL_EFAIL;
                }
                oam_warning_log1(0, 0, "Hi3881 VERSION:: [%s]", g_st_cfg_info.auc_CfgVersion);
            }
            i++;
        }
        flag = end + 1;
    }

    /* ����ʵ������������޸����յ�������� */
    g_st_cfg_info.al_count[ul_index] = i;
    oam_info_log2(0, 0, "firmware_parse_cfg:: effective cmd count: al_count[%d] = %d\n", ul_index,
        g_st_cfg_info.al_count[ul_index]);

    return HI_SUCCESS;
}


/*****************************************************************************
 ��������  : ��ȡcfg�ļ����������������Ľ��������g_st_cfg_infoȫ�ֱ�����
 �������  : cfg_patch: cfg�ļ���·��
             ul_index     : ��������������������ֵ
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
static hi_s32 firmware_get_cfg(const hi_u8 *cfg_patch, hi_u32 ul_index)
{
    hi_u8   *read_cfg_buf = HI_NULL;
    hi_s32   l_readlen;
    hi_s32   l_ret;
    hi_u32  ul_soft_ver = 0;

    if (cfg_patch == HI_NULL) {
        oam_warning_log0(0, 0, "firmware_get_cfg:: cfg file path is HI_NULL");
        return -OAL_EFAIL;
    }

    /* cfg�ļ��޶���С��2048,���cfg�ļ��Ĵ�Сȷʵ����2048�������޸�READ_CFG_BUF_LEN��ֵ */
    read_cfg_buf = OS_KMALLOC_GFP(READ_CFG_BUF_LEN);
    if (read_cfg_buf == HI_NULL) {
        oam_warning_log0(0, 0, "firmware_get_cfg:: kmalloc READ_CFG_BUF fail");
        return -OAL_EFAIL;
    }

    memset_s(read_cfg_buf, READ_CFG_BUF_LEN, 0, READ_CFG_BUF_LEN);

#ifndef _PRE_HI113X_FS_DISABLE
    (hi_void)ul_soft_ver;
    l_readlen = firmware_read_cfg(cfg_patch, read_cfg_buf);
    if (l_readlen < 0) {
        oam_warning_log1(0, 0, "firmware_get_cfg:: firmware_read_cfg failed[%d]", l_readlen);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    } else if (l_readlen > READ_CFG_BUF_LEN - 1) {
        /*
         * ��1��Ϊ��ȷ��cfg�ļ��ĳ��Ȳ�����READ_CFG_BUF_LEN��
         * ��Ϊfirmware_read_cfg���ֻ���ȡREAD_CFG_BUF_LEN���ȵ�����
         */
        oam_error_log2(0, 0, "firmware_get_cfg:: cfg file [%s] larger than %d", cfg_patch, READ_CFG_BUF_LEN);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    }
#else
    ul_soft_ver = get_device_soft_version();
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        oam_error_log1(0, 0, "firmware_get_cfg:: get_device_soft_version failed[%d]", ul_soft_ver);
        oal_free(read_cfg_buf);
        return -OAL_EFAIL;
    }

    l_readlen = g_st_wifi_cfg[ul_soft_ver]->len;
    if (l_readlen > READ_CFG_BUF_LEN) {
        oam_error_log1(0, 0, "firmware_get_cfg:: read_wifi_cfg failed[%d]", l_readlen);
        oal_free(read_cfg_buf);
        read_cfg_buf = HI_NULL;
        return -OAL_EFAIL;
    }

    if (memcpy_s(read_cfg_buf, l_readlen, g_st_wifi_cfg[ul_soft_ver]->addr, l_readlen) != EOK) {
        oal_free(read_cfg_buf);
        return -OAL_EFAIL;
    }
#endif

    l_ret = firmware_parse_cfg(read_cfg_buf, l_readlen, ul_index);
    if (l_ret < 0) {
        oam_error_log1(0, 0, "firmware_get_cfg:: firmware_parse_cfg failed[%d]", l_ret);
    }

    oal_free(read_cfg_buf);
    read_cfg_buf = HI_NULL;

    return l_ret;
}

/*****************************************************************************
 ��������  : firmware����
 �������  : ul_index: ��Ч�����������������
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
hi_s32 firmware_download(hi_u32 ul_index)
{
    hi_s32 l_ret;
    hi_s32 i;
    hi_s32 l_cmd_type;
    hi_u8 *puc_cmd_name = HI_NULL;
    hi_u8 *puc_cmd_para = HI_NULL;
    firmware_mem_stru *firmware_mem = HI_NULL;

    if (ul_index >= CFG_FILE_TOTAL) {
        oam_error_log1(0, 0, "firmware_download:: ul_index [%d] is error", ul_index);
        return -OAL_EFAIL;
    }

    if (g_st_cfg_info.al_count[ul_index] == 0) {
        oam_error_log1(0, 0, "firmware_download:: firmware download cmd count is 0, ul_index = [%d]", ul_index);
        return -OAL_EFAIL;
    }

    firmware_mem = firmware_mem_request();
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "firmware_download:: firmware_mem_request fail");
        return -OAL_EFAIL;
    }

    for (i = 0; i < g_st_cfg_info.al_count[ul_index]; i++) {
        l_cmd_type   = g_st_cfg_info.apst_cmd[ul_index][i].cmd_type;
        puc_cmd_name = g_st_cfg_info.apst_cmd[ul_index][i].cmd_name;
        puc_cmd_para = g_st_cfg_info.apst_cmd[ul_index][i].cmd_para;
        l_ret = execute_download_cmd(l_cmd_type, puc_cmd_name, puc_cmd_para, firmware_mem);
        if (l_ret < 0) {
            if (puc_cmd_name == HI_NULL) {
                continue;
            }
            if (ul_index == RAM_REG_TEST_CFG && ((!memcmp(puc_cmd_name, JUMP_CMD_KEYWORD, strlen(JUMP_CMD_KEYWORD)))
                && (g_ul_jump_cmd_result == CMD_JUMP_EXEC_RESULT_FAIL))) {
                /* device mem check ����ʧ�ܣ�����ִ��READM�������������� */
                oam_error_log0(0, 0, "firmware_download:: Device Mem Reg check result is fail");
                continue;
            }
            l_ret = -OAL_EFAIL;
            firmware_mem_free(firmware_mem);
            return l_ret;
        }
    }
    l_ret = HI_SUCCESS;
    firmware_mem_free(firmware_mem);
    return l_ret;
}


EXPORT_SYMBOL(firmware_download);

efuse_info_stru *get_efuse_info_handler(hi_void)
{
    return &g_st_efuse_info;
}
EXPORT_SYMBOL(get_efuse_info_handler);

hi_u32 get_device_soft_version(hi_void)
{
    efuse_info_stru *pst_efuse_info;
    hi_u32   ul_soft_ver;
    pst_efuse_info = get_efuse_info_handler();
    if (pst_efuse_info == HI_NULL) {
        printk("***get_device_soft_version***[%d]\n", __LINE__);
        return SOFT_VER_BUTT;
    }

    ul_soft_ver = pst_efuse_info->soft_ver;
    if (ul_soft_ver >= SOFT_VER_BUTT) {
        printk("***get_device_soft_version***[%d]\n", __LINE__);
        return SOFT_VER_BUTT;
    }

    return ul_soft_ver;
}

hi_s32 firmware_read_efuse_info(hi_void)
{
    firmware_mem_stru *firmware_mem = HI_NULL;
    unsigned long ul_mac_addr = DEVICE_EFUSE_ADDR;

    efuse_info_stru *pst_efuse_info = get_efuse_info_handler();
    if (pst_efuse_info == HI_NULL) {
        oam_error_log0(0, 0, "pst_efuse_info is HI_NULL!\n");
        goto failed;
    }
    if (memset_s(pst_efuse_info, sizeof(efuse_info_stru), 0, sizeof(efuse_info_stru)) != EOK) {
        goto failed;
    }

    firmware_mem = firmware_mem_request();
    if (firmware_mem == HI_NULL) {
        oam_error_log0(0, 0, "firmware_mem_request fail\n");
        goto failed;
    }

    hi_u32 ul_size = DEVICE_EFUSE_LENGTH;
#if (_PRE_FEATURE_USB == _PRE_FEATURE_CHANNEL_TYPE)
    ul_size = hiusb_align_32(ul_size);
#elif (_PRE_FEATURE_SDIO == _PRE_FEATURE_CHANNEL_TYPE)
    ul_size = HISDIO_ALIGN_4_OR_BLK(ul_size);
#endif
    if (ul_size > firmware_mem->ul_data_buf_len) {
        oam_error_log0(0, 0, "device mac length is too long !\n");
        goto failed;
    }

    if (snprintf_s((hi_char *)firmware_mem->puc_send_cmd_buff, CMD_BUFF_LEN, CMD_BUFF_LEN - 1, "%s%c0x%lx%c%d%c",
                   RMEM_CMD_KEYWORD, COMPART_KEYWORD, ul_mac_addr, COMPART_KEYWORD, ul_size, COMPART_KEYWORD) == -1) {
        goto failed;
    }
    oam_info_log1(0, 0, "read mac cmd:[%s]\n", firmware_mem->puc_send_cmd_buff);
    hi_s32 l_ret = firmware_send_msg(firmware_mem->puc_send_cmd_buff,
                                     strlen((const hi_char *)firmware_mem->puc_send_cmd_buff));
    if (l_ret < 0) {
        oam_error_log1(0, 0, "read device mac cmd send fail![%d]\n", l_ret);
        goto failed;
    }

    l_ret = firmware_read_msg(firmware_mem->puc_data_buf, ul_size);
    if (l_ret < 0) {
        oam_error_log1(0, 0, "read device mac fail![%d]\n", l_ret);
        goto failed;
    }

    if (memcpy_s(pst_efuse_info, sizeof(efuse_info_stru), firmware_mem->puc_data_buf,
        sizeof(efuse_info_stru)) != EOK) {
        goto failed;
    }

    firmware_mem_free(firmware_mem);
    return HI_SUCCESS;

failed:
    if (firmware_mem != HI_NULL) {
        firmware_mem_free(firmware_mem);
    }
    return -OAL_EFAIL;
}
/*****************************************************************************
 ��������  : firmware���ص�cfg�ļ���ʼ������ȡ������cfg�ļ����������Ľ��������
             g_st_cfg_infoȫ�ֱ�����
 �������  : ��
 �������  : ��
 �� �� ֵ  : 0��ʾ�ɹ���-1��ʾʧ��
*****************************************************************************/
hi_u32 plat_firmware_init(void)
{
    hi_s32  l_ret;
    hi_u32   ul_soft_ver;

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    if (g_ul_wlan_resume_wifi_init_flag) {
        l_ret = plat_data_recover();
        if (l_ret != HI_SUCCESS) {
            oam_error_log1(0, 0, "plat_firmware_init:: plat_data_recover failed[%d]", l_ret);
            return OAL_EFAIL;
        }
    }
#endif

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    if (!g_ul_wlan_resume_wifi_init_flag) {
#endif
        ul_soft_ver = get_device_soft_version();
        if (ul_soft_ver >= SOFT_VER_BUTT) {
            oam_error_log1(0, 0, "plat_firmware_init:: get_device_soft_version failed[%d]", ul_soft_ver);
            return OAL_EFAIL;
        }

        /* ����cfg�ļ� */
        l_ret = firmware_get_cfg(g_auc_cfg_path[ul_soft_ver], WIFI_CFG);
        if (l_ret < 0) {
            oam_error_log1(0, 0, "plat_firmware_init:: firware_get_cfg faile[%d]d", l_ret);
            plat_firmware_clear();
            return OAL_EFAIL;
        }

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    }
#endif

#ifdef _PRE_WLAN_FEATURE_DATA_BACKUP
    l_ret = plat_data_backup();
    if (l_ret !=  HI_SUCCESS) {
        oam_error_log1(0, 0, "plat_firmware_init:: plat_data_backup failed[%d]", l_ret);
        plat_firmware_clear();
        return OAL_EFAIL;
    }
#endif

    printk("plat_firmware_init SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

EXPORT_SYMBOL(plat_firmware_init);

/*****************************************************************************
 ��������  : �ͷ�firmware_cfg_initʱ������ڴ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ���Ƿ���0����ʾ�ɹ�
*****************************************************************************/
hi_s32 plat_firmware_clear(void)
{
    hi_s32 i;

    for (i = 0; i < CFG_FILE_TOTAL; i++) {
        g_st_cfg_info.al_count[i] = 0;
        if (g_st_cfg_info.apst_cmd[i] != HI_NULL) {
            oal_free(g_st_cfg_info.apst_cmd[i]);
            g_st_cfg_info.apst_cmd[i] = HI_NULL;
        }
    }
    return HI_SUCCESS;
}

hi_bool char_2_hex(hi_u8 c, hi_u8 *val)
{
    if ((c >= '0') && (c <= '9')) {
        *val = c - '0';
        return HI_TRUE;
    }
    if ((c >= 'a') && (c <= 'f')) {
        *val = 0x0a + c - 'a';
        return HI_TRUE;
    }
    if ((c >= 'A') && (c <= 'F')) {
        *val = 0x0a + c - 'A';
        return HI_TRUE;
    }

    return HI_FALSE;
}

hi_bool parse_mac_addr(const hi_u8 *str, hi_u8 str_len, hi_u8 *mac, hi_u8 size)
{
    hi_u8   i, temp;
    hi_u8   val = 0;
    hi_u8   idx = 0;
    hi_u8   offset = 0;
    hi_bool is_mac_valid = HI_FALSE;
    hi_bool ret;

    for (i = 0; (i < str_len) && (idx < size); ++i) {
        ret = char_2_hex(str[i], &temp);
        if (ret) {
            if (offset == 2) { /* 2:������Ч�ַ�.����2����Чֵ */
                return HI_FALSE;
            }
            val = (offset == 0) ? temp : ((val << 4) + temp); /* 4:λ�� */
            if (offset == 1) {
                mac[idx] = val;
                is_mac_valid = (val != 0) ? HI_TRUE : is_mac_valid;
                ++idx;
            }
            ++offset;
        } else if (offset != 2) { /* 2:������Ч�ַ�,����2����Чֵ */
            return HI_FALSE;
        } else if (str[i] == ':') {
            offset = 0;
        } else {
            return HI_FALSE;
        }
    }

    return ((idx == size) && is_mac_valid) ? HI_TRUE : HI_FALSE;
}

hi_bool split(hi_u8 *src, const hi_char *separator, hi_char **dest, hi_u8 num)
{
    char *next = HI_NULL;
    int count = 0;

    if ((src == HI_NULL) || (strlen((const hi_char *)src) == 0) || (separator == HI_NULL) || (strlen(separator) == 0)) {
        return HI_FALSE;
    }
    next = oal_strtok((char *)src, separator);
    while (next != HI_NULL) {
        *(dest++) = next;
        ++count;
        if (count == num) {
            break;
        }
        next = oal_strtok(HI_NULL, separator);
    }
    if (count < num) {
        return HI_FALSE;
    }
    return HI_TRUE;
}

hi_bool split_cmd_paras(const hi_char *cmd_para, hi_u8 size, hi_u32 *data, hi_u8 data_size, hi_bool by_hex)
{
    hi_u8 i;
    hi_char *stop = HI_NULL;
    hi_u8 src[DOWNLOAD_CMD_PARA_LEN + 1] = {0};
    hi_char *revbuf[CMD_SUB_PARA_CNT_MAX] = {0};

    if (data_size > CMD_SUB_PARA_CNT_MAX) {
        return HI_FALSE;
    }

    memcpy_s(src, sizeof(src), cmd_para, size);

    if (!split(src, ",", revbuf, data_size)) {
        return HI_FALSE;
    }
    for (i = 0; i < data_size; ++i) {
        if (by_hex) {
            data[i] = (hi_u32)oal_strtol(revbuf[i], &stop, 16); /* 16:��16���ƽ��� */
        } else {
            data[i] = (hi_u32)oal_strtol(revbuf[i], &stop, 10); /* 10:��10���ƽ��� */
        }
    }
    return HI_TRUE;
}

hi_bool get_cfg_idx(const hi_char *cfg_name, hi_u8 *idx)
{
    hi_u8 i;
    /* ���Ҳ������Ƿ񻺴� */
    for (i = 0; i < CFG_CMD_NUM_MAX; ++i) {
        if (strcmp(cfg_name, (const hi_char *)g_cus_cfg_cmd[i].cmd_name) == 0) {
            break;
        }
    }
    *idx = i;
    return HI_TRUE;
}

hi_bool cfg_get_mac(hi_u8 *mac_addr, hi_u8 size)
{
    hi_u8 idx;

    if (!get_cfg_idx((const hi_char *)WIFI_CFG_MAC, &idx)) {
        return HI_FALSE;
    }

    return parse_mac_addr(g_cus_cfg_cmd[idx].cmd_para,
                          strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para),
                          mac_addr, size);
}

hi_u32 cfg_dbb(hi_void)
{
    const hi_u8 data_size = 7; /* 7:�������� */
    hi_u32 data[data_size];
    hi_u8 idx;
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* ������������ */
    if (!get_cfg_idx(WIFI_CFG_DBB_PARAMS, &idx)) {
        return HI_FAIL;
    }
    /* �������� */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_TRUE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_dbb:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_dbb(data, data_size);
}

hi_u32 cfg_country_code(hi_void)
{
    const hi_u8 size = 3;
    hi_char data[size];
    hi_u8 idx;

    /* ������������ */
    if (!get_cfg_idx(WIFI_CFG_COUNTRY_CODE, &idx)) {
        return HI_FAIL;
    }
    if (strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para) < size - 1) {
        return HI_FAIL;
    }
    /* ����������,˳�򽻲� */
    data[0] = g_cus_cfg_cmd[idx].cmd_para[1];
    data[1] = g_cus_cfg_cmd[idx].cmd_para[0];
    data[2] = '\0'; /* �±�2 */

    return wal_cfg_country_code(data, size);
}

hi_u32 cfg_tx_pwr_offset(hi_void)
{
    hi_u8 idx;
    const hi_u8 data_size = 13; /* 13:�������� */
    hi_u32 data[data_size];
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* ������������ */
    if (!get_cfg_idx(WIFI_CFG_CH_TXPWR, &idx)) {
        return HI_FAIL;
    }
    /* �������� */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_TRUE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_tx_pwr_offset:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_fcc_tx_pwr(data, data_size);
}

hi_u32 cfg_freq_comp_val(hi_void)
{
    hi_u8 idx;
    const hi_u8 data_size = 3; /* 3:�������� */
    hi_u32 data[data_size];
    hi_bool ret;

    memset_s(data, sizeof(data), 0, sizeof(data));
    /* ������������ */
    if (!get_cfg_idx(WIFI_CFG_FREQ_COMP, &idx)) {
        return HI_FAIL;
    }
    /* �������� */
    ret = split_cmd_paras((const hi_char *)g_cus_cfg_cmd[idx].cmd_para,
        strlen((const hi_char *)g_cus_cfg_cmd[idx].cmd_para), data, data_size, HI_FALSE);
    if (ret != HI_FAIL) {
        oam_error_log0(0, 0, "cfg_freq_comp_val:: split_cmd_paras failed");
        return ret;
    }

    return wal_cfg_freq_comp_val(data, data_size);
}

hi_u32 cfg_rssi_ofset(hi_void)
{
    hi_s32 data;
    hi_char *stop = HI_NULL;
    hi_u8 idx;

    /* ������������ */
    if (!get_cfg_idx(WIFI_CFG_RSSI_OFFSET, &idx)) {
        return HI_FAIL;
    }
    data = oal_strtol((const hi_char *)g_cus_cfg_cmd[idx].cmd_para, &stop, 10); /* 10:��10���ƽ��� */
    return wal_cfg_rssi_ofset(data);
}

hi_u32 firmware_sync_cfg_paras_to_wal_customize(hi_void)
{
    /* ͬ��dbb scale�������� */
    if (cfg_dbb() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_dbb failed");
        return HI_FAIL;
    }
    /* ͬ�������� */
    if (cfg_country_code() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_country_code failed");
        return HI_FAIL;
    }
    /* ͬ��FCC�������� */
    if (cfg_tx_pwr_offset() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_tx_pwr_offset failed");
        return HI_FAIL;
    }
    /* ͬ������Ƶƫ�������� */
    if (cfg_freq_comp_val() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_dbb failed");
        return HI_FAIL;
    }
    /* ͬ��rssi�������� */
    if (cfg_rssi_ofset() != HI_SUCCESS) {
        oam_error_log0(0, 0, "firmware_sync_cfg_paras_to_wal_customize:: cfg_rssi_ofset failed");
        return HI_FAIL;
    }

    printk("firmware_sync_cfg_paras_to_wal_customize SUCCESSFULLY\r\n");
    return HI_SUCCESS;
}

EXPORT_SYMBOL(plat_firmware_clear);
EXPORT_SYMBOL(get_device_soft_version);

