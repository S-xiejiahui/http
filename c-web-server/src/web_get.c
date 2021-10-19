#include "main.h"
#include "public.h"
#include "server.h"
#include "cJSON.h"
#include "file.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/****************************************************
 * @brief  解析URI 为 filename 和 CGI 参数
 * @note
 * @param  fd:
 * @param  *index:
 * @retval 如果是动态内容返回0；静态内容返回 1
 ***************************************************/
int parse_url(char *url, char *filename, char *cgiargs)
{
    if (!strstr(url, "cgi-xjh"))
    {
        char *p = strchr(url, '?');
        if (NULL == p)
        {
            strcpy(cgiargs, "");
            strcpy(filename, ".");
            strcat(filename, url);
        }
        else
        {
            sscanf(p + 1, "%s", cgiargs);
            strcpy(filename, ".");
            sscanf(url, "%[^?]", filename+1);
        }
        //printf("[1]url = [%s]\t filename = [%s]\t argv = [%s]\n", url, filename, cgiargs);
        return 1; // static
    }
    else
    {
        char tmp_url[128] = {0};
        snprintf(tmp_url, sizeof(tmp_url), "%s", url);
        char *p = strchr(url, '?');
        if(p)
        {
            char *u = strrchr(p, '/');
            sscanf(p, "?filename=%[^&]", filename);
            sscanf(p, "?%[^&]", cgiargs);
            memset(url, 0, strlen(url));
            sscanf(tmp_url, "%[^?]", url);
        }
        else
        {
            strcpy(cgiargs, "");
            strcpy(filename, "");
        }
        //printf("[2]url = [%s]\t filename = [%s]\t argv = [%s]\n", url, filename, cgiargs);
        return 0;
    }
}
/****************************************************
 * @brief  发送一个HTTP响应，主体包含一个本地文件的内容
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void serve_static(int fd, char *filename, int filesize)
{
    char body[MAXBUF];
    char *filetype = get_filetype(filename);
    
    int count = 0;
    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", filesize);
    count += snprintf(body + count, sizeof(body), "Content-type: %s;charset=utf-8\r\n\r\n", filetype);
    Rio_writen(fd, body, strlen(body));

    /* send request file */
    int srcfd = Open(filename, O_RDONLY, 0);
    // printf("send file(%s) success\n\n", filename);

    char *srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);

    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);
}
/****************************************************
 * @brief  运行客户端请求的CGI程序
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void serve_dynamic(int fd, char *filename, char *cgiargs)
{
    char buf[MAXLINE];
    char *emptylist[] = {NULL};

    /* 发送响应行 和 响应报头 */
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server XJH Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* 剩下的内容由CGI程序负责发送 */
    if (Fork() == 0)
    { //子进程
        setenv("QUERY_STRING", cgiargs, 1);
        Dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, __environ);
    }
    Wait(NULL);
}
/****************************************************
 * @brief  动态页面请求cgi
 * @note
 * @param  fd:
 * @param  *url:
 * @retval 
 ***************************************************/
void get_file_dynamic(int fd, char *url)
{
    struct stat sbuf;
    char filename[128] = {0}, cgi_argv[128] = {0};
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
    {
        clienterror(fd, filename, "403", "Forbidden", "XJH couldn't run the CGI program");
        return;
    }
    serve_dynamic(fd, filename, cgi_argv);
}
/****************************************************
 * @brief  only send msg of file
 * @note   
 * @note   
 ***************************************************/
