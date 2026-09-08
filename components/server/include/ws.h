#ifndef WS_WS_H
#define WS_WS_H

void ws_send(const char*, int);
void ws_set_client_connected_callback(void (*fn)(int));

#endif // WS_WS_H