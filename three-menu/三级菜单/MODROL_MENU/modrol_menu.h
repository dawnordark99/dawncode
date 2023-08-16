#ifndef _MODROL_MENU_H
#define _MODROL_MENU_H

#include <stdio.h>
#include <stdint.h>
#include "GPIO_int.h"
#include "Variables.h"


#define GET_BIT(data, bit) ((data >> bit) & 0x1)
#define GET_BITS(data, start, end) ((data >> start) & ((1 << (end - start + 1)) - 1))


/*  ����״̬*/
#define RUN_short 1
#define RUN_long 2
#define STOP_short 3
#define STOP_long 4
#define UP_short 5
#define UP_long 6
#define DOWN_short 7
#define DOWN_long 8

// ���������״̬ö��
#define LEVEL1 0
#define LEVEL2 1
#define LEVEL3 2   //��ʾ����ҳ
#define LEVEL4 3   //�޸Ĳ���ҳ
#define LEVEL5 4   //��ʾ�޸ĳɹ�
//�ĸ����Ӳ���
#define MNT_page 10
#define U_page   3
#define OP_page  2
#define F_page   1
#define P_page   0
//����λ��
#define Postion_X -10
#define Postion_Y 0



typedef struct {
    uint8_t  level;         //�������
	uint8_t  page;          //�Ĵ������
	uint8_t  state;	        //����״̬
	
	int32_t  init_idx;       //����	
	uint8_t  symbol;         //����λ
	uint8_t  ifwrite;        //��д����
	uint8_t  format;         //��ʾ��ʽ
	uint16_t maximum ;      //���ֵ
	uint16_t minimum ;      //��Сֵ
	
	uint8_t  Pvar_bit ;       //�����޸Ĳ����ĵڼ�λ�б�
	uint8_t  Uvar_bit;       //�����޸Ĳ����ĵڼ�λ�б�
	uint8_t  OPvar_bit;       //�����޸Ĳ����ĵڼ�λ�б�
	uint8_t  Fvar_bit;       //�����޸Ĳ����ĵڼ�λ�б�
	uint8_t  flag;
	
	
    int8_t  Ptwo;                  //����ϵͳPλ��
	int8_t  Utwo;                //����ϵͳUλ��
	int8_t  OPtwo;             //����ϵͳOPλ��
	int8_t  Ftwo;              //����ϵͳFλ��
	
	int8_t  Pthree[5];         //����ϵͳPλ��
	int8_t  Uthree[8];         //����ϵͳUλ��
	int8_t  Fthree[12];        //����ϵͳFλ��
	int8_t  OPthree[9];        //����ϵͳOPλ��
	
	int32_t Pset_var;           //�޸�֮���P�������������ַ
	int32_t Uset_var;           //�޸�֮���U�������������ַ
	int32_t OPset_var;          //�޸�֮���OP�������������ַ
	int32_t Fset_var;           //�޸�֮���F�������������ַ

} G_Key;  //���� 

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


