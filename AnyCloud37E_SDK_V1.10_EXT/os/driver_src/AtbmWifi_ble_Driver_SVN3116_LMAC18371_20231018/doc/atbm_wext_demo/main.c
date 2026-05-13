/********************
V7.3：增加 ap list 扫描时候加密算法类型的获取以及打印
		struct _scan_ap_info_ 结构体增加参数 unsigned char enc_type_name
		该参数按位对应的加密算法类型为：enc_type_name_string 这个数组
		例如：
			for(i=0;i<sizeof(enc_type_name);i++)
				if(enc_type_name & 1<<i)
					printf("enc_type_name:%s \n",enc_type_name_string[i]);
V7.4:
	设置功率ATBM_DEV_IO_SET_TXPWR参数和驱动没匹配上

V7.5:
	增加设置国家码，目前只有美国US，日本JP，中国CN

V7.6:
	增加获取驱动版本
	获取efuse
	获取 start rx 结果

V7.7:
	ATBM_DEV_IO_SET_TXPWR
	杩欎釜鍑芥暟瀵瑰簲鐨勬槸iwpriv wlan0 fwcmd set_rate_txpower_mode,[idx]
V7.8:
	ATBM_DEV_IO_SET_AUTO_CALI_PPM	= 37,
	ATBM_DEV_IO_GET_CALI_REAULTS	= 38,
	ATBM_DEV_IO_SET_EFUSE_GAIN_COMPENSATION_VALUE = 39
V7.9:
	set efuse dcxo & deltagain add parameters write rom
V8.0:
	set sta channel listen 
	ATBM_DEV_IO_SET_STA_LISTEN_CHANNEL = 41,
V8.1:
	altm_cmd_anker_wdt
	ATBM_DEV_IO_SET_ANKER_WTD_CONTROL = 44
v8.2:
	altm_cmd_get_chip_name
	ATBM_DEV_IO_GET_CHIP_NAME = 45,
v8.3:
	altm_cmd_ant_control
	ATBM_DEV_IO_SET_ANT_CONTROL = 46
v8.4:
	altm_cmd_get_ap_list
	change enc type name ,u8 -> u32
v8.5:
	altm_cmd_get_ap_list
	Compatible with earlier driver configurations
	SCAN_AP_INFO->flag = 1 ==> wifi driver is old version
	SCAN_AP_INFO->flag = 2 ==> support display SAE and PSK 
**********************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/rtnetlink.h>
#include <netpacket/packet.h>
#include <linux/filter.h>
#include <linux/errqueue.h>
#include "atbm_ioctrl.h"

//#define unsigned char u8


#define VERSION "ATBM_V_8.5_20231027"
struct nl80211_global g_nl80211_global_s;

#define MAC2STR(a) (a)[0],(a)[1],(a)[2],(a)[3],(a)[4],(a)[5]
#define MAC2STR2HEX(a) atoi((a)[0]),atoi((a)[1]),atoi((a)[2]),atoi((a)[3]),atoi((a)[4]),atoi((a)[5])
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

//---------------------------------------------------------------------------------------
#define WSM_MAX_NUM_LINK_STA 14//Lmac support station number is 8;
typedef struct _atbm_wifi_ap_info_{
	int wext_rssi;              //信号强度
	unsigned long rx_packets;    //收包数量
	unsigned long tx_packets;    //发包数量
	unsigned long tx_retry_count; //重传次数
	int last_rx_rate_idx;         //收包速率
	unsigned char  wext_mac[6]; //Station 的MAC 地址
	unsigned char  sta_cnt;     //已经连接的AP个数 
}atbm_wifi_ap_info;
atbm_wifi_ap_info atbm_ap_info[WSM_MAX_NUM_LINK_STA];

//-----------------------------------------------------------------------------------------
typedef struct _atbm_wifi_sta_info_{
	int rssi;
    unsigned long rx_packets;
    unsigned long tx_packets;
    unsigned short channel_idle;
    u8  bssid[6];
    u8  ssid[32];
    u8  ssid_len;
}atbm_wifi_sta_info;
atbm_wifi_sta_info atbm_sta_info;
//-----------------------------------------------------------------------------------------
#define MAC_FILTER_NUM 16
typedef struct _Wifi_Recv_Info
{
	unsigned char Ssid[32];
	unsigned char channel;
	unsigned int   Rssi;
	unsigned int Quality;
	unsigned char phy_noise;
	unsigned char Bssid[6];	
	unsigned char User_data[256]; 
}Wifi_Recv_Info_t;



Wifi_Recv_Info_t private_ap_info[MAC_FILTER_NUM];
//-----------------------------------------------------------------------------------------
#define AP_SCAN_NUM_MAX 32 //ap number per channel
#define CHANNEL_NUM 14 //channel
typedef struct _scan_ap_info_
{
	unsigned char ssid[32];
	unsigned char mac_addr[6];
	unsigned char rssi;
	unsigned char flag;
	unsigned char enc_type;
	unsigned int enc_type_name;
}SCAN_AP_INFO;
typedef struct _scan_ap_info_old_
{
	unsigned char ssid[32];
	unsigned char mac_addr[6];
	unsigned char rssi;
	unsigned char flag;
	unsigned char enc_type;
	unsigned char enc_type_name;
}SCAN_AP_INFO_OLD;


const char enc_string[][10]={
	{"OPEN"},
	{"WPA"},
	{"WPA2"},
	{"WPA/WPA2"},
	{"WEP"}
	
	
};
const char enc_type_name_string[][16]={
	{"NONE"},
	{"WEP-40"},
	{"WEP-104"},
	{"TKIP"},
	{"CCMP"},
	{"WEP-104"},
	{"AES_128_CMAC"},
	{"GCMP"},
	{"SMS4"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
	{"NONE"},
};

const char enc_type_auth_key_name_string[][32]={
	{"IEEE8021X"},
	{"PSK"},
	{"NONE"},
	{"IEEE8021X_NO_WPA"},
	{"WPA_NONE"},
	{"FT_IEEE8021X"},
	{"FT_IEEE8021X"},
	{"IEEE8021X_SHA256"},
	{"PSK_SHA256"},
	{"WPS"},
	{"SAE"},
	{"FT_SAE"},
	{"WAPI_PSK"},
	{"WAPI_CERT"},
	{"CCKM"},
	{"NONE"},
	{"NONE"},

};

const int rate_val[]={
10,20,55,110,
60,90,120,180,240,360,480,540,
65,130,195,260,390,520,585,650	
};
SCAN_AP_INFO scan_ap_info_buff[CHANNEL_NUM][AP_SCAN_NUM_MAX];
SCAN_AP_INFO_OLD (*scan_ap_info_old_buff)[AP_SCAN_NUM_MAX] = scan_ap_info_buff;
	//[CHANNEL_NUM][AP_SCAN_NUM_MAX];

//-----------------------------------------------------------------------------------------

typedef struct _best_ch_scan_result_
{
	unsigned int channel_ap_num[18]; //每个信道的AP 个数
	unsigned int busy_ratio[18];      //每个信道的繁忙比例
	unsigned int weight[18];
	unsigned char suggest_ch;       //建议的最优信道号
}Best_Channel_Scan_Result;
Best_Channel_Scan_Result best_chan_results;

//-----------------------------------------------------------------------------------------
struct efuse_headr{
	unsigned char specific;
	unsigned char version;
	unsigned char dcxo_trim;
	unsigned char delta_gain1;
	unsigned char delta_gain2;
	unsigned char delta_gain3;
	unsigned char Tj_room;
	unsigned char topref_ctrl_bias_res_trim;
	unsigned char PowerSupplySel;
	unsigned char mac[6];
};

//-----------------------------------------------------------------------------------------
struct rx_results{
	unsigned int  rxSuccess;
	unsigned int FcsErr;
	unsigned int PlcpErr;
};

//-----------------------------------------------------------------------------------------
enum atbm_msg_type{
	ATBM_DEV_IO_GET_STA_STATUS   = 0,
	ATBM_DEV_IO_GET_STA_RSSI     = 1, //STA connected AP's RSSI
	ATBM_DEV_IO_GET_AP_INFO      = 2,  //STA or AP
	ATBM_DEV_IO_GET_STA_INFO     = 3,
	ATBM_DEV_IO_SET_STA_SCAN     = 4,
	ATBM_DEV_IO_SET_FREQ         = 5,
	ATBM_DEV_IO_SET_SPECIAL_OUI  = 6, //use for Beacon and Probe package
	ATBM_DEV_IO_SET_STA_DIS      = 7,
	ATBM_DEV_IO_SET_IFTYPE      = 8,
	ATBM_DEV_IO_SET_ADAPTIVE     = 9,
	ATBM_DEV_IO_SET_TXPWR_DCXO   = 10,
	ATBM_DEV_IO_SET_TXPWR        = 11,
	ATBM_DEV_IO_GET_WORK_CHANNEL   = 12,
	ATBM_DEV_IO_SET_BEST_CHANNEL_SCAN   = 13,
	ATBM_DEV_IO_GET_AP_LIST   = 14,
	ATBM_DEV_IO_GET_TP_RATE   = 15,
	ATBM_DEV_IO_ETF_START_TX	= 18,
    ATBM_DEV_IO_ETF_STOP_TX		= 19,
    ATBM_DEV_IO_ETF_START_RX	= 20,
    ATBM_DEV_IO_ETF_STOP_RX		= 21,
	ATBM_DEV_IO_FIX_TX_RATE		 = 22,
    ATBM_DEV_IO_MAX_TX_RATE		 = 23,
    ATBM_DEV_IO_TX_RATE_FREE	 = 24,
	ATBM_DEV_IO_SET_EFUSE_MAC    = 25,
	ATBM_DEV_IO_SET_EFUSE_DCXO   = 26,
	ATBM_DEV_IO_SET_EFUSE_DELTAGAIN = 27,
	ATBM_DEV_IO_MIN_TX_RATE          = 28,
    ATBM_DEV_IO_SET_RATE_POWER       = 29,
	ATBM_DEV_IO_SET_SPECIAL_FILTER = 30,
	ATBM_DEV_IO_SET_COUNTRY_CODE	= 31,
	ATBM_DEV_IO_GET_DRIVER_VERSION = 32,
	ATBM_DEV_IO_GET_EFUSE			= 33,
	ATBM_DEV_IO_GET_ETF_START_RX_RESULTS = 34,
	ATBM_DEV_IO_SET_FIX_SCAN_CHANNEL = 36,
	ATBM_DEV_IO_SET_AUTO_CALI_PPM	= 37,
	ATBM_DEV_IO_GET_CALI_REAULTS	= 38,
	ATBM_DEV_IO_SET_EFUSE_GAIN_COMPENSATION_VALUE = 39,
	ATBM_DEV_IO_GET_VENDOR_SPECIAL_IE	= 40,
	ATBM_DEV_IO_SET_STA_LISTEN_CHANNEL = 41,
	ATBM_DEV_IO_SET_ANKER_WTD_CONTROL = 44,
	ATBM_DEV_IO_GET_CHIP_NAME = 45,
	ATBM_DEV_IO_SET_ANT_CONTROL = 46,
};

/*
msg.type:
	1 filter_frame , 80 , 40

	2 filter_ie 
	
	3 filter clean

	4 filter show

msg.value:
	filter_frame : 80 or 40
	filter_ie	 : ie

msg.externData:	
	filter_ie	 : oui1 oui2 pui3
*/
enum SPECIAL_FILTER_TYPE{
	FILTER_FRAME = 1,
	FILTER_IE	 = 2,
	FILTER_CLEAR = 3,
	FILTER_SHOW	 = 4,
};



