/* ************************************************************************
 *        Name: test.c
 *  Programmer: ZhuJinliang
 * Description:
 *     Version: 1.0
 *     Created: 2011-11-16
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
    char buf[100];
    char *path;
    char schema[] = "auto generate schema";
    char *sch = "./test.json";
    char *schName;
    char tempCh[2];
    cJSON *json = NULL;
    ListNode *jsonPosHead = NULL;
    struct tb_event ev;
    struct tb_cell cell;
    
    if (!(jsonPosHead = InitList()))
    {
        Printf(40,1,TB_RED,TB_BLACK, "%s","List init err!");
    }
    GetOption(argc, argv, &sch);
    path = argv[optind];
    if ((data = ReadFile(path)))
    {
        data = ReadFile(path);
        json = Text2Json(data);
        filename = GetFilename(path);
        schName = schema;
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
        schName = GetFilename(sch);
    }
    InitWindow();
    DrawWindow(filename, schName);
    ParseObject(json, jsonPosHead,  2, 4);
    Display(jsonPosHead);
    tb_present();
    while (tb_poll_event(&ev))
    {
        switch(ev.type)
        {
            case TB_EVENT_KEY: //键盘输入
                if (ev.key == TB_KEY_CTRL_X)
                {
                    //提示保存
                    gl_keyFlag = TB_KEY_CTRL_X;
                }
                if (ev.key == TB_KEY_CTRL_W)
                {
                    //Write/Save
                    gl_keyFlag = TB_KEY_CTRL_W;
                    gl_y = tb_height() - 4;
                    gl_record[gl_y].str = (char*)malloc((strlen(filename)+1));
                    strcpy(gl_record[gl_y].str, filename);
                }
                if (ev.key == TB_KEY_CTRL_N)
                {
                    ;//insert new item
                }
                if (ev.key == TB_KEY_CTRL_D)
                {
                    ;//delete item
                }
                tb_clear();
                if (gl_keyFlag == TB_KEY_CTRL_X)
                {
                    utf8_unicode_to_char(tempCh, ev.ch);
                    if (gl_modifyFlag == 1)
                    {
                        if (ev.key == TB_KEY_ESC)
                        {
                            gl_keyFlag = NONE;
                        }
                        else if ((tempCh[0] == 'y') || (tempCh[0] == 'Y'))
                        {
                            //Save to disk
                            gl_keyFlag = NONE;
                            if (!SaveToDisk(filename, json))
                            {
                                //err
                                Printf(65, gl_y, TB_RED, TB_BLACK, "%s", "save err!");
                            }
                            else
                            {
                                Printf((tb_width()/2 - 5), (tb_height() - 4), TB_GREEN, TB_BLACK, "%s saved", filename );
                                tb_shutdown();
                                return TRUE;
                            }
                        }
                        else if ((tempCh[0] == 'n') || (tempCh[0] == 'N'))
                        {
                            tb_shutdown();
                            return TRUE;
                        }
                        
                    }
                    else
                    {
                        tb_shutdown();
                        return TRUE;
                    }
                }
                else if (gl_keyFlag == TB_KEY_CTRL_W)
                {
                    gl_y = tb_height() - 4;
                    gl_bound[gl_y].left = 24;
                    gl_bound[gl_y].right = gl_bound[gl_y].left + strlen(gl_record[gl_y].str);
                    gl_record[gl_y].x = 24;
                    if (ev.key == TB_KEY_ESC)
                    {
                        //strcpy(gl_record[gl_y].str, "");
                        gl_keyFlag = NONE;
                    }
                    else if (ev.key == TB_KEY_ENTER)
                    {
                        //Save to disk
                        gl_keyFlag = NONE;
                        strcpy(buf, gl_record[gl_y].str);
                        filename = buf;
                        if (!SaveToDisk(filename, json))
                        {
                            //err
                            Printf(65, gl_y, TB_RED, TB_BLACK, "%s", "save err!");
                        }
                        else
                        {
                            Printf((tb_width()/2 - 5), (tb_height() - 4), TB_GREEN, TB_BLACK, "%s saved", filename );
                            //strcpy(gl_record[gl_y].str, "");
                        }
                    }
                }
                j = 1;
                s_str = s;
                s_cnt = 0;
                DrawWindow(filename, schName);
                //ParseObject(json, jsonPosHead,  2, 4);
                InputKey(&ev, jsonPosHead);
                SaveToJson(jsonPosHead);//把输入的内容复制给json结构体
                Display(jsonPosHead);
                CheckBound(&ev);
                tb_set_cursor(gl_x, gl_y);
                tb_present();
                break;

            case TB_EVENT_RESIZE: //调整窗口大小
                tb_clear();
                DrawWindow(filename, schName);
                j = 1;
                s_str = s;
                s_cnt = 0;
                //ParseObject(json, jsonPosHead,  2, 4);
                InputKey(&ev, jsonPosHead);
                SaveToJson(jsonPosHead);
                Display(jsonPosHead);
                CheckBound(&ev);
                tb_set_cursor(gl_x, gl_y);
                tb_present();
                break;
        }
    }
    tb_shutdown();
    return TRUE;
}
