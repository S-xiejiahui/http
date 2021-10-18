#ifndef __FILE_H__
#define __FILE_H__
/****************************************************
 * @brief  由文件名得出文件的类型
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
char *get_filetype(char *filename);

/****************************************************
 * @brief  
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
int get_filesize(char *filename);

/****************************************************
 * @brief  
 * @note
 * @param  fd:
 * @param  *index:
 * @retval None
 ***************************************************/
cJSON *get_all_file_info();

/****************************************************
 * @brief  
 * @note
 * @param  filename:
 * @param  *index:
 * @retval None
 ***************************************************/
int check_whether_the_file_exists(char *filename);

#endif