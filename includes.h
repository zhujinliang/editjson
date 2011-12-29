/* ************************************************************************
 *        Name: includes.h
 *  Programmer: ZhuJinliang
 * Description:
 *     Version: 1.0
 *     Created: 2011-11-16
 *    Function:
 * ***********************************************************************/
#ifndef _INCLUDES_H
#define _INCLUDES_H

#include <assert.h>
#include <stdint.h>
#include <stdarg.h>
#include <sys/types.h>	        
#include <stdio.h>		
#include <stdlib.h>		
#include <unistd.h>
#include <string.h>		
#include <malloc.h>

#include "termbox.h"
#include "cJSON.h"
#include "LinkList.h"


#define  SCREEN_SIZE   50
#define  TRUE          1
#define  FALSE         0

#define  EDIT_LEFT     2
#define  EDIT_RIGHT    (tb_width() - 2)
#define  EDIT_BOTTOM   5
#define  EDIT_TOP      (tb_height() - 6)

#define NONE           0


typedef struct jsonPostion
{
    cJSON *jsonPtr;
    unsigned int x;
    unsigned int y;
}JsonPos;

typedef struct record
{
    char *str;       // store the string
    unsigned int x;  // save the position of the first input 
}Record;

typedef struct bound
{
    unsigned int left;
    unsigned int right;
}Bound;


/* global valible declaration*/
extern int j;
extern char *s_str;
extern int s_cnt;
extern int gl_schFlag;
extern int gl_modifyFlag;  
extern Record gl_record[SCREEN_SIZE];
extern int gl_x;
extern int gl_y;
extern Bound gl_bound[SCREEN_SIZE];
extern uint16_t gl_keyFlag;

/* function declaration*/
/* Get option such as filename and schema name*/
void GetOption(int argc, char *argv[], char **sch);
/* Read file and get the text */
char *ReadFile(char *fileName);
/* Print string*/
void PrintStr(char *str, unsigned int x, unsigned int y, uint16_t fg, uint16_t bg);
/* Printf */
void Printf(unsigned int x, unsigned int y, uint16_t fg, uint16_t bg, const char *fmt, ...);
/* Init window*/
int InitWindow(void);
/* Draw a window*/
void DrawWindow(char *filename, char *sch);
/* Print json name*/
void PrintJson(void *ptr);
/* Display json information*/
int Display(ListNode *listHead);
/* Display the input info*/
int DisplayInput(ListNode *listHead);
/* transfer text to json*/
cJSON *Text2Json(char *text);
/* Compare the row*/
int compare(void *r1, void *r2);
/* parse all the item, and display them on the screen, then save to record*/
int ParseObject(cJSON *item, ListNode *ListHead, int32_t w, int32_t h);
/* Save the key value that input*/ 
int InputKey(struct tb_event *ev, ListNode *pHead);
/* Save the screen infomation to the json*/
void SaveToJson(ListNode *head);
/* Save the json structure to disk*/
int SaveToDisk(char *filename, cJSON *jsonPtr);
/* Get the filename*/
char *GetFilename(char *path);
/* Check the cusor bound*/
void CheckBound(struct tb_event *evP);

cJSON *EditJson(cJSON *json, char *filename);


#endif 