struct altm_cmd{
	const char *cmd;
	int (*handler)(struct nl80211_global *nl_connect, int argc, char *argv[]);
	const char *uage;
};

void print_usage (void);


/*

*/
int altm_cmd_special_filter(struct nl80211_global *nl_connect, int argc, char *argv[])
{

	int ret = -1;
	int status;
	struct altm_wext_msg msg;
	
	memset(&msg,0,sizeof(msg));
	
	
	msg.type = ATBM_DEV_IO_SET_SPECIAL_FILTER;
/*
	 msg.value = atoi(argv[0]);
	switch(msg.value)
	{
		case FILTER_FRAME:{
			if(argc > 1)
				msg.externData[0] = atoi(argv[1]);
		}break;
        	case FILTER_IE:{
			if(argc>1)
				msg.externData[0] = atoi(argv[1]);
			if(argc>4){
				msg.externData[1] = atoi(argv[2]);
				msg.externData[2] = atoi(argv[3]);
				msg.externData[3] = atoi(argv[4]);
			}
		}break;
        	case FILTER_CLEAR:{
		}break;
        	case FILTER_SHOW:{
		}break;
		default:
			break;
	}
*/
	if(argc > 0)
		msg.value = atoi(argv[0]);
	if(argc > 1)
		msg.externData[0] = atoi(argv[1]);
	if(argc > 2)
		msg.externData[1] = atoi(argv[2]);
 	if(argc > 3)
		msg.externData[2] = atoi(argv[3]);
	if(argc > 4)
		msg.externData[3] = atoi(argv[4]);


	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
        if(ret != 0){
                printf("error, ioctrl send,ret = %d\n",ret);
        }
	if(msg.value == FILTER_SHOW)
		printf("[show] %s \n ",msg.externData);
	return ret; 

}

