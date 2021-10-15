#include "main.h"
#include "JSON_checker.h"
#include "public.h"
#include "cJSON.h"

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
const char *Post_Value(char *message, int Socket)
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
    return suffix;
}
/****************************************************
 * @brief  由文件名得出文件的类型
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html") || strstr(filename, ".php"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".css"))
        strcpy(filetype, "text/css");
    else if (strstr(filename, ".js"))
        strcpy(filetype, "text/javascript");
    else if (strstr(filename, ".png"))
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpeg");
    else if (strstr(filename, ".svg"))
        strcpy(filetype, "image/svg+xml");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else
        strcpy(filetype, "text/plain");
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
    char body[MAXBUF], file_type[64], send_filename[64];
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
    get_filetype(send_filename, file_type);
    /* get file size */
    int pic_size = lseek(pic_fd, 0, SEEK_END);
    lseek(pic_fd, 0, SEEK_SET);
    /* Encapsulated header */
    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", pic_size);
    count += snprintf(body + count, sizeof(body), "Content-type: %s\r\n\r\n", file_type);
    /*send header*/
    Rio_writen(fd, body, strlen(body));
    /* send file */
    char *srcp = Mmap(0, pic_size, PROT_READ, MAP_PRIVATE, pic_fd, 0);
    Close(pic_fd);
    Rio_writen(fd, srcp, pic_size);
    Munmap(srcp, pic_size);
}


void deal_with_post_request(int fd, char *url)
{

}