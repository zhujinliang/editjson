/* ************************************************************************
 *        Name: edit.c
 *  Programmer: ZhuJinliang
 * Description:
 *     Version: 1.0
 *     Created: 2011-11-16
 *    Function:
 * ***********************************************************************/
#include "includes.h"

int j = 1;
char *s_str = "Array";
int s_cnt;
int gl_schFlag = 0;
int gl_modifyFlag = 0;  
Record gl_record[SCREEN_SIZE];
int gl_x = 3;
int gl_y = 5;
Bound gl_bound[SCREEN_SIZE];
uint16_t gl_keyFlag = NONE;
 

/* Get option such as filename and schema name*/
void GetOption(int argc, char *argv[], char **sch)
{
    int ch;
    opterr = 0;

    while ((ch = getopt(argc, argv, "s:")) != -1)
    {
        switch (ch)
        {
            case 's':
                gl_schFlag = 1;
                (*sch) = (char*)malloc(strlen(optarg)+1);
                strcpy((*sch), optarg);
                break;
                
            case '?':
                printf("Unknown option: %c\n", (char)optopt);
                break;

            default:
               // printf("%s","Step into the default mode!/n");
                break;
        }
    }
}
/* Read file and get the text */
char *ReadFile(char *fileName)
{
    FILE *fp;
    long len;
    char *data;

    if (!(fp = fopen(fileName, "rb")))
    {
        //printf("can't open the file");
        return FALSE;
    }
    else
    {    
        fseek(fp, 0, SEEK_END);  // 找到文件的末尾
        len = ftell(fp);         // 得到文件的长度
        fseek(fp, 0, SEEK_SET);  // 将位置定在文件的开头
        data = (char *)malloc(len + 1);
        fread(data, 1, len, fp);
        fclose(fp);
    }
    return data;
}
/* Print string*/
void PrintStr(char *str, unsigned int x, unsigned int y, uint16_t fg, uint16_t bg)
{
    uint32_t unicode;
    while (*str)
    {
        str += utf8_char_to_unicode(&unicode, str);
        tb_change_cell(x, y, unicode, fg, bg);
        x ++;
    }
}

/* Printf */
void Printf(unsigned int x, unsigned int y, uint16_t fg, uint16_t bg, const char *fmt, ...)
{
    char buf[4096];
    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buf, sizeof(buf), fmt, vl);
    va_end(vl);
    PrintStr(buf, x, y, fg, bg);
}

/* Init window*/
int InitWindow(void)
{
    int ret;
    
    ret = tb_init();
    if (ret)
    {
        fprintf(stderr, "tb_init failed with err code %d\n", ret);
        return FALSE;
    }
    tb_select_input_mode(TB_INPUT_ESC);
    return TRUE;
}

