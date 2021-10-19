#ifndef __SERVER_H__
#define __SERVER_H__

int Open_listenfd(char *port);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);
void get_local_ip_addr(char *local_ipaddr, int length);

#endif