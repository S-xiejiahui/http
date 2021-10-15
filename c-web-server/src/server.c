#include "public.h"

static int listenfd = -1;
static int connfd   = -1;

int open_listenfd(char *port)
{
    int optval = 1;
    struct sockaddr_in serveraddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }

    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
    {
        return -1;
    }
    bzero((char *)&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)atoi(port));

    if (bind(listenfd, (SA *)&serveraddr, sizeof(serveraddr)) < 0)
        return -1;

    if (listen(listenfd, LISTENQ) < 0)
        return -1;

    return listenfd;
}

int Open_listenfd(char *port)
{
    int rc;
    if ((rc = open_listenfd(port)) < 0)
        unix_error("open_listenfd error");
    return rc;
}

/****************************************************
 * @brief  检查一些明显的错误，报告给客户端
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    int count = 0;
    char buf[MAXLINE], body[MAXBUF];

    /* 构建HTTP response 响应主体 */
    count += snprintf(body + count, sizeof(body), "<html><title>XJH Error</title>");
    count += snprintf(body + count, sizeof(body), "<body bgcolor=\"white\">\r\n");
    count += snprintf(body + count, sizeof(body), "<center><h1>%s: %s</h1></center>", errnum, shortmsg);
    count += snprintf(body + count, sizeof(body), "<center><h3>%s: %s</h3></center>", longmsg, cause);
    count += snprintf(body + count, sizeof(body), "<hr><center>The XJH Web server</center>\r\n");

    /* 打印HTTP响应报文 */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}