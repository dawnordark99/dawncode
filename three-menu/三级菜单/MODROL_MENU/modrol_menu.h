#ifndef _MODROL_MENU_H
#define _MODROL_MENU_H

#include <stdio.h>
#include <stdint.h>
#include "GPIO_int.h"
#include "Variables.h"


#define GET_BIT(data, bit) ((data >> bit) & 0x1)
#define GET_BITS(data, start, end) ((data >> start) & ((1 << (end - start + 1)) - 1))


/*  按键状态*/
#define RUN_short 1
#define RUN_long 2
#define STOP_short 3
#define STOP_long 4
#define UP_short 5
#define UP_long 6
#define DOWN_short 7
#define DOWN_long 8

// 三级界面的状态枚举
#define LEVEL1 0
#define LEVEL2 1
#define LEVEL3 2   //显示参数页
#define LEVEL4 3   //修改参数页
#define LEVEL5 4   //显示修改成功
//四个监视参数
#define MNT_page 10
#define U_page   3
#define OP_page  2
#define F_page   1
#define P_page   0
//参数位置
#define Postion_X -10
#define Postion_Y 0



typedef struct {
    uint8_t  level;         //代码层数
	uint8_t  page;          //四大类参数
	uint8_t  state;	        //按键状态
	
	int32_t  init_idx;       //变量	
	uint8_t  symbol;         //符号位
	uint8_t  ifwrite;        //读写限制
	uint8_t  format;         //显示格式
	uint16_t maximum ;      //最大值
	uint16_t minimum ;      //最小值
	
	uint8_t  Pvar_bit ;       //用来修改参数的第几位判别
	uint8_t  Uvar_bit;       //用来修改参数的第几位判别
	uint8_t  OPvar_bit;       //用来修改参数的第几位判别
	uint8_t  Fvar_bit;       //用来修改参数的第几位判别
	uint8_t  flag;
	
	
    int8_t  Ptwo;                  //二级系统P位置
	int8_t  Utwo;                //二级系统U位置
	int8_t  OPtwo;             //二级系统OP位置
	int8_t  Ftwo;              //二级系统F位置
	
	int8_t  Pthree[5];         //三级系统P位置
	int8_t  Uthree[8];         //三级系统U位置
	int8_t  Fthree[12];        //三级系统F位置
	int8_t  OPthree[9];        //三级系统OP位置
	
	int32_t Pset_var;           //修改之后的P，用来保存进地址
	int32_t Uset_var;           //修改之后的U，用来保存进地址
	int32_t OPset_var;          //修改之后的OP，用来保存进地址
	int32_t Fset_var;           //修改之后的F，用来保存进地址

} G_Key;  //按键 

struct DataFields {
    int parameter_position;
    int length;
    int display_format;
    int sign_bit;
    int menu;
    int write_restriction;
    int associate_attribute;
};


extern  G_Key key;

void showMenu(G_Key *menu);
struct DataFields parse_data(unsigned short data);
void Display_format(uint8_t point);
void Cursor_position(uint8_t var_bit);
void Show_data(int32_t data  ,uint16_t max_data);
void Judge_positive_negative(int sign_bit,int32_t data);
#endif


