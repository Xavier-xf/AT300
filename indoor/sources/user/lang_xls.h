#ifndef _LANG_XLS_H_
#define _LANG_XLS_H_

#include <stdbool.h>

int lang_xls_init(const char *path);

bool lang_xls_init_state_get(void);

int lang_xls_null_str_num_get(void);

int lang_xls_language_num_get(void);

int lang_xls_str_num_get(void);

const char *lang_xls_str_get(int str_num, int lang_type);

#endif // _LANG_XLS_H_