/* Draw a window*/
void DrawWindow(char *filename, char *sch)
{
    int i;
    int len = 0;
    unsigned int width;
    unsigned int height;
    
    len = strlen(filename);
    width = tb_width() - 1;
    height = tb_height() - 1;

    PrintStr("edit json!", width*2/5, 1, TB_MAGENTA | TB_BOLD, TB_BLACK);
    PrintStr("filename:", 2, 2, TB_YELLOW, TB_BLACK);
    PrintStr(filename, 12, 2, TB_WHITE, TB_BLACK);
    if (gl_modifyFlag == 1)
    {
        PrintStr("(modified)", 14+len, 2, TB_RED, TB_BLACK);
    }
    PrintStr("schema:", width*3/5, 2, TB_YELLOW, TB_BLACK);
    PrintStr(sch, width*3/5+7, 2, TB_WHITE, TB_BLACK);
    //画框
    tb_change_cell(0, 0, 0x250c, TB_WHITE, TB_BLACK);
    tb_change_cell(width, 0, 0x2510, TB_WHITE, TB_BLACK);
    tb_change_cell(0, height, 0x2514, TB_WHITE, TB_BLACK);
    tb_change_cell(width, height, 0x2518, TB_WHITE, TB_BLACK);
    for (i=1; i<width; i++) //画4条横线
    {
        tb_change_cell(i, 0, 0x2500, TB_WHITE, TB_BLACK);
        tb_change_cell(i, 4, 0x2500, TB_WHITE, TB_BLACK);
        tb_change_cell(i, height-4, 0x2500, TB_WHITE, TB_BLACK);
        tb_change_cell(i, height, 0x2500, TB_WHITE, TB_BLACK);
    }
    for (i=1; i<height; i++) //画2条竖线
    {
        tb_change_cell(0, i, 0x2502, TB_WHITE, TB_BLACK);
        tb_change_cell(width, i, 0x2502, TB_WHITE, TB_BLACK);
    }
    tb_change_cell(0, 4, 0x251c, TB_WHITE, TB_BLACK);
    tb_change_cell(width, 4, 0x2524, TB_WHITE, TB_BLACK);
    tb_change_cell(0, height-4, 0x251c, TB_WHITE, TB_BLACK);
    tb_change_cell(width, height-4, 0x2524, TB_WHITE, TB_BLACK);
    
    if (gl_keyFlag == TB_KEY_CTRL_W)
    {
        //write/save
        Printf(1, (height-3), TB_BLACK, TB_WHITE, "%s", "File name to write to?");
        Printf(1, (height-2), TB_BLACK, TB_WHITE, "%s", "Enter:  Confirm");
        Printf(1, (height-1), TB_BLACK, TB_WHITE, "%s", "ESC:  Cancel");
    }
    else if ((gl_keyFlag == TB_KEY_CTRL_X) && (gl_modifyFlag == 1))
    {
        //write/save
        Printf(1, (height-3), TB_BLACK, TB_WHITE, "%s", "Save changes (Answering 'No' will destory changes)?");
        Printf(1, (height-2), TB_BLACK, TB_WHITE, "%s", "Y  Yes");
        Printf((width/2), (height-2), TB_BLACK, TB_WHITE, "%s", "ESC  Cancel");
        Printf(1, (height-1), TB_BLACK, TB_WHITE, "%s", "N  No");
    }
    else
    {
        PrintStr("^W", 2, height-2, TB_BLACK, TB_WHITE);
        PrintStr("Write/Save", 6, height-2, TB_WHITE, TB_BLACK);
        //PrintStr("^N", width/2, height-2, TB_BLACK, TB_WHITE);
        //PrintStr("Inset New Item", width/2+4, height-2, TB_WHITE, TB_BLACK);
        PrintStr("^X", 2, height-1, TB_BLACK, TB_WHITE);
        PrintStr("Exit", 6, height-1, TB_WHITE, TB_BLACK);
        //PrintStr("^D", width/2, height-1, TB_BLACK, TB_WHITE);
        //PrintStr("Delete Item", width/2+4, height-1, TB_WHITE, TB_BLACK);
    }
}

/* Print json name*/
void PrintJson(void *ptr)
{
    unsigned int x = 0;
    unsigned int y = 0;
    char *str = NULL;
    int type = 0;

    x = ((JsonPos*)ptr)->x;
    y = ((JsonPos*)ptr)->y;
    str = ((JsonPos*)ptr)->jsonPtr->string;
    type = ((JsonPos*)ptr)->jsonPtr->type;
    
    switch (type)
    {
        case cJSON_Object:
            if (str)
            {
                Printf(x, y, TB_CYAN, TB_BLACK, "-%s:", str);
            }
            else
            {
                s_cnt ++;
                Printf(x, y, TB_CYAN, TB_BLACK, "-%s %s #%d:", s_str, "item", s_cnt);
            }
            break;
        case cJSON_Array:
            if (str)
            {
                s_str = str;
            }
            Printf(x, y, TB_CYAN, TB_BLACK, "-%s:", s_str);
            s_cnt = 0;
            break;
        default:
            Printf(x, y, TB_CYAN, TB_BLACK, "%s:", str);
            break;
    }
}
/* Display json information*/
int Display(ListNode *listHead)
{

    TraverseList(listHead, PrintJson);

    DisplayInput(listHead);

    return TRUE;
}


/* Display the input info*/
int DisplayInput(ListNode *listHead)
{
    int i = 0;
    int x = 0;
    int len = 0;

    for (i=0; i<SCREEN_SIZE; i++)
    {
        if ((i > EDIT_BOTTOM) && (i < EDIT_TOP))
        {
            if (gl_record[i].str != NULL)
            {
                x = gl_record[i].x;
                Printf(x, i, TB_WHITE, TB_BLACK, "%s", gl_record[i].str);
            }
        }
        if (gl_keyFlag == TB_KEY_CTRL_W)
        {
            if ((gl_record[i].str != NULL) && (i == (tb_height() - 4)))
            {
                x = gl_record[i].x;
                Printf(x, i, TB_WHITE, TB_BLACK, "%s", gl_record[i].str);
            }
        }
    }
}
/* transfer text to json*/
cJSON *Text2Json(char *text)
{
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json)
    {
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
        return FALSE;
    }
    else
    {
        return json;
    }
}