int altm_cmd_set_country_code(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int status;
	struct altm_wext_msg msg;

	if (argc > 3){
		printf("altm_cmd_set_country_code,No need more argument\n");
		return ret;
	}
	
	memset(&msg,0,sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_COUNTRY_CODE;
	memcpy(msg.externData,argv[0],2);
	printf(" msg.externData [%c%c] \n",msg.externData[0],msg.externData[1]);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
    if(ret != 0){
            printf("altm_cmd_country_code error, ioctrl send\n");
    }
	return 0;
}


int altm_cmd_get_status(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status;

	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_GET_STA_STATUS;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	status = msg.externData[0];
	printf("atbm_test: connect status %d \n", status);


	return ret;
}

int altm_cmd_get_rssi(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int rssi;
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_GET_STA_RSSI;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &rssi);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	memcpy(&rssi, msg.externData, sizeof(rssi));
	
	printf("atbm_test: rssi = %d \n", rssi);

	return ret;
}

int altm_cmd_get_ap_info(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	int i;
	int addr_val = (int)(&atbm_ap_info[0]);
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(atbm_ap_info, 0, sizeof(atbm_ap_info));
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_AP_INFO;
	memcpy(&msg.externData[0], &addr_val, sizeof(int));
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	for(i=0; i<WSM_MAX_NUM_LINK_STA; i++){
		printf("[%d] %d "MACSTR"\n",  i, atbm_ap_info[i].wext_rssi, MAC2STR(atbm_ap_info[i].wext_mac));
		printf("rx packets num : %d \n",atbm_ap_info[i].rx_packets);
		printf("tx packets num : %d \n",atbm_ap_info[i].tx_packets);
		printf("tx retry num : %d \n",atbm_ap_info[i].tx_retry_count);
		printf("last tx rate : %d \n",atbm_ap_info[i].last_rx_rate_idx);
		printf("station num : %d \n",atbm_ap_info[i].sta_cnt);
	}
	
	return ret;
}
int altm_cmd_get_sta_info(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&atbm_sta_info, 0, sizeof(atbm_sta_info));
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_STA_INFO;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	memcpy((u8*)&atbm_sta_info, (u8*)msg.externData, sizeof(atbm_sta_info));

	printf("bssid: "MACSTR"\n", MAC2STR(atbm_sta_info.bssid));
	printf("ssid(%d): %s\n", atbm_sta_info.ssid_len, atbm_sta_info.ssid);
	printf("rssi: %d\n", atbm_sta_info.rssi);
	printf("rx packets num : %d \n",atbm_sta_info.rx_packets);
	printf("tx packets num : %d \n",atbm_sta_info.tx_packets);
	printf("channel idle : %d \n",atbm_sta_info.channel_idle);
	
	return ret;
}

int altm_cmd_get_chip_name(struct nl80211_global *nl_connect)
{
	int i;
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;

	msg.type = ATBM_DEV_IO_GET_CHIP_NAME;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
		return -1;
	}
	if(memcmp(msg.externData,"ATBM",4) == 0){
		printf("%s is WIFI6\n",msg.externData);
		return 16;
	}else{
		printf("%s is WIFI4\n",msg.externData);
		return 8;
	}
	return 0;
}


int altm_cmd_set_sta_scan(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int i;
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	unsigned short channel = 0;
	unsigned short support_sta_num = 0;
	int addr_val = (int)(&private_ap_info[0]);
	char filter_mac[MAC_FILTER_NUM][6];

	printf("%s\n", __func__);
	memset(&filter_mac[0][0],0,MAC_FILTER_NUM*6);
	support_sta_num = altm_cmd_get_chip_name(nl_connect);

	if(support_sta_num <= 0){
		printf("%s get support sta num err!\n", __func__);
		return -1;
	}
	
	for(i = 0;i < argc - 1 && i < support_sta_num; i++){
		sscanf(argv[1+i],"%02x:%02x:%02x:%02x:%02x:%02x",&filter_mac[i][0],&filter_mac[i][1],
												&filter_mac[i][2],&filter_mac[i][3],
												&filter_mac[i][4],&filter_mac[i][5]);
		printf(MACSTR,MAC2STR(filter_mac[i]));
		
	}
	channel = (unsigned short)atoi(argv[0]);
	if(channel > 14 || channel < 0){
		printf("Invalie parameter %d\n", channel);
		return ret;
	}
	memset(private_ap_info, 0, sizeof(Wifi_Recv_Info_t));
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_STA_SCAN;
	//[0:1] channel
	memcpy(&msg.externData[0], &channel, (sizeof(channel)));
	//[2:49] mac address * 8
	memcpy(&msg.externData[2], &filter_mac[0][0], support_sta_num*6);
	//[50:53] memory addr


	memcpy(&msg.externData[2 + support_sta_num*6], &addr_val, sizeof(int));
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	for(i=0; i<support_sta_num; i++){
		printf("[%d] "MACSTR"\n",  i, MAC2STR(private_ap_info[i].Bssid));
		printf("ssid : %s \n", private_ap_info[i].Ssid);
		printf("channel : %d \n",private_ap_info[i].channel);
		printf("rssi : %d \n",private_ap_info[i].Rssi);
		printf("quality : %d \n",private_ap_info[i].Quality);
		printf("noise : %d \n",private_ap_info[i].phy_noise);
		printf("user data : %s \n",private_ap_info[i].User_data);
	}
	return ret;
}

