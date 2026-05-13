#include <xls.h>
#include <stdlib.h>
#include <string.h>
#include "lang_xls.h"
#include "db_common.h"

// xls文件的编码格式
#define XLS_CODE "UTF-8"

typedef struct
{
    bool xls_init_state;  // xls文件初始化状态
    int xls_null_str_num; // xls文件 在有效行列中 存在空字符的数量
    int row_total;        // xls文档的有效总行数
    int col_total;        // xls文档的有效总列数
} xls_info_t;

static xls_info_t xls_info = {false, 0, 0, 0};

static char ***lang_xls_buf = NULL;

/*******************************************************************
 * @brief  : 初始化xls文件，加载语言字符串至内存
 * @return  {*} 0:初始化成功 other:初始化失败
 * @param {char} *path xls文件路径
 *******************************************************************/
int lang_xls_init(const char *path)
{
    xlsWorkBook *pWb = NULL;
    xlsWorkSheet *pWs = NULL;

    pWb = xls_open(path, XLS_CODE);

    if (NULL == pWb)
    {
        goto failed;
    }

    pWs = xls_getWorkSheet(pWb, 0); // pWs指向第 0 个 Sheet
    xls_parseWorkSheet(pWs);

    xls_info.row_total = pWs->rows.lastrow + 1;
    xls_info.col_total = pWs->rows.lastcol;

    lang_xls_buf = (char ***)malloc(sizeof(char ***) * xls_info.row_total);
    if (lang_xls_buf == NULL)
    {
        goto failed;
    }

    for (int i = 0; i < xls_info.row_total; i++)
    {
        lang_xls_buf[i] = (char **)malloc(sizeof(char **) * xls_info.col_total);
        if (lang_xls_buf[i] == NULL)
        {
            goto failed;
        }

        for (int j = 0; j < xls_info.col_total; j++)
        {
            if ((&(pWs->rows.row[i]))->cells.cell[j].str == NULL)
            {
                lang_xls_buf[i][j] = NULL;
                xls_info.xls_null_str_num++;
            }
            else
            {
                lang_xls_buf[i][j] = (char *)malloc(strlen((char *)(&(pWs->rows.row[i]))->cells.cell[j].str) + 1);
                if (lang_xls_buf[i][j] == NULL)
                {
                    goto failed;
                }

                strcpy(lang_xls_buf[i][j], (char *)((&(pWs->rows.row[i]))->cells.cell[j].str));
            }
            // printf("%s\t", (char *)((&(pWs->rows.row[i]))->cells.cell[j].str));
        }
        // printf("\n");
    }

    xls_close_WS(pWs);
    xls_close_WB(pWb);

    xls_info.xls_init_state = true;

    db_log_debug("language xls table init successed ! row_total:[%d] col_total:[%d]\n", xls_info.row_total, xls_info.col_total);
    return 0;

failed:

    db_log_error("language xls table init failed !\n");
    return -1;
}
/*******************************************************************
 * @brief  : 获取xls文件初始化状态
 * @return  {bool} false：初始化失败  true：初始化成功
 *******************************************************************/
bool lang_xls_init_state_get(void)
{
    return xls_info.xls_init_state;
}
/*******************************************************************
 * @brief  : 获取xls文件中 在有效行列中 含有的空字符数量
 * @return  {int}
 *******************************************************************/
int lang_xls_null_str_num_get(void)
{
    return xls_info.xls_null_str_num;
}
/*******************************************************************
 * @brief  : 获取xls文件中语言的数量（即列数）
 * @return  {int}
 *******************************************************************/
int lang_xls_language_num_get(void)
{
    return xls_info.col_total;
}
/*******************************************************************
 * @brief  : 获取xls文件中一种语言的字符串数量（即行数）
 * @return  {int}
 *******************************************************************/
int lang_xls_str_num_get(void)
{
    return xls_info.row_total;
}
/*******************************************************************
 * @brief  : 获取xls文件中指定单元格的字符串
 * @return  {const char *} 返回对应的字符串
 * @param {int} str_num 字符串的序号（行号） 从0起
 * @param {int} lang_type 字符串的语言类型（列号） 从0起
 *******************************************************************/
const char *lang_xls_str_get(int str_num, int lang_type)
{
    if (str_num >= xls_info.row_total || lang_type >= xls_info.col_total)
    {
        return NULL;
    }
    return lang_xls_buf[str_num][lang_type];
}