void send_response_file(int fd, char *filename)
{
    if(NULL == filename)
    {
        fprintf(stderr, "[%s][%d]input is NULL\n", __FILE__, __LINE__);
        return;
    }
    char body[2048] = {0}, tmp[512+1] = {0}, content[20480] = {0};
    /*char *filetype = get_filetype(filename);*/
    int number = 0, count = 0;
    int filesize = get_filesize(filename);
    if(filesize > 25000)
    {
        clienterror(fd, filename, "403", "Forbidden", "The file is too large to read");
        return;
    }
    
    FILE *fp = fopen(filename, "r");
    number += snprintf(content + number, sizeof(content) - number, "<ul>");
    while(fgets(tmp, 512, fp))
    {
        number += snprintf(content + number, sizeof(content) - number, "<li>%s</li>", tmp);
    }
    number += snprintf(content + number, sizeof(content) - number, "</ul>");

    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", number);
    count += snprintf(body + count, sizeof(body), "Content-type: text/plain;charset=utf-8\r\n\r\n");
    Rio_writen(fd, body, strlen(body));
    Rio_writen(fd, content, strlen(content));
    // int srcfd = Open(filename, O_RDONLY, 0);

    // char *srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    // Close(srcfd);

    // Rio_writen(fd, srcp, filesize);
    // Munmap(srcp, filesize);
}
/****************************************************
 * @brief  only send msg of json type
 * @note   
 * @note   
 ***************************************************/
void send_response_msg(int fd, char *send_msg, int msg_length)
{
    char body[20480] = {0};
    int count = 0;
    count += snprintf(body + count, sizeof(body) - count, "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Content-length: %d\r\n", msg_length);
    count += snprintf(body + count, sizeof(body) - count, "Content-type: application/json;charset=utf-8\r\n\r\n%s", send_msg);
    Rio_writen(fd, body, strlen(body));
}
/****************************************************
 * @brief  
 * @note   
 * @param  fd: 
 * @param  : 
 * @retval None
 ***************************************************/
void get_file_content(int fd, char *argv)
{
    if(NULL == argv)
    {
        return;
    }
    printf("argv = %s\n", argv);
    char key[32] = {0}, value[32] = {0};
    sscanf(argv, "%[^=]=%[^&]", key, value);
    printf("key = %s, value = %s\n", key, value);
    if(!strcmp("filename", key))
    {
        send_response_file(fd, value);
    }
    return;
}

void get_detailed_info(int fd, char *argv)
{
    cJSON *root = get_all_file_info();
    cJSON_AddStringToObject(root, "type", "DIR");
    char  *send_msg = cJSON_Print(root);
    // printf("root = %s\n", send_msg);
    // printf("data size = %ld\n%s\n", strlen(send_msg), send_msg);
    send_response_msg(fd, send_msg, strlen(send_msg));
    cJSON_Delete(root);
    return;
}
/****************************************************
 * @brief  public_request 结构体数组(url + 处理函数)
 * @note   根据 url 运行相应处理函数
 * @note   
 ***************************************************/
 #define GET_REQUEST_MAX  (sizeof(public_get_request)/sizeof(public_get_request[0]))

cgi_public public_get_request[] = {
    {"/cgi-xjh/get_file_content", get_file_content},
    {"/cgi-xjh/get_detailed_info", get_detailed_info},
};
/****************************************************
 * @brief  
 * @note   
 * @param  fd: 
 * @param  *url: 
 * @retval None
 ***************************************************/
void deal_with_get_request(int fd, char *url)
{
    //printf("url = %s\n", url);
    cJSON *root = cJSON_CreateObject();
    char filename[128] = {0}, cgi_argv[128] = {0};

    int is_static = parse_url(url, filename, cgi_argv);
    //printf("url = %s, filename = %s, ragv = %s\n", url, filename, cgi_argv);
    //登陆成功，将用户访问页面发送
    if (is_static)
    {
        if (check_whether_the_file_exists(filename) == 0)
        {
            clienterror(fd, filename, "404", "Not found", "XJH couldn't find this file");
            return;
        }
        else if (check_whether_the_file_exists(filename) == -1)
        {
            clienterror(fd, filename, "403", "Forbidden", "XJH couldn't read the file");
            return;
        }
        serve_static(fd, filename, get_filesize(filename));
    }
    else
    {
        int i = 0;
        for (i = 0;i < GET_REQUEST_MAX; i++)
        {
            if(!strcmp(url, public_get_request[i].url))
            {
                public_get_request[i].callback_function(fd, cgi_argv);
            }
        }
        return;
    }
}