int altm_cmd_set_freq(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	unsigned short channel = 0;
	int freq = 0;
	
	printf("%s\n", __func__);

	if (argc != 2){
		printf("No need more argument\n");
		return ret;
	}

	channel = (unsigned short)atoi(argv[0]);
	if(channel > 14 || channel < 0){
		printf("Invalie parameter %d\n", channel);
		return ret;
	}

	freq = (unsigned short)atoi(argv[1]);
	printf("freq %d\n", freq);

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_FREQ;
	//msg.value = (freq << 4) | (channel & 0xF);
	memcpy(&msg.externData[0], &channel, sizeof(unsigned short));
	memcpy(&msg.externData[2], &freq, sizeof(int));
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_set_special_oui(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	unsigned int length = 0;
	
	printf("%s\n", __func__);

	if (argc != 1){
		printf("No need more argument\n");
		return ret;
	}

	length = strlen(argv[0]);
	if(length > 255){
		printf("Invalid length %d\n", length);
		return ret;
	}

	printf("insert data: %s\n", argv[0]);

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_SPECIAL_OUI;
	memcpy(msg.externData, argv[0], length);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_set_sta_dis(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;

	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_STA_DIS;

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}

int altm_cmd_set_monitor(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	char ch_type = 0;
	int chan = 0,freq = 0;

	printf("%s\n", __func__);
	if (argc < 2){
		printf("No need more argument\n");
		return ret;
	}
/****
	enum	nl80211_channel_type { 
					IEEE80211_INTERNAL_IFTYPE_REQ__MANAGED,
					IEEE80211_INTERNAL_IFTYPE_REQ__MONITOR,
 }
	*****/
	ch_type = (char)atoi(argv[0]);
	if(ch_type > 2 || ch_type < 0){
		printf("Invalid ch type %d\n", ch_type);
		return ret;
	}
	
	chan = (int)atoi(argv[1]);
	if(chan > 14 || chan < 0){
		printf("Invalid channel %d\n", chan);
		return ret;
	}
	printf("freq %d\n", freq);
//	freq = 2412 + (chan - 1)*5;
	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_IFTYPE;
	//msg.value = (freq << 8) | (ch_type & 0xFF);
	memcpy(&msg.externData[0], &ch_type, sizeof(char));
	memcpy(&msg.externData[1], &chan, sizeof(int));

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_set_adaptive(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	int adaptive = 0;

	printf("%s\n", __func__);

	if (argc != 1){
		printf("No need more argument\n");
		return ret;
	}

	adaptive = (int)atoi(argv[0]);
	if(adaptive != 0 && adaptive != 1){
		printf("Invalid adaptive %d\n", adaptive);
		return ret;
	}
	
	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_ADAPTIVE;
	//msg.value = adaptive;
	memcpy(&msg.externData[0], &adaptive, sizeof(int));

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_set_txpwer_dcxo(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	int txpwr_L = 0;
	int txpwr_M = 0;
	int txpwr_H = 0;
	int dcxo = 0;

	printf("%s\n", __func__);

	if (argc != 4){
		printf("No need more argument\n");
		return ret;
	}

	txpwr_L = (int)atoi(argv[0]);
	if(txpwr_L > 32 || txpwr_L < -32){
		printf("Invalid txpwr_L %d\n", txpwr_L);
		return ret;
	}
	txpwr_M = (int)atoi(argv[1]);
	if(txpwr_M > 32 || txpwr_M < -32){
		printf("Invalid txpwr_M %d\n", txpwr_M);
		return ret;
	}
	txpwr_H = (int)atoi(argv[2]);
	if(txpwr_H > 32 || txpwr_H < -32){
		printf("Invalid txpwr_H %d\n", txpwr_H);
		return ret;
	}	

	dcxo = (int)atoi(argv[3]);
	if(dcxo > 127 || dcxo < 0){
		printf("Invalid dcxo %d\n", dcxo);
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_TXPWR_DCXO;
	memcpy(&msg.externData[0], &txpwr_L, 4);
	memcpy(&msg.externData[4], &txpwr_M, 4);
	memcpy(&msg.externData[8], &txpwr_H, 4);
	
	memcpy(&msg.externData[12], &dcxo, 4);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_set_rate_txpwer_mode(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	int txpwr_indx = 0;


	printf("%s\n", __func__);


	txpwr_indx = (int)atoi(argv[0]);

	
	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_TXPWR;

	msg.externData[0] = txpwr_indx;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	
	return ret;
}
int altm_cmd_get_work_channel(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	unsigned short channel = 0;
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_GET_WORK_CHANNEL;

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	memcpy(&channel, &msg.externData[0], sizeof(unsigned short));
	printf("work channle %d\n", channel);
	
	return ret;
}
int altm_cmd_set_best_channel_scan(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int i;
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s argc = %d\n", __func__,argc);

	if (argc > 6){
		printf("No need more argument\n");
		return ret;
	}

	memset(&best_chan_results, 0, sizeof(best_chan_results));
	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_BEST_CHANNEL_SCAN;
	if(argv[0]){
		msg.externData[0] = atoi(argv[0]);
		if(argv[1])
			msg.externData[1] = atoi(argv[1]);
	}
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	memcpy(&best_chan_results, &msg.externData[0], sizeof(best_chan_results));

	for(i=0; i<18; i++){
		printf("ch:%d, ap num:%d, ratio:%d weight:%d\n", i+1, best_chan_results.channel_ap_num[i], best_chan_results.busy_ratio[i],best_chan_results.weight[i]);
	}

	printf("suggest channel %d\n", best_chan_results.suggest_ch);
	
	return ret;
}
#if 0
int altm_cmd_set_best_channel_end(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_SET_BEST_CHANNEL_END;

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	return ret;
}
int altm_cmd_set_best_channel_result(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	msg.type = ATBM_DEV_IO_GET_BEST_CHANNEL_RESULT;

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	return ret;
}
#endif

int altm_cmd_get_ap_list(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int i, j,k,cnt = 0,ssid_len;
	int ret = -1;
	struct altm_wext_msg msg;
	int status = 0;
	int addr_val = (int)(&scan_ap_info_buff[0][0]);
	char old_flag = 0;
	int enc_type = 0;
	printf("%s\n", __func__);

	if (argc != 0){
		printf("No need more argument\n");
		return ret;
	}

	memset(scan_ap_info_buff, 0, sizeof(scan_ap_info_buff));
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_AP_LIST;
	memcpy(&msg.externData[0], &addr_val, 4);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	if(scan_ap_info_buff[0][0].flag == 1){
		printf("old version! \n");
		old_flag = 1;
	}
	
	for(i=0; i<CHANNEL_NUM; i++){
		for(j=0; j<AP_SCAN_NUM_MAX; j++){
			if(old_flag == 1 && scan_ap_info_old_buff[i][j].flag == 1){
				printf("channel: %d\n", i+1);
				printf("MAC: "MACSTR"\n", MAC2STR(scan_ap_info_old_buff[i][j].mac_addr));
				printf("SSID:");
				for(ssid_len = 0;ssid_len < 32;ssid_len++)
					printf("%c",scan_ap_info_old_buff[i][j].ssid[ssid_len]);
				//printf("SSID: %s\n", scan_ap_info_buff[i][j].ssid);
				printf("\n");
				printf("RSSI: %d\n", scan_ap_info_old_buff[i][j].rssi-256);
				printf("encrypt type : %s \n",enc_string[scan_ap_info_old_buff[i][j].enc_type]);
				if(scan_ap_info_old_buff[i][j].enc_type_name){
					printf("encrypt type name :");
					for(k = 0;k < 8;k++){
						if(scan_ap_info_old_buff[i][j].enc_type_name & (1<<k)){
							printf(" %s ",enc_type_name_string[k]);
						}
					}
					printf("\n");
				}
				printf("\n");
			}else if(scan_ap_info_buff[i][j].flag == 2)
			{
				printf("%d \n",++cnt);
				printf("SSID:");
				for(ssid_len = 0;ssid_len < 32;ssid_len++)
					printf("%c",scan_ap_info_buff[i][j].ssid[ssid_len]);
				printf("\n");
				printf("channel: %d\n", i+1);
				printf("MAC: "MACSTR"\n", MAC2STR(scan_ap_info_buff[i][j].mac_addr));
				//printf("SSID: %s\n", scan_ap_info_buff[i][j].ssid);
				
				printf("RSSI: %d\n", scan_ap_info_buff[i][j].rssi-256);
				printf("encrypt type : %s \n",enc_string[scan_ap_info_buff[i][j].enc_type]);
				if(scan_ap_info_buff[i][j].enc_type_name){
					
					printf("encrypt type name :");
					enc_type = scan_ap_info_buff[i][j].enc_type_name & 0xffff;
					for(k = 0;k < 16;k++){
						if(enc_type & (1<<k)){
							printf(" %s ",enc_type_name_string[k]);
						}
					}
					enc_type = (scan_ap_info_buff[i][j].enc_type_name >> 16) & 0xffff;
					for(k = 0;k < 16;k++){
						if(enc_type & (1<<k)){
							printf(" %s ",enc_type_auth_key_name_string[k]);
						}
					}
					printf("\n");
				}
				printf("\n");
			}
		}
	}
	
	return ret;
}

int altm_cmd_get_tp_rate(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s argc = %d \n", __func__,argc);

	if (argc > 3){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	if(argv[0]){
		/*
		printf("argv[0] = %s \n",argv[0]);
		sscanf(argv[0],MACSTR,&msg.externData[0],&msg.externData[1],&msg.externData[2],&msg.externData[3],&msg.externData[4],&msg.externData[5]);
		printf("mac:"MACSTR"\n",MAC2STR(msg.externData) );
		*/
		memcpy(msg.externData,argv[0],17);
		
	}
	msg.type = ATBM_DEV_IO_GET_TP_RATE;
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	memcpy(&rate_val,&msg.externData[0],sizeof(int));
	printf("current channel rate_tp =%d bit/s \n",rate_val);

	return ret;
}

/*
11b:10,20,55,110
11g:60,90,120,180,240,360,480,540
11n:65,130,195,260,390,520,585,650


*/
int search_val(char *val)
{
	int i = 0;
	int value = atoi(val);
	for(i = 0; i < sizeof(rate_val)/sizeof(int);i++){
		if(value == rate_val[i])
			return value;
		
	}
	return 0;
}
int altm_cmd_set_max_rate(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val_t = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);
	rate_val_t = search_val(argv[0]);
	if (argc > 3 ){
		printf("No need more argument,rate_val_t = %d \n",rate_val_t);
		return ret;
	}
	
	
	
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_MAX_TX_RATE;
	msg.value = rate_val_t;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	

	return ret;
	
}

int altm_cmd_set_fix_rate(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val_t = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);
	rate_val_t = search_val(argv[0]);
	if (argc > 3){
		printf("No need more argument,rate_val_t = %d \n",rate_val_t);
		return ret;
	}
	

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_FIX_TX_RATE;
	msg.value = rate_val_t;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	

	return ret;
	
}
int altm_cmd_free_rate(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc > 3){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_TX_RATE_FREE;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
	
}
//ATBM_DEV_IO_MIN_TX_RATE          = 28,
  //  ATBM_DEV_IO_SET_RATE_POWER       = 29,
int altm_cmd_set_min_rate(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val_t = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);
	rate_val_t = search_val(argv[0]);
	if (argc > 3 ){
		printf("No need more argument,rate_val_t = %d \n",rate_val_t);
		return ret;
	}
	

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_MIN_TX_RATE;
	msg.value = rate_val_t;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	

	return ret;
	
}

int altm_cmd_set_rate_power(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int rate_val_t = 0;
	struct altm_wext_msg msg;
	int status = 0;
	int rate_index = 0;
	int power = 0;
	
	printf("%s\n", __func__);
	rate_index = atoi(argv[0]);
	power = atoi(argv[1]);
	if (argc > 3 || rate_index > 10 || rate_index < 0 || power > 16 || power < -16){
		printf("altm_cmd_set_rate_power error, argc = %d ,rate_index = %d , power = %d\n",argc,rate_index,power);
		return ret;
	}
	

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_RATE_POWER;
	msg.externData[0] = rate_index;
	msg.externData[1] = power;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	

	return ret;
	
}

int altm_cmd_set_efuse_mac(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc > 9){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
//printf("mac："MACSTR,MAC2STR2HEX(argv));
	sscanf(argv[0],"%02x:%02x:%02x:%02x:%02x:%02x",&msg.externData[0],&msg.externData[1],&msg.externData[2]
													,&msg.externData[3],&msg.externData[4],&msg.externData[5]);
	msg.type = ATBM_DEV_IO_SET_EFUSE_MAC;
	printf("mac : ");
	for(i = 0; i < 6 ; i++){
	//	msg.externData[i] = atoi(argv[i]);
		printf("%02x ",msg.externData[i]);
	}
	printf("\n");
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
	
}

int altm_cmd_set_efuse_dcxo(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct efuse_headr *efuse;
	printf("%s\n", __func__);

	if (argc > 3){
		printf("No need more argument\n");
		return ret;
	}

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_EFUSE_DCXO;
	
	msg.externData[0] = atoi(argv[0]);
	msg.value = atoi(argv[1]);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret < 0){
		printf("change efuse dcxo err!\n");
		return ret;
	}
	if(msg.value == 0){
		printf("change efuse dcxo success! \n");
	}else if(msg.value == 1){
		printf("the same dcxo value!\n");
	}
	efuse = (struct efuse_headr *)msg.externData;
	printf("version[%d] dcxo[%d] delta_gain[%d:%d:%d] mac:[" MACSTR "]\n",efuse->version,efuse->dcxo_trim,efuse->delta_gain1,efuse->delta_gain2,efuse->delta_gain3,MAC2STR(efuse->mac));

	return ret;
	
}

int altm_cmd_set_efuse_delta_gain(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	if (argc > 6){
		printf("No need more argument\n");
		return ret;
	}
	
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_EFUSE_DELTAGAIN;
	
	msg.externData[0] = atoi(argv[0]);
	msg.externData[1] = atoi(argv[1]);
	msg.externData[2] = atoi(argv[2]);
	msg.value = atoi(argv[3]);
	printf("gain = %02x : %02x : %02x , write=%d \n",msg.externData[0],msg.externData[1],msg.externData[2],msg.value);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
	
}

int altm_cmd_etf_start_tx(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	short *p;
	printf("%s\n", __func__);

	if (argc > 6){
		printf("No need more argument\n");
		return ret;
	}
	
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_ETF_START_TX;
	
	msg.externData[0] = atoi(argv[0]);
	printf("channel = %d ",msg.externData[0]);
	p = (short *)&msg.externData[1];
	*p = atoi(argv[1]);
	printf("rate = %d ",*p);
	p = (short *)&msg.externData[3];
	*p = atoi(argv[2]);
	printf("len = %d ",*p);
	msg.externData[5] = atoi(argv[3]);
	printf("is_40M = %d ",msg.externData[5]);
	msg.externData[6] = atoi(argv[4]);
	printf("greedfiled = %d \n",msg.externData[6]);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
}
int altm_cmd_etf_stop_tx(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);


	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_ETF_STOP_TX;
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
}
int altm_cmd_etf_start_rx(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_ETF_START_RX;
	
	msg.externData[0] = atoi(argv[0]);
	msg.externData[1] = atoi(argv[1]);
	printf("channel[%d],is_40M[%d] \n",msg.externData[0],msg.externData[1]);
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	return ret;
}
int altm_cmd_etf_stop_rx(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	printf("%s\n", __func__);


	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_ETF_STOP_RX;
	

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}


	return ret;
}
int altm_cmd_etf_start_rx_results(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct rx_results{
		u32  rxSuccess;
		u32 FcsErr;
		u32 PlcpErr;
	};
	struct rx_results *rx_results_t;
	printf("%s\n", __func__);

	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_ETF_START_RX_RESULTS;
	
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	rx_results_t = (struct rx_results *)msg.externData;
	printf("rxSuccess[%d] FcsErr[%d] PlcpErr[%d] \n",
			rx_results_t->rxSuccess,rx_results_t->FcsErr,rx_results_t->PlcpErr);

	return ret;
}
int altm_cmd_get_driver_version(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;

	struct altm_wext_msg msg;
	int status = 0;
	short *p1,*p2;
	printf("%s\n", __func__);


	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_DRIVER_VERSION;
	

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	p1 = (short *)&msg.externData[0];
	p2 = (short *)&msg.externData[2];
	printf("driver_version[%d],firmware_version[%d] \n",*p1,*p2);

	return ret;
}
int altm_cmd_get_efuse(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct efuse_headr *efuse_data;
	printf("%s\n", __func__);


	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_EFUSE;
	

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	efuse_data = (struct efuse_headr *)msg.externData;

	
	printf("dcxo[%d],gain1[%d],gain1[%d],gain1[%d]\n",efuse_data->dcxo_trim,
		efuse_data->delta_gain1,efuse_data->delta_gain2,efuse_data->delta_gain3 );
	printf("mac : "MACSTR"\n",MAC2STR(efuse_data->mac));
		
	return ret;
}

struct ieee80211_internal_ap_conf{
	u8 bssid[6];
	u8 ssid[32];
	u8 ssid_len;
	u8 channel;
	/*others password or enc type*/
};

int altm_cmd_set_fix_channel(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct ieee80211_internal_ap_conf conf_req;


	memset(&msg, 0, sizeof(msg));
	memset(&conf_req, 0, sizeof(conf_req));
	
	conf_req.channel = atoi(argv[0]);
	msg.type = ATBM_DEV_IO_SET_FIX_SCAN_CHANNEL;

	memcpy(msg.externData,&conf_req,sizeof(conf_req));

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	
	return ret;
	
}
int altm_cmd_set_ppm_cali(struct nl80211_global *nl_connect, int argc, char *argv[])
{	
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	msg.type = ATBM_DEV_IO_SET_AUTO_CALI_PPM;
	msg.value = atoi(argv[0]);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}

	
	return ret;
	
}

