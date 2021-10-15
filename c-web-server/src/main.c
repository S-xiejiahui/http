#include "main.h"
#include "JSON_checker.h"
#include "cJSON.h"

#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void Logo(char *port);
void doit(int fd);
void show_picture(int fd, char *index);
void serve_static(int fd, char *filename, int filesize);
int parse_uri(char *uri, char *filename, char *cgiargs);
void read_requesthdrs(rio_t *rp);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);

#define ERROR 0
#define PORT "8080"
/****************************************************
 * @brief  main function
 * @note
 * @param  argc:
 * @param  **argv:
 * @retval
 ***************************************************/
int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    char *port = NULL;
    struct sockaddr_in clientaddr;
    if (argc == 1)
    {
        port = PORT;
    }
    else if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    else
    {
        port = argv[1];
    }
    listenfd = Open_listenfd(port);
    Logo(port);
    printf("The web server has been started.....\n");

    /*信号处理函数,用来处理僵尸进程*/
    signal_r(SIGCHLD, sigchild_handler);

    while (1)
    {
        clientlen = sizeof(clientaddr);
        if ((connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen)) < 0)
        {
            if (errno == EINTR)
                continue;
            else
                printf("Accept error...");
        }

        pid_t pid = Fork();
        if (pid == 0)
        {
            doit(connfd);
            Close(connfd);
            exit(0);
        }
        else
        {
            Close(connfd);
        }
    }
}
/****************************************************
 * @brief  Check whether it is GET or POST
 * @note
 * @param  *method:
 * @param  Socket:
 * @retval
 ***************************************************/
const char *Judge_Method(char *method, int Socket)
{
    //判断请求方式
    if (strcmp(method, "GET") == 0)
    {
        return "GET";
    }
    else if (strcmp(method, "POST") == 0)
    {
        return "POST";
    }
    else
    {
        clienterror(Socket, method, "501", "Not Implemented", "XJH does not implement this method");
        return ERROR;
    }
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
    count += snprintf(body + count, sizeof(body) - count, "Content-type: text/html;charset=utf-8\r\n\r\n%s", send_result);
    return suffix;
}
/**
 * @brief  get file info
 * @note
 * @param  *pathname:
 * @param  *info:
 * @param  size:
 * @retval None
 */
void get_file_info(char *pathname, char *info, int size)
{
    if (NULL == pathname || NULL == info)
    {
        return;
    }
    char *p = strrchr(pathname, '.');
    if (p)
    {
        printf("p = %s\n", p);
        snprintf(info, 64, "type=%s;size=%d", p, size);
    }
    else
    {
        snprintf(info, 64, "type=%s;size=%d", "dir", size);
    }

    return;
}
/**
 * @brief  post file info
 * @note
 * @param  fd:
 * @retval None
 */
