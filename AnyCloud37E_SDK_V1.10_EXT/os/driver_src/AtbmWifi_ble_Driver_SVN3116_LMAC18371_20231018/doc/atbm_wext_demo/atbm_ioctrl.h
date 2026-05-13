#ifndef _ALTBEAM_TOOL_H_
#define _ALTBEAM_TOOL_H_
typedef unsigned long long u64;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;

#ifndef MSG_ERROR
#define MSG_ERROR -1
#endif
#ifndef ENOMEM
#define ENOMEM -1
#endif
#ifndef ENOBUFS
#define ENOBUFS -1
#endif
#ifndef EINVAL
#define EINVAL -1
#endif

struct nl80211_global{
	int ioctl_sock;
	char *ifname;
};

struct altm_wext_msg{
	int type;
	int value;
	unsigned char externData[256];
};
struct atbm_vendor_cfg_ie{
	u8 ie_id;
	u8 ie_len;
	u8 OUI[4];
	u8 ssid_len;
	u8 password_len;
	u8 ssid[32];
	u8 password[64];
};

extern int atbm_ioctrl_dinit(struct nl80211_global *global);
//extern int atbm_ioctrl_init(struct nl80211_global *global);
extern int atbm_ioctrl_init(struct nl80211_global *global,char * ifname);

extern int ioctrl_wext_send_ack(struct nl80211_global *global, void *data, int *out);
extern int ioctrl_wext_send_noack(struct nl80211_global *global, void *data);

#endif //_ALTBEAM_TOOL_H_
