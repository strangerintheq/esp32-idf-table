#ifndef NETWORK_SETTINGS_H
#define NETWORK_SETTINGS_H

typedef struct {
    char mode[8];
    char wifi_ssid[32];
    char wifi_password[64];
    char ap_ssid[32];
    char ap_password[64];
} network_settings_t;

void network_settings_read(network_settings_t*);

char* network_settings_get_json_str(void);

void network_settings_update_from_json_str(char*);

#endif