void send_allfile_info(int fd)
{
    struct stat st;
    char pathname[64] = {0};
    char temp[64] = {0};
    char suffix[64] = {0};

    cJSON *root = cJSON_CreateObject();
    cJSON *obj_reg = cJSON_CreateObject();
    cJSON *obj_dir = cJSON_CreateObject();

    cJSON_AddItemToObject(root, "reg", obj_reg);
    cJSON_AddItemToObject(root, "dir", obj_dir);

    DIR *dir = opendir("./");
    struct dirent *read_dir = NULL;
    while (read_dir = readdir(dir))
    {
        if (!strcmp(read_dir->d_name, ".") || !strcmp(read_dir->d_name, ".."))
        {
            continue;
        }
        stat(read_dir->d_name, &st);
        get_file_info(read_dir->d_name, suffix, st.st_size);
        if (S_ISDIR(st.st_mode))
        {
            cJSON_AddStringToObject(obj_dir, read_dir->d_name, suffix);
        }
        else if (S_ISREG(st.st_mode))
        {
            cJSON_AddStringToObject(obj_reg, read_dir->d_name, suffix);
        }
    }
    char *p = cJSON_Print(root);
    printf("root= %s\n", p);
    cJSON_Delete(root);
    closedir(dir);

    char body[1024] = {0};
    int count = 0;
    count += snprintf(body + count, sizeof(body) - count, "HTTP/1.0 200 OK\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body) - count, "Content-length: %ld\r\n", strlen(p));
    count += snprintf(body + count, sizeof(body) - count, "Content-type: text/html;charset=utf-8\r\n\r\n%s", p);
    Rio_writen(fd, body, strlen(body));
    return;
}
/****************************************************
 * @brief  Handle client connections
 * @note   GET or POST
 * @param  fd:
 * @retval None
 ***************************************************/
void doit(int fd)
{
    int is_static;
    struct stat sbuf;
    rio_t rio;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE]; //设置根目录
    char cgiargs[MAXLINE];

    //初始化 rio 结构
    Rio_readinitb(&rio, fd);
    //读取http请求行
    Rio_readlineb(&rio, buf, MAXLINE);
    //格式化存入 把该行拆分
    sscanf(buf, "%s %s %s", method, uri, version);
    //读取并忽略请求报头
    read_requesthdrs(&rio);

    //只能处理GET/POST请求，如果不是GET/POST请求的话返回错误
    if (Judge_Method(method, fd) == ERROR)
    {
        return;
    }
    else if (Judge_Method(method, fd) == "POST")
    {
        Post_Value(rio.rio_buf, fd);
    }
    else if (Judge_Method(method, fd) == "GET")
    {
        is_static = parse_uri(uri, filename, cgiargs);
        printf("[2]uri = %s, filename = %s, cgiargs = %s\n", uri, filename, cgiargs);
        if (!strcmp("/app.html/allfile", uri))
        {
            send_allfile_info(fd);
            return;
        }
        //文件不存在
        if (stat(filename, &sbuf) < 0)
        {
            clienterror(fd, filename, "404", "Not found", "XJH couldn't find this file");
            return;
        }

        if (is_static)
        {
            if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode))
            {
                clienterror(fd, filename, "403", "Forbidden", "XJH couldn't read the file");
                return;
            }
            char *tmp = strchr(cgiargs, '&');
            if (NULL == tmp)
            {
                serve_static(fd, filename, sbuf.st_size);
            }
            else
            {
                char index[64] = {0};
                sscanf(cgiargs, "&%[^&]%*s", index);
                show_picture(fd, index);
            }
        }
        else
        {
            if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode))
            {
                clienterror(fd, filename, "403", "Forbidden", "XJH couldn't run the CGI program");
                return;
            }
            serve_dynamic(fd, filename, cgiargs);
        }
    }
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
/****************************************************
 * @brief  读取http 请求报头，
 * @note   无法使用请求报头的任何信息，读取之后忽略掉
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void read_requesthdrs(rio_t *rp)
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    while (strcmp(buf, "\r\n"))
    {
        Rio_readlineb(rp, buf, MAXLINE);
    }
    return;
}
/****************************************************
 * @brief  解析URI 为 filename 和 CGI 参数
 * @note
 * @param  fd:
 * @param  *index:
 * @retval 如果是动态内容返回0；静态内容返回 1
 ***************************************************/
int parse_uri(char *uri, char *filename, char *cgiargs)
{
    if (!strstr(uri, "cgi-bin"))
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
        strcpy(filename, ".");
        strcat(filename, uri);
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
    char body[MAXBUF], filetype[64];
    get_filetype(filename, filetype);
    
    int count = 0;
    count += snprintf(body + count, sizeof(body), "HTTP/1.1 200 OK\r\n");
    count += snprintf(body + count, sizeof(body), "Server: XJH Web Server\r\n");
    count += snprintf(body + count, sizeof(body), "Connection:close\r\n");
    count += snprintf(body + count, sizeof(body), "Content-length: %d\r\n", filesize);
    count += snprintf(body + count, sizeof(body), "Content-type: %s;charset=utf-8\r\n\r\n", filetype);
    Rio_writen(fd, body, strlen(body));

    /* send request file */
    int srcfd = Open(filename, O_RDONLY, 0);
    printf("send file(%s) success\n\n", filename);

    char *srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Close(srcfd);

    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);
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
    /*
 if(strstr(filename, ".php")) {
           sprintf(response, "HTTP/1.1 200 OK\r\n");
           sprintf(response, "%sServer: Pengge Web Server\r\n",response);
           sprintf(response, "%sConnection: close\r\n",response);
           sprintf(response, "%sContent-type: %s\r\n\r\n",response,filetype);
           Write(connfd, response, strlen(response));
           printf("Response headers:\n");
           printf("%s\n",response);
           php_cgi(filename, connfd,cgi_params);
           Close(connfd);
           exit(0);
       //静态页面输出
 }
 */
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
    sprintf(buf, "HTTP/1.0 %s %s", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
/****************************************************
 * @brief  print log
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void Logo(char *port)
{
    printf("-----------------------------------------------------------\n");
    printf("                       PORT = %s                     \n", port);
    printf("-----------------------------------------------------------\n");
    printf("  __          ________ _______\n");
    printf("  \\ \\        / /  ____|  _____\\\n");
    printf("   \\ \\  /\\  / /| |____  |____) )\n");
    printf("    \\ \\/  \\/ / |  ____|  ____(   __  __     __ ___\n");
    printf("     \\  /\\  /  | |____  |____) )(__ |_ \\  /|_ |___)\n");
    printf("      \\/  \\/   |______|_______/  __)|__ \\/ |__|   \\\n");
    printf("\n");
    printf("            Welcome to use the Web Server!\n");
    printf("                     Version 1.0\n\n");
    printf("                         XJH\n");
    printf("-----------------------------------------------------------\n\n");

    return;
}