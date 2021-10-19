#include "main.h"
#include "JSON_checker.h"
#include "public.h"
#include "cJSON.h"
#include "file.h"

#include <dirent.h>
#include <sys/types.h>

void check_account_password(int fd, char *url)
{

}



/****************************************************
 * @brief  get post msg from client
 * @note
 * @param  *message:
 * @param  Socket:
 * @retval
 ***************************************************/
int check_user_password(int Socket_fd, char *message)
{
    char *suffix = NULL;
    char *send_result = NULL;
    char user[64] = {0}, pwd[64] = {0};

    //获取客户端POST请求方式的值
    if ((suffix = strrchr(message, '\n')) != NULL)
    {
        suffix = suffix + 1;
    }

    printf("\n\nPost Value: %s\n\n", suffix);
    //获取页面下发的 账号密码
    if (-1 == json_checker(suffix)) //页面下发的是text格式的数据
    {
        sscanf(suffix, "user=%[^&]&pwd=%[^&]", user, pwd);
    }
    else //页面下发的是json格式的数据
    {
        cJSON *post_msg = cJSON_Parse(suffix);
        cJSON *obj_user = cJSON_GetObjectItem(post_msg, "user");
        cJSON *obj_pwd = cJSON_GetObjectItem(post_msg, "pwd");
        snprintf(user, sizeof(user), "%s", cJSON_Print(obj_user));
        snprintf(pwd, sizeof(user), "%s", cJSON_Print(obj_pwd));
        cJSON_Delete(post_msg);
    }

    printf("user = %s, pwd = %s\n\n", user, pwd);

    if (!strcmp("123", user) && !strcmp("456", pwd))
    {
        send_result = "success";
    }
    else
    {
        send_result = "error";
    }
    //回应 客户端 post 请求 响应
    char body[1024] = {0};
    int count = 0;
    count += snprintf(body + count, sizeof(body) - count, "HTTP/1.0 200 OK\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Content-length: %ld\r\n", strlen(body));
    count += snprintf(body + count, sizeof(body) - count, "Content-type: text/html;\r\n\r\n%s", send_result);
    return 0;
}
/****************************************************
 * @brief  To show picture for client
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void show_picture(int fd, char *index)
{
    int count = 0;
    char body[MAXBUF], send_filename[64];
    /* get file name */
    snprintf(send_filename, 64, "video/%s.jpg", index);
    printf("show pic %s.jpg\n", index);
    /* open file */
    int pic_fd = open(send_filename, O_RDONLY);
    if (-1 == pic_fd)
    {
        printf("open jpg failed\n");
        return;
    }
    /* get file type */
    char *file_type = get_filetype(send_filename);
    /* get file size */
    int file_size = get_filesize(send_filename);
    /* Encapsulated header */
    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", file_size);
    count += snprintf(body + count, sizeof(body), "Content-type: %s\r\n\r\n", file_type);
    /*send header*/
    Rio_writen(fd, body, strlen(body));
    /* send file */
    char *srcp = Mmap(0, file_size, PROT_READ, MAP_PRIVATE, pic_fd, 0);
    Close(pic_fd);
    Rio_writen(fd, srcp, file_size);
    Munmap(srcp, file_size);
}
/****************************************************
 * @brief
 * @note
 * @return
 ***************************************************/
void log_in_check(int fd, char *argv)
{
    check_user_password(fd, "pwd=123&pwd=456");
}

void play_surveillance_screen(int fd, char *argv)
{

}
/****************************************************
 * @brief  public_request 结构体数组(url + 处理函数)
 * @note   根据 url 运行相应处理函数
 * @note   
 ***************************************************/
 #define POST_REQUEST_MAX  (sizeof(public_post_request)/sizeof(public_post_request[0]))

cgi_public public_post_request[] = {
    {"/cgi-xjh/log_in_check", log_in_check},
    {"/cgi-xjh/play_surveillance_screen", play_surveillance_screen},
};
/****************************************************
 * @brief  
 * @note   
 * @param  fd: 
 * @param  *url: 
 * @retval None
 ***************************************************/
void deal_with_post_request(int fd, char *url)
{
    char argvs[128] = "", *p = NULL;
    if(p = strchr(url, '?'))
    {
        sscanf(p, "?%[^&]", argvs);
    }
  
    int i = 0;
    for (i = 0; i < POST_REQUEST_MAX; i++)
    {
        if(!strcmp(url, public_post_request[i].url))
        {
            public_post_request[i].callback_function(fd, argvs);
        }
    }
}