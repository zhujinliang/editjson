/* ************************************************************************
 *        Name: example.c
 *  Programmer: ZhuJinliang
 * Description:
 *     Version: 1.0
 *     Created: 2011-12-28
 *    Function:
 * ***********************************************************************/
#include "includes.h"

/* main function*/
int main(int argc, char *argv[])
{
    int i = 0;
    char *s = "Array";
    char *data;
    char *filename = "(new file)";
    char *sch;
    char buf[100];
    char *path;
    char tempCh[2];
    cJSON *json = NULL;
    
    GetOption(argc, argv, &sch);
    path = argv[optind];
    if ((data = ReadFile(path)))
    {
        data = ReadFile(path);
        json = Text2Json(data);
        filename = GetFilename(path);
    }
    else
    {
        //新建json文件
        data = ReadFile(sch);
        json = Text2Json(data);
        if (path)
        {
            filename = GetFilename(path);
        }
    }
    EditJson(json, filename);

    return TRUE;
}
