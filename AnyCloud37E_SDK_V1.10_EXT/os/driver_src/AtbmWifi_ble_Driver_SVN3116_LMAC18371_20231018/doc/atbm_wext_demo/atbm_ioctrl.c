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
//#include "nl80211_copy.h"
#include "atbm_ioctrl.h"

#define WIRELESSDIR "/proc/net/wireless"
#define IFNAME	"wlan"
#define ATBM_TOOL_IO_CTRL_CMD (SIOCDEVPRIVATE + 2)
#define WIRELESS_LEN 500
#ifndef IFNAMSIZ
#define IFNAMSIZ 16
#endif
int atbm_ioctrl_dinit(struct nl80211_global *global)
{
	if(global->ioctl_sock)
		close(global->ioctl_sock);
	return 0;
}
int atbm_ioctrl_init(struct nl80211_global *global,char *ifname)
{
	int wireless_fd = 0;
	char *wireless_buff = NULL;
	int ret = 0;
	int real_len = 0;
	char *position = NULL;
	int index_pos = 0;

	if(ifname == NULL){
		printf("please input wifi ifname!\n");
		return -1;
	}
	//memcpy(global->ifname,ifname,strlen(ifname));
	global->ifname = ifname;
	printf("ifname [%s] \n",global->ifname,strlen(global->ifname));
	global->ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);

	if(global->ioctl_sock < 0)
	{
		printf("globle_atbm_ioctrl.ioctl_sock err \n");
		ret = -1;
		goto exit;
	}
	
	printf("ifname(%s),ioctl_sock(%d)\n",global->ifname,global->ioctl_sock);
	
	ret = 0;
exit:
	if(wireless_buff)
		free(wireless_buff);
	if(wireless_fd > 0)
		close(wireless_fd);
	return ret;

}
struct msg_resp{
		u32 len;
		u32 respbuff[31];
};

int ioctrl_wext_send_ack(struct nl80211_global *global, void *data, int *out)
{
	struct ifreq ifr;
	int ret = 0;
	struct altm_wext_msg *pMsg;

	if((data == NULL)||(out == NULL) || (global == NULL))
	{
		ret = -1;
		goto exit;
	}
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, global->ifname, IFNAMSIZ);
	ifr.ifr_data = data;
	if ((ret = ioctl(global->ioctl_sock, SIOCDEVPRIVATE, &ifr)) < 0) {
		printf("ioctrl_send_ack:ioctrl err,ret = %d \n",ret);
		ret = -1;
		goto exit;
	}

	if(out != NULL)
	{
		pMsg = (struct altm_wext_msg*)data;
		printf("ioctrl_wext_send_ack: type = %d, value = %d ,%x:%x\n", pMsg->type,pMsg->value,out,(u8 *)out);
		memcpy((u8 *)out, (u8 *)&pMsg->value, sizeof(int));
	}

	ret = 0;
exit:

	return ret;
}

int ioctrl_wext_send_noack(struct nl80211_global *global, void *data)
{
	struct ifreq ifr;
	int ret = 0;
	if((data == NULL)|| (global == NULL))
	{
		ret = -1;

		goto exit;
	}
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, global->ifname, IFNAMSIZ);
	ifr.ifr_data = data;
	if ((ret = ioctl(global->ioctl_sock, SIOCDEVPRIVATE, &ifr)) < 0) {
		goto exit;
	}

	ret = 0;
exit:

	return ret;
}

