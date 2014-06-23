#ifndef __client_h__
#define __client_h__

int client_connect(char *hostname, char *service);
void client_disconnect();
int client_set_speed(float l, float r);

#endif // __client_h__