int altm_cmd_get_ppm_cali_reaults(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	int dcxo = -1;
	int cfo_val;
	msg.type = ATBM_DEV_IO_GET_CALI_REAULTS;
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	if(msg.externData[0] == 2)
		dcxo = -1;
	else
		dcxo = msg.externData[1];
	
	cfo_val = msg.externData[2];
	if(cfo_val > 128)
		cfo_val -= 256;
	
	printf("%s , dcxo:%d ,cfo:%d ppm\n",msg.externData[0] == 2?"share crystal, dcxo Setting is invalid ":"independent crystal",dcxo,cfo_val);
	
	return ret;
}
int altm_cmd_set_compensation_val(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	int status = 0;
	struct altm_wext_msg msg;
	float compensation_data;
	int val;
	msg.type = ATBM_DEV_IO_SET_EFUSE_GAIN_COMPENSATION_VALUE;
	compensation_data = atof(argv[0]);
	compensation_data *= 4;
	printf("compensation_data = %f \n",compensation_data);
	val = (int)compensation_data;
	msg.externData[0] = val;
	printf("msg.externData[0] = %d , val = %d\n",msg.externData[0],val);
	msg.value = atoi(argv[1]);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	return ret;
}
int altm_cmd_get_vendor_ie(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct atbm_vendor_cfg_ie *private_ie;
	printf("%s\n", __func__);


	memset(&msg, 0, sizeof(msg));
	

	msg.type = ATBM_DEV_IO_GET_VENDOR_SPECIAL_IE;
	

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	private_ie = (struct atbm_vendor_cfg_ie *)msg.externData;

	
	if(private_ie->ie_id == 221){
		printf("ssid[%s] pwd[%s] \n",private_ie->ssid,private_ie->password);
		ret = 0;
	}else{
		printf("not recive special vendor data \n");
		ret = -1;
	}
		
	return ret;
	

}


