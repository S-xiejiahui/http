#ifndef __JSON_CHECKER_H__
#define __JSON_CHECKER_H__
/* JSON_checker.h */

typedef struct JSON_checker_struct
{
    int valid;
    int state;
    int depth;
    int top;
    int *stack;
} * JSON_checker;

extern int json_checker(char *json_str); //use

extern JSON_checker new_JSON_checker(int depth);
extern int JSON_checker_char(JSON_checker jc, int next_char);
extern int JSON_checker_done(JSON_checker jc);

#endif