/* Compare the row*/
int Compare(void *r1, void *r2)
{
    if (((JsonPos*)r1)->y == (*(int*)r2))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/* parse all the item, and display them on the screen, then save to record*/
int ParseObject(cJSON *item, ListNode *ListHead, int32_t w, int32_t h)
{
    cJSON *subItem = item;
    int i = 1;
    int tempy = 0;
    int len = 0;
    char buf[7];
    JsonPos *pJsonPos = NULL;
    ListNode *pNode = NULL;
    ListNode *node = NULL;
    
    
    tempy = h+j;
    pNode = ListHead;
    while (subItem)
    {
        switch (subItem->type)
        {
            case cJSON_Object:
                 gl_bound[h+j].left = w+i+1;
                 gl_bound[h+j].right = gl_bound[h+j].left;
                 break;
            case cJSON_Array:
                 gl_bound[h+j].left = w+i+1;
                 gl_bound[h+j].right = gl_bound[h+j].left;
                 break;
            case cJSON_String:
                 gl_record[h+j].str = (char*)malloc((strlen(subItem->valuestring) * sizeof(char))+1);
                 strcpy(gl_record[h+j].str, subItem->valuestring); // 把输出显示的copy给屏幕保存
                 gl_record[h+j].x = w+i+16;
                 len = strlen(subItem->valuestring);
                 gl_bound[h+j].left = w+i+16;
                 gl_bound[h+j].right = gl_bound[h+j].left + len;
                 break;
            case cJSON_Number:
                 sprintf(buf, "%d", subItem->valueint);
                 gl_record[h+j].str = (char*)malloc((strlen(buf) * sizeof(char))+1);
                 strcpy(gl_record[h+j].str, buf);
                 gl_record[h+j].x = w+i+16;
                 len = strlen(buf);
                 gl_bound[h+j].left = w+i+16;
                 gl_bound[h+j].right = gl_bound[h+j].left + len;
                 break;
            case cJSON_False:
                 gl_record[h+j].str = (char*)malloc((strlen("[ ]") * sizeof(char))+1);
                 strcpy(gl_record[h+j].str, "[ ]");
                 gl_record[h+j].x = w+i+16;
                 gl_bound[h+j].left = w+i+17;
                 gl_bound[h+j].right = gl_bound[h+j].left;
                 break;
            case cJSON_True:
                 gl_record[h+j].str = (char*)malloc((strlen("[X]") * sizeof(char))+1);
                 strcpy(gl_record[h+j].str, "[X]");
                 gl_record[h+j].x = w+i+16;
                 gl_bound[h+j].left = w+i+17;
                 gl_bound[h+j].right = gl_bound[h+j].left;
                 break;
            default:
                 break;
        }
        tempy = h+j;
        if (!(node = SearchNode(pNode, &tempy, Compare))) //对应行没有分配空间
        {
            pJsonPos = (JsonPos *)malloc(sizeof(JsonPos));
            pJsonPos->jsonPtr = subItem;
            pJsonPos->x = w+i;
            pJsonPos->y = h+j;
            if (!(AddTail(pNode, (void *)pJsonPos)))
            {
                Printf(50, tempy, TB_RED, TB_BLACK, "Add node err!");
                return FAIL;
            }
        }
        else
        {
            ((JsonPos*)(node->data))->jsonPtr = subItem;
            ((JsonPos*)(node->data))->x = w+i;
            ((JsonPos*)(node->data))->y = h+j;
         }
        j ++;
        if (subItem->child)
        {
            ParseObject(subItem->child, pNode, w+i+2, h);  //访问子节点
        }
        subItem = subItem->next;                    //访问兄弟节点 
    }
    return TRUE;
}

/* Save the key value that input*/ 
int InputKey(struct tb_event *ev, ListNode *pHead)
{
    char buf[2];
    int len = 0;
    int len2 = 0;
    int i =0;
    int y = 0;
    ListNode *node;
    
    y = gl_y;
    if (gl_record[y].str != NULL)
    {
        len = strlen(gl_record[gl_y].str);
    }
    else
    {
        gl_record[y].str = (char*)malloc(sizeof(char));
        strcpy(gl_record[y].str, "");
        len = strlen(gl_record[gl_y].str);
    }
    if ((node = SearchNode(pHead, &y, Compare)))
    {
        if (((((JsonPos*)(node->data))->jsonPtr->type) == cJSON_Object)
         || ((((JsonPos*)(node->data))->jsonPtr->type) == cJSON_Array))
        { 
            return TRUE;
        }
        else if ((((JsonPos*)(node->data))->jsonPtr->type) == cJSON_True)
        {
            if (ev->key == TB_KEY_ENTER)
            {
                strcpy(gl_record[y].str, "[ ]");
                gl_modifyFlag = 1;
            }
            return TRUE;
        }
        else if (((((JsonPos*)(node->data))->jsonPtr->type) == cJSON_False))
        {
            if (ev->key == TB_KEY_ENTER)
            {
                strcpy(gl_record[y].str, "[X]");
                gl_modifyFlag = 1;
            }
            return TRUE;
        }
    }
    else
    {
        return TRUE;
    }
    if (ev->key == TB_KEY_BACKSPACE2)
    {
        //删除光标前一个
        if ((len2 = len) <= 1)
        {
            len2 = 1;
        }
        for (i=(gl_x - gl_record[y].x - 1); i<len; i++)
        {
            gl_record[y].str[i] = gl_record[y].str[i+1];
        }
        gl_record[y].str = (char*)realloc(gl_record[y].str, len2);
        gl_x --;
        gl_modifyFlag = 1;
    }
    else if (ev->key == TB_KEY_DELETE)
    {
        //删除光标当前所在位置的信息
        if ((len2 = len) <= 1)
        {
            len2 = 1;
        }
        for (i=(gl_x - gl_record[y].x); i<len; i++)
        {
            gl_record[y].str[i] = gl_record[y].str[i+1];
        }
        gl_bound[y].right = gl_bound[y].left + len - 1;
        gl_x --;
        gl_modifyFlag = 1;
    }
    else if ((ev->ch >= 0x20) && (ev->ch <= 0x7B) || (ev->key == 0x20)) //输入的是字符信息
    {
        len2 = len + 2;  //实际数组长度为字符串长度+1, '\0'
        if (ev->key == 0x20)
        {
            buf[utf8_unicode_to_char(buf, ev->key)] = '\0';
        }
        else
        {
            buf[utf8_unicode_to_char(buf, ev->ch)] = '\0';
        }
        gl_record[y].str = (char*)realloc(gl_record[y].str, len2);
        
        if ((gl_x < (gl_record[y].x + len)) && (gl_x > gl_record[y].x)) // 在中间插入字符
        {
            for (i=len; i>=(gl_x - gl_record[y].x); i--)
            {
                gl_record[y].str[i+1] = gl_record[y].str[i];
            }
            gl_record[y].str[++i] = buf[0];
        }
        else if (gl_x <= gl_record[y].x) // 在最前面插入字符
        {
            for (i=len; i>=0; i--)
            {
                gl_record[y].str[i+1] = gl_record[y].str[i];
            }
            gl_record[y].str[++i] = buf[0];
        }
        else // 光标紧跟在最后一个字符后面
        {
            strcat(gl_record[y].str, buf);
            gl_record[y].x = gl_bound[y].left;
        }
        gl_bound[y].right = gl_bound[y].left + len + 1;
        gl_x ++;
        gl_modifyFlag = 1;
    }
    return TRUE;
}

/* Save the screen infomation to the json*/
void SaveToJson(ListNode *head)
{
    int i = 0;
    int type = 0;
    ListNode *pHead = head;
    ListNode *node = NULL;

    for (i=EDIT_BOTTOM; i<=EDIT_TOP; i++)
    {
        if ((gl_record[i].str != NULL) && (strlen(gl_record[i].str) > 0) && (gl_keyFlag == NONE)) //屏幕有输入信息的行的信息复制给json
        {
            if (!(node = SearchNode(pHead, &i, Compare)))
            {
                Printf(65,i, TB_WHITE,TB_BLACK, "%s","can not find node!");
            }
            else
            {
                type = ((JsonPos*)(node->data))->jsonPtr->type;
                //可以根据当前行的json中的不同数据类型来将字符串转换过去。
                switch (type)
                {
                    case cJSON_String:
                        ((JsonPos*)(node->data))->jsonPtr->valuestring = gl_record[i].str;
                        break;
                    case cJSON_Number:
                        ((JsonPos*)(node->data))->jsonPtr->valueint = atoi(gl_record[i].str);
                        break;
                    case cJSON_False:
                        if (!(strcmp(gl_record[i].str, "[X]")))
                        {
                            ((JsonPos*)(node->data))->jsonPtr->type = cJSON_True;
                        }
                        break;
                    case cJSON_True:
                        if (!(strcmp(gl_record[i].str, "[ ]")))
                        {
                            ((JsonPos*)(node->data))->jsonPtr->type = cJSON_False;
                        }
                        break;
                }
            }
        }
    }
}

/* Save the json structure to disk*/
int SaveToDisk(char *filename, cJSON *jsonPtr)
{
    FILE *fp;
    char *out;
    int len;
    int i = 0;

    if (!(fp = fopen(filename, "wt")))
    {
        Printf(1,1, TB_RED, TB_BLACK, "%s", "Can not creat the file!");
        return FALSE;
    }
    else
    { 
        out = cJSON_Print(jsonPtr);
        len = strlen(out);
        fwrite(out, sizeof(char), len, fp); 
        free(out);
        fclose(fp);
    }
    return TRUE;
}
/* Get the filename*/
char *GetFilename(char *path)
{
    char *filename;
    if (filename = strrchr(path, '/')) //delete the directory, then get the file name
    {
        filename ++;
    }
    else
    {
        filename = path;
    }
    return filename;
}

/* Check the cusor bound*/
void CheckBound(struct tb_event *evP)
{
    int y = 0;

    y = gl_y;
    if (evP->key == TB_KEY_ARROW_UP) gl_y --;
    if (evP->key == TB_KEY_ARROW_DOWN) gl_y ++;
    if (evP->key == TB_KEY_ARROW_LEFT) gl_x --;
    if (evP->key == TB_KEY_ARROW_RIGHT) gl_x ++;

    if (evP->key == TB_KEY_HOME) gl_x = gl_bound[y].left;
    if (evP->key == TB_KEY_END) gl_x = gl_bound[y].right;
   
    if (gl_x < gl_bound[y].left) gl_x = gl_bound[y].left;
    if (gl_x > gl_bound[y].right) gl_x = gl_bound[y].right;
    
    if (gl_keyFlag == TB_KEY_CTRL_W)
    {
        if (gl_x > EDIT_RIGHT) gl_x = EDIT_RIGHT;
        if (gl_x < EDIT_LEFT) gl_x = EDIT_LEFT;
        if (gl_y < tb_height()-3) gl_y = tb_height()-3;
        if (gl_y > tb_height()-4) gl_y = tb_height()-4;
    }
    if (gl_keyFlag == NONE)
    {
        if (gl_x > EDIT_RIGHT) gl_x = EDIT_RIGHT;
        if (gl_x < EDIT_LEFT) gl_x = EDIT_LEFT;
        if (gl_y < EDIT_BOTTOM) gl_y = EDIT_BOTTOM;
        if (gl_y > EDIT_TOP) gl_y = EDIT_TOP;
    }
}


cJSON *EditJson(cJSON *json, char *filename)
{
    int i = 0;
    char *s = "Array";
    char *data;
    char buf[100];
    char schema[] = "auto generate schema";
    char tempCh[2];
    ListNode *jsonPosHead = NULL;
    struct tb_event ev;
    struct tb_cell cell;
    
    if (!(jsonPosHead = InitList()))
    {
        Printf(40,1,TB_RED,TB_BLACK, "%s","List init err!");
    }
    InitWindow();
    DrawWindow(filename, schema);
    ParseObject(json, jsonPosHead, 2, 4);
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
                                return json;
                            }
                        }
                        else if ((tempCh[0] == 'n') || (tempCh[0] == 'N'))
                        {
                            tb_shutdown();
                            return json;
                        }
                        
                    }
                    else
                    {
                        tb_shutdown();
                        return json;
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
                DrawWindow(filename, schema);
                InputKey(&ev, jsonPosHead);
                SaveToJson(jsonPosHead);//把输入的内容复制给json结构体
                Display(jsonPosHead);
                CheckBound(&ev);
                tb_set_cursor(gl_x, gl_y);
                tb_present();
                break;

            case TB_EVENT_RESIZE: //调整窗口大小
                tb_clear();
                DrawWindow(filename, schema);
                j = 1;
                s_str = s;
                s_cnt = 0;
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
    return json;
}