int altm_cmd_set_sta_listen_channel(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	
	
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_STA_LISTEN_CHANNEL;
	msg.value = atoi(argv[0]);

	printf("set listen channel = %d \n",msg.value);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
	}
	return 0;
}

int altm_cmd_test_listen_ie(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	int ret = -1;
	int i = 0;
	struct altm_wext_msg msg;
	int status = 0;
	struct atbm_vendor_cfg_ie *private_ie;
	printf("start +++++++++++++++++++++ ,set listen\n");
	
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_STA_LISTEN_CHANNEL;
	msg.value = atoi(argv[0]);

	printf("set listen channel = %d \n",msg.value);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
		return ret;
	}

	printf("================read value!\n");
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_GET_VENDOR_SPECIAL_IE;
	
	while(1){
		ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
		if(ret != 0){
			printf("error, ioctrl send\n");
		}
		private_ie = (struct atbm_vendor_cfg_ie *)msg.externData;

		
		if(private_ie->ie_id == 221){
			printf("ssid[%s] pwd[%s] \n",private_ie->ssid,private_ie->password);
			ret = 0;
			break;
		}else{
			//printf("not recive special vendor data \n");
			ret = -1;
		}
	}
	printf("+++++++++stop listen!\n");
	memset(&msg, 0, sizeof(msg));
	
	msg.type = ATBM_DEV_IO_SET_STA_LISTEN_CHANNEL;
	msg.value = 0;

	//printf("set listen channel = %d \n",msg.value);
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
		return ret;
	}
	printf("------------stop listen!\n");
	return 0;
}

