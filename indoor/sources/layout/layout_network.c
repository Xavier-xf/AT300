// /*
//  * @Descripttion: 
//  * @version: 
//  * @Author: wxj
//  * @Date: 2023-02-26 20:31:49
//  * @LastEditors: wxj
//  * @LastEditTime: 2023-05-02 18:27:29
//  */
// #include "layout_common.h"

// typedef enum
// {
//     NETWORK_BTN_BACK_ID,
//     NETWORK_BTN_WIFI_SW_ID,
//     NETWORK_BTN_WIFI_SCAN_ID,
//     NETWORK_BTN_TOTAL,
// } network_btn_t;

// static location_t location[NETWORK_BTN_TOTAL] = {

//     {20, 10, 80, 50},
//     {200, 10, 80, 50},
//     {100, 100, 80, 80},
//     // {250, 100, 80, 80}
// };


// static lv_obj_t *network_back_btn = NULL;
// static lv_obj_t *network_wifi_sw = NULL;
// static lv_obj_t *network_wifi_list = NULL;

// static bool wlan_open_flag = false;

// static void music_network_back_btn_click(lv_obj_t *obj)
// {
//     goto_layout(pLAYOUT(home));
// }
// static void music_network_back_btn_create(void)
// {
//     network_back_btn = lv_btn_create(lv_scr_act());
//     lv_obj_set_pos(network_back_btn, location[NETWORK_BTN_BACK_ID].x, location[NETWORK_BTN_BACK_ID].y);
//     lv_obj_set_size(network_back_btn, location[NETWORK_BTN_BACK_ID].w, location[NETWORK_BTN_BACK_ID].h);
//     static user_btn_data_t user_btn_data = user_btn_data_set_click(music_network_back_btn_click);
//     layout_btn_event_add(network_back_btn, &user_btn_data);

//     lv_obj_t *label = lv_label_create(network_back_btn);
//     lv_label_set_text(label, "BACK");
//     lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0);
//     lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
// }


// static void music_network_wifi_sw_everything(lv_event_t *event)
// {
//     lv_obj_t *sw = lv_event_get_target(event);
//     lv_event_code_t code = lv_event_get_code(event);
//     if (code == LV_EVENT_VALUE_CHANGED)
//     {
        
//         if(wlan_wifi_run_state_get() == WLAN_NO_DEVICE)
//         {
//             lv_obj_add_state(network_wifi_sw, LV_STATE_DEFAULT);
//             lv_obj_clear_flag(network_wifi_sw, LV_OBJ_FLAG_CLICKABLE);
//             return;
//         }
//         wlan_open_flag = lv_obj_has_state(sw, LV_STATE_CHECKED);
//         if(wlan_open_flag)
//         {
//             wlan_wifi_device_open();
//         }
//         else
//         {
//             wlan_wifi_device_close();
//         }
//     }
// }
// static void music_network_wifi_sw_create(void)
// {
//     network_wifi_sw = lv_switch_create(lv_scr_act());
//     lv_obj_set_pos(network_wifi_sw, location[NETWORK_BTN_WIFI_SW_ID].x, location[NETWORK_BTN_WIFI_SW_ID].y);
//     lv_obj_set_size(network_wifi_sw, location[NETWORK_BTN_WIFI_SW_ID].w, location[NETWORK_BTN_WIFI_SW_ID].h);
//     static user_btn_data_t user_btn_data = user_btn_data_set_everything(music_network_wifi_sw_everything);
//     layout_btn_event_add(network_wifi_sw, &user_btn_data);
//     if(wlan_wifi_run_state_get() == WLAN_WIFI_STATE_OPEN)
//     {
//         lv_obj_add_state(network_wifi_sw, LV_STATE_CHECKED);
//     }
//     else
//     {
//         lv_obj_add_state(network_wifi_sw, LV_STATE_DEFAULT);
//         if(wlan_wifi_run_state_get() == WLAN_NO_DEVICE)
//         {
//             lv_obj_clear_flag(network_wifi_sw, LV_OBJ_FLAG_CLICKABLE);
//         }
//     }
// }

// static void network_bg_display(void)
// {
//     lv_obj_set_style_bg_img_src(lv_scr_act(), UI_RES_PATH "ui_network_bg1_icon.bin", 0);
//     lv_obj_set_style_bg_img_opa(lv_scr_act(), LV_OPA_COVER, 0);
// }

// // static void network_wifi_scan_btn_click(lv_obj_t *obj)
// // {
// //     // if(wlan_wifi_run_state_get() != WLAN_WIFI_STATE_OPEN)
// //     // {
// //     //     lv_obj_clean(network_wifi_list);
// //     // }
// //     // unsigned char name[128] = {0};
// //     // unsigned char ip[32] = {0};
// //     // wlan_wifi_connection_stauts(name, NULL, ip, NULL, NULL);
// //     // printf("==============>>> name : [%s] ip : [%s]\n", name, ip);

// //     // int pid = 0;
// //     // wlan_wpa_supplicant_pid_exist_check(&pid);
// //     // printf("=================>>> wpa_supplicant进程的pid:[%d]\n", pid);
// // }
// // static void network_wifi_scan_btn_create(void)
// // {
// //     static user_btn_data_t user_btn_data = user_btn_data_set_click(network_wifi_scan_btn_click);
// //     common_btn_icon_create(lv_scr_act(), location[NETWORK_BTN_WIFI_SCAN_ID], LV_SYMBOL_REFRESH, "Scan", &user_btn_data);
// // }


// static void network_wifi_list_btn_create(void)
// {
//     network_wifi_list = lv_list_create(lv_scr_act());
//     lv_obj_set_size(network_wifi_list, 400, 480);
//     // lv_obj_center(network_wifi_list);
//     lv_obj_align(network_wifi_list, LV_ALIGN_RIGHT_MID, 0, 0);
// }

// static void network_wifi_scan_timer(lv_timer_t *timer)
// {
//     if(wlan_wifi_run_state_get() != WLAN_WIFI_STATE_OPEN)
//     {
//         lv_obj_clean(network_wifi_list);
//         return;
//     }
//     if(wlan_wifi_info_scan())
//     {
//         lv_obj_clean(network_wifi_list);
//         wifi_info_t *wifi_info = wlan_wifi_info_get();
//         for (int i = 0; i < wlan_wifi_total_get(); i++)
//         {
//             // printf("wifi名称:[%s]\t", wifi_info[i].ssid);
//             // printf("wifi信号:[%d]\t", wifi_info[i].signal_level);
//             // printf("%s\n", wifi_info[i].free ? "免费wifi" : "加密wifi");
//             lv_list_add_btn(network_wifi_list, LV_SYMBOL_WIFI, wifi_info[i].ssid);
//         }
//     }
// }

// LAYOUT_ENETER_FUNC(network)
// {
//     network_bg_display();
//     music_network_back_btn_create();
//     music_network_wifi_sw_create();
//     // network_wifi_scan_btn_create();
//     network_wifi_list_btn_create();

//     lv_timer_t *timer = lv_layout_timer_create(network_wifi_scan_timer, 2000, NULL);
//     lv_timer_ready(timer);
// }

// LAYOUT_QUIT_FUNC(network)
// {
// }

// CREATE_LAYOUT(network)