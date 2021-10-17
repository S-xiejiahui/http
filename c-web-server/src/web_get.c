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
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    if (!strstr(uri, "cgi-xjh"))
    {
        char *p = strchr(uri, '&');
        if (NULL == p)
        {
            strcpy(cgiargs, "");
            strcpy(filename, ".");
            strcat(filename, uri);
        }
        else
        {
            strcpy(cgiargs, p);
            strcpy(filename, ".");
            sscanf(uri, "%[^&]", filename + 1);
        }
        return 1; // static
    }
    else
    {
        char *ptr = strchr(uri, '?');
        if (ptr)
        {
            strcpy(cgiargs, ptr + 1);
            *ptr = '\0';
        }
        else
        {
            strcpy(cgiargs, "");
        }
        strcpy(filename, "");
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
    char body[20480] = {0};
    char *filetype = get_filetype(filename);
    int  filesize = get_filesize(filename);
    
    int count = 0;
    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", filesize);
    count += snprintf(body + count, sizeof(body), "Content-type: %s;charset=utf-8\r\n\r\n", filetype);
    Rio_writen(fd, body, strlen(body));
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

}

void get_detailed_info(int fd, char *argv)
{
    cJSON *root = get_all_file_info();
    cJSON_AddStringToObject(root, "type", "DIR");
    char  *send_msg = cJSON_Print(root);
    // printf("root = %s\n", send_msg);
    send_response_msg(fd, send_msg, strlen(send_msg));
    cJSON_Delete(root);
    return;
}
/****************************************************
 * @brief  public_request 结构体数组(url + 处理函数)
 * @note   根据 url 运行相应处理函数
 * @note   
 ***************************************************/
enum{
    GET_FILE_INFO,
    GET_ALL_FILE_INFO,
    REQUEST_MAX,
};

cgi_public public_request[REQUEST_MAX] = {
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
    struct stat sbuf;
    cJSON *root = cJSON_CreateObject();
    char filename[128] = {0}, cgi_argv[128] = {0};

    int is_static = parse_uri(url, filename, cgi_argv);
    // printf("url = %s, filename = %s, ragv = %s\n", url, filename, cgi_argv);
    //登陆成功，将用户访问页面发送
    if (is_static)
    {
        if (stat(filename, &sbuf) < 0)
        {
            clienterror(fd, filename, "404", "Not found", "XJH couldn't find this file");
            return;
        }
        if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
        {
            clienterror(fd, filename, "403", "Forbidden", "XJH couldn't read the file");
            return;
        }
        serve_static(fd, filename, sbuf.st_size);
    }
    else
    {
        int i = 0;
        for (i = 0;i < REQUEST_MAX; i++)
        {
            if(!strcmp(url, public_request[i].url))
            {
                public_request[i].callback_function(fd, cgi_argv);
            }
        }
        return;
    }
}