int altm_cmd_anker_wdt(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	struct altm_wext_msg msg;
	int ret = 0,status = 0;
	memset(&msg, 0, sizeof(msg));

	// ATBM_DEV_IO_SET_ANKER_WTD_CONTROL = 44
	msg.type = ATBM_DEV_IO_SET_ANKER_WTD_CONTROL; 
	msg.value = 0;

	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
		return ret;
	}

	return 0;

}
int altm_cmd_ant_control(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	struct altm_wext_msg msg;
	int status = 0,ret = 0,start,scan_control;	
	int rssi = 0;
	msg.type = ATBM_DEV_IO_SET_ANT_CONTROL; 
	msg.value = 0;

	start = atoi(argv[0]);
	scan_control = atoi(argv[1]);
	msg.externData[0] = start;
	msg.externData[1] = scan_control;
	
	ret = ioctrl_wext_send_ack(nl_connect, &msg, &status);
	if(ret != 0){
		printf("error, ioctrl send\n");
		return ret;
	}
	
	if(start == 0){
		rssi = msg.value;
		printf("altm_cmd_ant_control: rssi = %d \n",rssi);
	}
	
	return 0;

}






int altm_cmd_help(struct nl80211_global *nl_connect, int argc, char *argv[])
{	
	print_usage();	
	return 0;
}

static struct altm_cmd altm_commands[]={
	{"-h", altm_cmd_help,
				"get help information"},
	{"status", altm_cmd_get_status,
				"= get wifi connect status"},
	{"rssi", altm_cmd_get_rssi,
				"= get latest rssi value"},
	{"ap_info", altm_cmd_get_ap_info,
				"= get wifi ap information--ap mode"},
	{"sta_info", altm_cmd_get_sta_info,
				"= get wifi sta information--sta mode"},
	{"private_scan", altm_cmd_set_sta_scan,
				"= start private scan([ch] 1~14, 0:all channel scan"},
	{"set_freq", altm_cmd_set_freq,
				"= set special freq([freq] 2300~2400 2407~2489 2500~2600)"},
	{"set_oui", altm_cmd_set_special_oui,
				"= set special oui([data] 0~255 bytes)"},
	{"sta_dis", altm_cmd_set_sta_dis,
				"= set station deauth"},
	{"monitor", altm_cmd_set_monitor,
				"= set to monitor mode([ch_type] 0 manager,1monitor [channel]1~14)"},
	{"set_adaptive", altm_cmd_set_adaptive,
				"= set adaptive mode([mode] 0, 1)"},
	{"set_txpwr_dcxo", altm_cmd_set_txpwer_dcxo,
				"= set tx power and dcxo([txpwr] -32~32 [dcxo] 0~127)"},
	{"set_rate_txpwr", altm_cmd_set_rate_txpwer_mode,
				"= set rate tx power mode([txpwr_idx] txpwr_id [-16,16])"},
	{"get_work_channel", altm_cmd_get_work_channel,
				"= get work channel"},
	{"best_ch_scan", altm_cmd_set_best_channel_scan,
				"= atbm_wext wlan0 best_ch_scan start_chan end_chan , start_chan&end_chan allow NULL"},
	//{"best_ch_end", altm_cmd_set_best_channel_end,
	//			"= set best channel scan end"},
	//{"best_ch_result", altm_cmd_set_best_channel_result,
	//			"= set best channel scan result"},
	{"ap_list", altm_cmd_get_ap_list,
				"= get ap list"},
	{"tp_rate", altm_cmd_get_tp_rate,
				"= get tp rate"},
	
