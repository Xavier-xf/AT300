#include <string.h>
#include <stdio.h>
#include "language.h"
#include "db_common.h"
#include "lang_xls.h"
// #include "user_data.h"

static const char *language_string_default[/* LANGUAGE_STRING_ID_TOTAL */] =
    {
        // layout home
        "NULL",
        "DOOR CAMERA",
        "DISCONNECTED",
        "CAMERA CONFLICT",
        "INTERCOM",
        "DUPLICATE ID",
        "NEW CAPTURE",
        "DO NOT DISTURB",
        "ABSENT MODE",
        "MON",
        "TUE",
        "WED",
        "THU",
        "FRI",
        "SAT",
        "SUN",
        "DEVICE 1",
        "DEVICE 2",
        "DEVICE 3",
        "DEVICE 4",
        "DEVICE ID CONFLICT",
        "AM",
        "PM",

        // layout monitor
        "Camera1",
        "Camera2",
        "Alarm",
        "Door Opened",
        "Camera Busy",
        "Camera signal has been lost !",
        "Duplicate camera ID !",
        "App is monitoring…",
        "The lobby is calling…",

        // layout intercom
        "Internal Call",
        "Device Busy",

        // layout media
        "Camera1",
        "Camera2",
        "Delete All Photo?",
        "Delete This Photo?",

        // layout security
        "Start",
        "Stop",
        "Active Now",
        "After",
        "seconds, the absent mode starts to run.",
        "Delete All?",

        // layout setting
        "General",
        "Sound",
        "Network",
        "Mode",
        "Other",
        "RESET",

        // general
        "Device ID",
        "Phone",
        "APP",
        "Time",
        "Date",
        "Network Time",
        "Auto Set",
        "Language",
        "English",
        "한글",
        "Español",
        "عربى",
        "Tiếng Việt",
        "русский",
        "Disconnecting from the APP requires a reboot, now reboot.",
        "NOTICE",
        "Please check the network connection,",
        "and make sure the device is bound to the phone.",
        "Only support one device and APP link",
        "There are duplicate devices or connections.",
        "Do you want to restart and update the device ID?",

        // sound
        "Camera1",
        "Camera2",
        "Intercom",
        "Calling Volume",
        "Talk Volume",
        "Calling Melody",
        "Speaker Sensitivity",
        "MIC Sensitivity",

        // wifi
        "WI-FI",
        "Add Network",
        "Connected",
        "Connecting",
        "Next >>",
        "<< Previous",
        "Connection Succeeded!",
        "Authentication Failed!",
        "Not Found!",
        "Input is Empty!",

        "SSID",
        "Password",
        "Enter SSID",
        "Enter Password",
        "SSID Is Empty!",
        "Wrong Password",
        "Password Is Too Short!",

        // mode
        "Auto Image Capture",
        "Always On Display",
        "Absent Setting Time",
        "Monitoring Time",
        "Door Open Time",

        // other
        "Screen Brightness",
        "Modify Password",
        "System Information",
        "Manual Download",
        // "Storage Info",
        "Check Updates",
        "Multigenerational Mode",

        // modify pwd
        "Old",
        "Old Password",
        "New",
        "New Password",
        "Modified successfully!",
        "Change Your Password Now?",

        // system info
        "System Version",
        "Camera1 Version",
        "Camera2 Version",
        "IP Address",
        "Serial Number",
        "TUYA UUID",

        "Check For Upgrades",
        "Upgrade File Transfer",
        "File Transfer Failed",
        "Damaged File",
        "File Transfer Completed",

        // apartment
        "Please input password",
        "Admin Password",
        "Wrong Password",
        "Administrator registration mode activated.\nAfter successful registration,\nKOCOM HOME will be disabled.",
        "Aprtment Server Setting",
        "Auto Regist",
        "Manual Regist",
        "Aprtment Server Setting",
        "Registration successful, restart now?",
        "Registration successful!",
        "Registration failed!",
        "Apartment Name",
        "Search Apartment",
        "Server IP",
        "API Port",
        "SIP Port",
        "Account",
        "Password",
        "Proxy",
        "Unit No.",
        "Room No.",
        "Input Unit No.",
        "Input Room No.",

        // system reset
        "Do you want to reset?",
        "Resetting, please wait.",

        // tuya
        "Door1 current",
        "Door1",
        "Door2 current",
        "Door2",

        // ota
        "Update",
        "Do not turn off power",
        "Not SD card",
        "File carete failed",
        "File check failed",
        "File download finish",

        // common
        "Lobby",
        "Device 1",
        "Device 2",
        "Device 3",
        "Device 4",
        "Sensor 1",
        "Sensor 2",
        "Approval",
        "Back",
        "OFF",
        "ON",
        "Yes",
        "No",
        "Input is Empty!",
        "Authentication Failed!",
        "Password",
        "Comfirm",
        "Quit",
        "WARNING",
};

COMPILE_TIME_ASSERT(sizeof(language_string_default) / sizeof(language_string_default[0]) == LANGUAGE_STRING_ID_TOTAL);

const char *language_string_get(layout_lang_id id)
{
    if (lang_xls_init_state_get())
    {
        return lang_xls_str_get(id, /* user_data_get()->general.language */0);
    }
    else
    {
        return language_string_default[id];
    }
}

/*******************************************************************
 * @brief  : 将str中的数字转为BaiJamjuree字体，输出到buf（buf要是str的两倍以上）
 * @return  {*}
 * @param {unsigned char} *str_buf
 *******************************************************************/
void language_num_to_BaiJamjuree(const char *str, char *buf)
{
    static const char *number[] = {"\u2460", "\u2461", "\u2462", "\u2463", "\u2464", "\u2465", "\u2466", "\u2467", "\u2468", "\u2469"};
    int str_len = strlen(str);
    for (int i = 0; i < str_len; i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
        {
            strcat(buf, number[str[i] - '0']);
        }
        else
        {
            strncat(buf, &str[i], 1);
        }
    }
}