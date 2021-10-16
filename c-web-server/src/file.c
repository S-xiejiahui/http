#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include "cJSON.h"

/****************************************************
 * @brief  由文件名得出文件的类型
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
char *get_filetype(char *filename)
{
    if(NULL == filename)
    {
        fprintf(stderr, "[%s][%d]input is NULL\n", __FILE__, __LINE__);
        return NULL;
    }
    if (strstr(filename, ".html") || strstr(filename, ".php"))
        return "text/html";
    else if (strstr(filename, ".css"))
        return "text/css";
    else if (strstr(filename, ".js"))
        return "text/javascript";
    else if (strstr(filename, ".png"))
        return "image/png";
    else if (strstr(filename, ".jpg"))
        return "image/jpeg";
    else if (strstr(filename, ".svg"))
        return "image/svg+xml";
    else if (strstr(filename, ".gif"))
        return "image/gif";
    else
        return "text/plain";
}
/****************************************************
 * @brief  由文件名得出文件的类型
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
int get_filesize(char *filename)
{
    if(NULL == filename)
    {
        fprintf(stderr, "[%s][%d]%s is NULL\n", __FILE__, __LINE__, filename);
        return -1;
    }
    int fd = open(filename, O_RDWR);
    if(-1 > fd)
    {
        fprintf(stderr, "[%s][%d]open %s failed\n", __FILE__, __LINE__, filename);
    }
    int file_size = lseek(fd, 0, SEEK_END);
    close(fd);
    return file_size;
}
/****************************************************
 * @brief  
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
void file_list(char *pathname, cJSON *root)
{
    struct stat st;
    static int count = 0;

    DIR *fd = opendir(pathname);
    struct dirent * dir = NULL;
    while (dir = readdir(fd))
    {
        if (!strcmp(".", dir->d_name) || !strcmp("..", dir->d_name))
        {
            continue;
        }
        //get file path
        char file_path[64] = {0};
        snprintf(file_path, sizeof(file_path), "%s/", pathname);
        strncat(file_path, dir->d_name, strlen(dir->d_name));
        stat(file_path, &st);
        if( S_ISREG(st.st_mode))
        {
            //Get file size
            char file_size[32] = {0};
            snprintf(file_size, sizeof(file_size), "%ld", st.st_size);

            //Get file last amend time
            struct tm *local_time = NULL;
            local_time = localtime(&st.st_mtime);
            char str_time[64] = {0};
            strftime(str_time, sizeof(str_time), "%Y-%m-%d,%H:%M:%S", local_time);

            //Add File attributes to obj
            cJSON *tmp_reg = cJSON_CreateObject();
            cJSON_AddStringToObject(tmp_reg, "size", file_size);
            cJSON_AddStringToObject(tmp_reg, "type", "REG");
            cJSON_AddStringToObject(tmp_reg, "time", str_time);

            //Add File attributes to root
            cJSON_AddItemToObject(root, dir->d_name, tmp_reg);
        }
        else if (S_ISDIR(st.st_mode))
        {
            //Add folder to root
            cJSON *tmp_dir = cJSON_CreateObject();
            cJSON_AddItemToObject(root, dir->d_name, tmp_dir);

            //Get the data recursively
            file_list(file_path, tmp_dir);
        }
    }
    return;
}
/****************************************************
 * @brief  
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
cJSON *get_all_file_info()
{
    cJSON *root = cJSON_CreateObject();
    file_list(".", root);
    return root;
}
/****************************************************
 * @brief  get file info
 * @note
 * @param  *pathname:
 * @param  *info:
 * @param  size:
 * @retval None
 ***************************************************/
void get_file_info(char *pathname, char *info, int size)
{
    if (NULL == pathname || NULL == info)
    {
        return;
    }
    char *p = strrchr(pathname, '.');
    if (p)
    {
        snprintf(info, 64, "type=%s;size=%d", p, size);
    }
    else
    {
        snprintf(info, 64, "type=%s;size=%d", "dir", size);
    }

    return;
}
/****************************************************
 * @brief  post file info
 * @note
 * @param  fd:
 * @retval None
 ***************************************************/
cJSON *get_allfile_info()
{
    struct stat st;
    char pathname[64] = {0};
    char temp[64] = {0};
    char suffix[64] = {0};

    cJSON *obj = cJSON_CreateObject();
    cJSON *obj_reg = cJSON_CreateObject();
    cJSON *obj_dir = cJSON_CreateObject();

    cJSON_AddItemToObject(obj, "reg", obj_reg);
    cJSON_AddItemToObject(obj, "dir", obj_dir);

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
    char *p = cJSON_Print(obj);
    printf("obj= %s\n", p);
    closedir(dir);
    return obj;
}