	{"max_rate", altm_cmd_set_max_rate,
				"= set max_tx rate,default:0 "},
	{"fix_rate", altm_cmd_set_fix_rate,
				"= set fix_tx rate,default:0 "},
	{"free_rate", altm_cmd_free_rate,
				"= set default_tx rate"},
				
	{"set_efuse_mac", altm_cmd_set_efuse_mac,
				"= set efuse mac addr"},
	{"set_efuse_dcxo", altm_cmd_set_efuse_dcxo,
				"= set efuse dcxo to change ppm,param1:dcxo_value,param2:write rom"},
	{"set_efuse_delta_gain", altm_cmd_set_efuse_delta_gain,
				"= set efuse txpower , low_chan,midle_chan,high_chan,param1~3:deltagain_value,param2:write rom"},
	{"min_rate", altm_cmd_set_min_rate,
				"= set min tx rate ,default:0 "},
	{"set_rate_power", altm_cmd_set_rate_power,
				"= set_rate_power , idx , power:\n \
					\t\tidx:\n\
					\t\t0: 1/2M\n \
					\t\t1: 5.5/11M\n\
					\t\t2: 6/6.5M\n\
					\t\t3: 9M\n\
					\t\t4: 12/13M\n\
					\t\t5: 18/19.5M\n\
					\t\t6: 24/26M\n\
					\t\t7: 36/39M\n\
					\t\t8: 48/52M\n\
					\t\t9: 54/58.5M\n\
					\t\t10:65M\n\
					\t\tpower:[-16,16]\
"},			
	{"filter_special",altm_cmd_special_filter,"type frame_control oui1 oui2 oui3 ##\n	type:1,filter_beacon,2,filter_probe,3,filter_clear,4,filter_show \n	frame_control:80,beacon 40,probe req  other_frame_ie\n	oui1~oui3:decimalism valus"},	
	{"country_code",altm_cmd_set_country_code,"country:CN(1~13),JP(1~14),US(1~11)"},
	{"start_tx",altm_cmd_etf_start_tx,"start_tx channel rate len is_40M greedfiled\n\t\t\
	rate:11b:10 20 55 110\n\t\t\
	11g:60 90 120 180 240 360 480 540\n \t\t\
	11n:65 130 195 260 390 520 585 650"},
	{"stop_tx",altm_cmd_etf_stop_tx,"no paramters"},
	{"start_rx",altm_cmd_etf_start_rx,"start_rx channel is_40M"},
	{"stop_rx",altm_cmd_etf_stop_rx,"no parameters"},
	{"start_rx_results",altm_cmd_etf_start_rx_results,"no parameters"},
	{"driver_version",altm_cmd_get_driver_version,"no parameters"},
	{"get_efuse",altm_cmd_get_efuse,"no parameters"},
	{"fix_channel",altm_cmd_set_fix_channel,"sta fix scan channel"},
	{"auto_ppm_cali",altm_cmd_set_ppm_cali,"must connect or sta scan enable! auto_ppm_cali 0/1 ==> off/on"},
	{"get_ppm_vale",altm_cmd_get_ppm_cali_reaults,"get auto_ppm_cali results, auto ppm cali  status on v"},
	{"set_compensation_val",altm_cmd_set_compensation_val,"parameters1 is float type,param2 is write efuse flag"},
	{"getVendor_ie",altm_cmd_get_vendor_ie,"getvendorie"},
	{"sta_channel",altm_cmd_set_sta_listen_channel,"sta_channel 6 "},
	{"test_listen_value",altm_cmd_test_listen_ie,"parameters channel"},
	{"anker_wdt",altm_cmd_anker_wdt,"anker wdt"},
	{"ant_ctl",altm_cmd_ant_control,"ant_ctl start scan"},
	{NULL,NULL,NULL}, //must be last
};

void print_usage (void)
{
	struct altm_cmd *cmd = NULL;

	cmd = altm_commands;
	printf("VERSION====>>>[%s]\n",VERSION);
	printf("Usage: atbm_tool [ifname] [option]\n");
	printf("[option] select : \n");
	while(cmd->cmd){
		printf("%-20s%-s\n", cmd->cmd, cmd->uage);
		cmd++;
	}
	printf("\n");
}

static int handleCmd(struct nl80211_global *nl_connect, int argc, char *argv[])
{
	struct altm_cmd *cmd, *match = NULL;
	int count = 0;
	int ret = 0;

	count = 0;
	cmd = altm_commands;

	while(cmd->cmd){
		if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0){
			match = cmd;
			if (strcasecmp(cmd->cmd, argv[0]) == 0){
				count = 1;
				break;
			}
			count++;
		}
		cmd++;
	}

	if (count > 1){
		printf("Ambiguous command '%s'; possible commands:", argv[0]);
		cmd = altm_commands;
		while(cmd->cmd){
			if (strncasecmp(cmd->cmd, argv[0], strlen(argv[0])) == 0){
				printf(" %s", cmd->cmd);
			}
			cmd++;
		}
		printf("\n");
		ret = 1;
	}else if (count == 0){
		printf("Unknown command '%s'\n", argv[0]);
		ret = 1;
	}else{
		ret = match->handler(nl_connect, argc - 1, &argv[1]);
	}

	return ret;
}

int main(int argc, char *argv[])
{
	int ret = 0;

	if (argc < 3)
	{
		print_usage();
		return 0;
	}
	ret = atbm_ioctrl_init(&g_nl80211_global_s,argv[1]);
	if (ret)
	{
		printf("Init driver failed\n");
		return 0;
	}

	ret = handleCmd(&g_nl80211_global_s, argc - 2, &argv[2]);
  	if (ret)
	{
		printf("Operate failed!!\n");
		ret = -1;
	}
	else
	{
		printf("Operate success!!\n");
		ret = 0;
	}


	atbm_ioctrl_dinit(&g_nl80211_global_s);

	return 0;
}
