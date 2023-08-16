#include "modrol_menu.h"

#include "OLED.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DataFields parse_data(unsigned short data) //读取菜单数据
{ 
    struct DataFields fields;
    fields.parameter_position = GET_BIT(data, 15);
    fields.length =  GET_BITS(data, 13, 14);
    fields.display_format = GET_BITS(data, 8, 12);
    fields.sign_bit = GET_BIT(data, 7);
    fields.menu = GET_BIT(data, 6);
    fields.write_restriction = GET_BITS(data, 4, 5);
    fields.associate_attribute = GET_BITS(data, 0, 3);
    return fields;
}

void showMenu(G_Key *menu)
{
	
     struct DataFields Presult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+ATTR_IDX]);  
     struct DataFields Uresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+ATTR_IDX]);      
     struct DataFields Fresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+ATTR_IDX]);          

	if( menu->level <4 )    //第一页显示1,2,3级，通过不同的level来切换光标查询想要看的数据
		
	{	
		if(key.Ptwo <= 1) key.Ptwo = 1;	
		if(key.OPtwo <= 1) key.OPtwo = 1;
		if(key.Pthree[key.Ptwo] <= 1) key.Pthree[key.Ptwo] = 1;
		if(key.Uthree[key.Utwo] <= 1) key.Uthree[key.Utwo] = 1;
		if(key.Fthree[key.Ftwo] <= 1) key.Fthree[key.Ftwo] = 1;
	
		   if(menu->page == P_page)
		{
			OLED_ShowString( Postion_X+40 ,4,"  P ",16);	
		    OLED_ShowNum( Postion_X+72 ,4,menu->Ptwo,1,16);	
		    OLED_ShowNum( Postion_X+88 ,4,(menu->Pthree[menu->Ptwo])/10%10,1,16);
		    OLED_ShowNum( Postion_X+104 ,4,menu->Pthree[menu->Ptwo]%10,1,16);  			
							
		}
	   else if (menu->page == U_page)
		{
			OLED_ShowString( Postion_X+40 ,4,"  U ",16);	
		    OLED_ShowNum( Postion_X+72 ,4,menu->Utwo,1,16);	
		    OLED_ShowNum( Postion_X+88 ,4,(menu->Uthree[menu->Utwo])/10%10,1,16);
		    OLED_ShowNum( Postion_X+104 ,4,menu->Uthree[menu->Utwo]%10,1,16);  			
		}
	   else if (menu->page == OP_page)
	   {
			OLED_ShowString( Postion_X+40 ,4,"O P ",16);
		   	OLED_ShowNum( Postion_X+72 ,4,menu->OPtwo,1,16);
            OLED_ShowString( Postion_X+88 ,4,"   ",16);		
	   }
	   else if (menu->page == F_page)
	   {	
			OLED_ShowString( Postion_X+40 ,4,"  F ",16);	
			if(menu->Ftwo<=9) OLED_ShowNum( Postion_X+72 ,4,menu->Ftwo,1,16);
		   else if (menu->Ftwo==10) OLED_ShowString( Postion_X+72 ,4,"A",16);
		   else if (menu->Ftwo==11) OLED_ShowString( Postion_X+72 ,4,"P",16);
		   OLED_ShowNum( Postion_X+88 ,4,(menu->Fthree[menu->Ftwo])/10%10,1,16);
		   OLED_ShowNum( Postion_X+104 ,4,menu->Fthree[menu->Ftwo]%10,1,16); 		   
	   }
		    OLED_ShowString( Postion_X+24 ,4," ",16);
	        OLED_ShowString( Postion_X+80 ,4,".",4);	   
	        OLED_ShowString( Postion_X+40 ,6," ",16);		
		 if(menu->level == LEVEL1) 
		 {
			OLED_ShowString( Postion_X+56 ,6,"-       ",16);		
		 }		
       else if(menu->level == LEVEL2) 
	   {
           OLED_ShowString( Postion_X+56 ,6,"  -    ",16);		

	    }			   
      else if(menu->level == LEVEL3) 
	   {
            OLED_ShowString( Postion_X+56 ,6,"    -    ",16);			   
	        OLED_ShowString( Postion_X+80 ,4,".",4);		   	   
	   }

      else if(menu->level == LEVEL4) 
	   {
            OLED_ShowString( Postion_X+56 ,6,"      -  ",16);			   
	        OLED_ShowString( Postion_X+80 ,4,".",4);		   	   
	   }	   
	   
   }   
	   

	 else  
	   
   {  
	  if((menu->flag == 1||menu->flag==0)&& menu->level ==LEVEL5)   //进入菜单查看数据
	   {		   
			if(menu->page == P_page )
				{
					 	
					menu->init_idx = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+INIT_IDX]; 
					menu->Pset_var = menu->init_idx;

                    Judge_positive_negative(Presult.sign_bit,menu->init_idx);
                    Show_data(menu->init_idx ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]);
					Display_format(Presult.display_format);	
				}
		   else if (menu->page == U_page)
				{
					menu->init_idx = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+INIT_IDX];	
					menu->Uset_var = menu->init_idx;

					Judge_positive_negative(Uresult.sign_bit,menu->init_idx);
	                Show_data(menu->init_idx ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]);
                    Display_format(Uresult.display_format);				
				}
//		   else if (menu->page == OP_page)
//			   {
//				  OLED_ShowNum( Postion_X+72 ,4,menu->OPtwo,1,16);
//                  OLED_ShowString( Postion_X+88 ,4,"   ",16);						   
//			   }
		   else if (menu->page == F_page)
			   {	
				    
		            menu->init_idx = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+INIT_IDX];	
                    menu->Fset_var= menu->init_idx;
			   
			        Judge_positive_negative(Fresult.sign_bit,menu->init_idx);
                    Show_data(menu->init_idx ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]);
		            Display_format(Fresult.display_format);
			   
			   }
			   OLED_ShowString( Postion_X+40 ,6,"          ",16);	//光标清空   
	   }
		
	  else if(menu->flag >1&&menu->level ==LEVEL5) /**********再次进入菜单修改数据*********/
	   {

				if(menu->page == P_page)
			{

				 Judge_positive_negative(Presult.sign_bit,menu->Pset_var);
				 Show_data(menu->Pset_var ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]);
				 Display_format(Presult.display_format);
				 Cursor_position(menu->Pvar_bit);					
			}
	   else if (menu->page == U_page)
			{

				Judge_positive_negative(Uresult.sign_bit,menu->Uset_var);
				Show_data(menu->Uset_var ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]);
				Display_format(Uresult.display_format);
				Cursor_position(menu->Uvar_bit);
			}
//		   else if (menu->page == OP_page)
//			   {
//				  OLED_ShowNum( Postion_X+72 ,4,menu->OPtwo,1,16);
//                  OLED_ShowString( Postion_X+88 ,4,"   ",16);						   
//			   }
	   else if (menu->page == F_page)
		   {					    					
			   Judge_positive_negative(Fresult.sign_bit,menu->Fset_var);
			   Show_data(menu->Fset_var ,M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX] );
			   Display_format(Fresult.display_format);
			   Cursor_position(menu->Fvar_bit);
		   }    
		   
	   }
   }
}


void Cursor_position(uint8_t var_bit) //光标位置
	
{
	 if(var_bit == 3) 
	{
	OLED_ShowString( Postion_X+56 ,6,"-       ",16);		
	}		
	else if(var_bit == 2) 
	{
	OLED_ShowString( Postion_X+56 ,6,"  -    ",16);		

	}			   
	else if(var_bit == 1) 
	{
	OLED_ShowString( Postion_X+56 ,6,"    -    ",16);			   

	}
	else if(var_bit == 0) 
	{
	OLED_ShowString( Postion_X+56 ,6,"      -  ",16);			   

	}	
	else if(var_bit == 4) 
	{
	OLED_ShowString( Postion_X+40 ,6,"-         ",16);			   
	}	

}

void Display_format(uint8_t point)  //小数点位置
{
	if(point == 0)
	{
	  OLED_ShowString( Postion_X+64 ,4," ",16);	
	  OLED_ShowString( Postion_X+80 ,4," ",16);							
	  OLED_ShowString( Postion_X+96 ,4," ",16);	
	}
	else if(point == 1) 
	{
	  OLED_ShowString( Postion_X+64 ,4," ",16);	
	  OLED_ShowString( Postion_X+80 ,4," ",16);							
	  OLED_ShowString( Postion_X+96 ,4,".",16);	
	}					
	else if(point == 2) 
	{						
	  OLED_ShowString( Postion_X+64 ,4," ",16);	
	  OLED_ShowString( Postion_X+80 ,4,".",16);							
	  OLED_ShowString( Postion_X+96 ,4," ",16);	
	}					
	else if(point == 3) 
	{						
	  OLED_ShowString( Postion_X+64 ,4,".",16);	
	  OLED_ShowString( Postion_X+80 ,4," ",16);							
	  OLED_ShowString( Postion_X+96 ,4," ",16);	
	}

}


void Show_data(int32_t data  ,uint16_t max_data)   //显示数据

{
		if(max_data>9999)
	{											
		 OLED_ShowNum( Postion_X+40 ,4,data/10000%10,1,16);
		 OLED_ShowNum( Postion_X+56 ,4,data/1000%10,1,16);
		 OLED_ShowNum( Postion_X+72 ,4,data/100%10,1,16);
		 OLED_ShowNum( Postion_X+88 ,4,data/10%10,1,16);
		 OLED_ShowNum( Postion_X+104 ,4,data%10,1,16);				
	}
	
	else if(max_data>999 && max_data<=9999)
	{											
		 OLED_ShowString( Postion_X+40 ,4," ",16);
		 OLED_ShowNum( Postion_X+56 ,4,data/1000%10,1,16);
		 OLED_ShowNum( Postion_X+72 ,4,data/100%10,1,16);
		 OLED_ShowNum( Postion_X+88 ,4,data/10%10,1,16);
		 OLED_ShowNum( Postion_X+104 ,4,data%10,1,16);				
	}			
	else if(max_data>99 && max_data<=999)
	{											
		 OLED_ShowString( Postion_X+40 ,4," ",16);	
		 OLED_ShowString( Postion_X+56 ,4," ",16);
		 OLED_ShowNum( Postion_X+72 ,4,data/100%10,1,16);
		 OLED_ShowNum( Postion_X+88 ,4,data/10%10,1,16);
		 OLED_ShowNum( Postion_X+104 ,4,data%10,1,16);				
	}			
	else if(max_data>9 && max_data<=99)
	{											
		 OLED_ShowString( Postion_X+40 ,4," ",16);	
		 OLED_ShowString( Postion_X+56 ,4," ",16);					
		 OLED_ShowString( Postion_X+72 ,4," ",16);
		 OLED_ShowNum( Postion_X+88 ,4,data/10%10,1,16);
		 OLED_ShowNum( Postion_X+104 ,4,data%10,1,16); 				
	}
	
	else if(max_data>=0 && max_data<=9)
	{											
		 OLED_ShowString( Postion_X+40 ,4," ",16);	
		 OLED_ShowString( Postion_X+56 ,4," ",16);					
		 OLED_ShowString( Postion_X+72 ,4," ",16);	
		 OLED_ShowString( Postion_X+88 ,4," ",16);	
		 OLED_ShowNum( Postion_X+104 ,4,data%10,1,16); 
		
	}

}

void Judge_positive_negative(int sign_bit,int32_t data) //判断正负
{

	if(sign_bit == 0)
	{
		 OLED_ShowString( Postion_X+24 ,4,"+",16);
	}
	else if (sign_bit == 1)							
	{	
		if(data >= 0) OLED_ShowString( Postion_X+24 ,4,"+",16);
		else OLED_ShowString( Postion_X+24 ,4,"-",16);
	}


}

/*****************************打印数据***************************************************************/
//	                printf("P-parameter_position: %d\n", Presult.parameter_position);
//					printf("P-length: %d\n", Presult.length);
//					printf("P-display_format: %d\n", Presult.display_format);
//					printf("P-sign_bit: %d\n", Presult.sign_bit);
//					printf("P-menu: %d\n", Presult.menu);
//					printf("P-write_restriction: %d\n", Presult.write_restriction);
//					printf("P-associate_attribute: %d\n", Presult.associate_attribute);
//					printf("P-地址: %x\n\r", M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+0]);
//  printf("yi%d\n\r",key.level);	
//	printf("er%d\n\r",key.flag);
//	printf("P%d\n\r",key.Pvar_bit);	
//	printf("U%d\n\r",key.Uvar_bit);
//	printf("F%d\n\r",key.Fvar_bit);	
//  printf("FMAX%d\n\r",M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo]]+key.Fthree[key.Ftwo]])+MAX_IDX]);	
//	printf("FINIT%d\n\r",M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo]]+key.Fthree[key.Ftwo]])+INIT_IDX]);	
//	printf("FMIN%d\n\r",M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo]]+key.Fthree[key.Ftwo]])+MIN_IDX]);	


// printf("shiyi%d\n\r", key.Pset_var);	
// printf("shier%d\n\r", key.Uset_var);	  
// printf("shisav%d\n\r", key.Fset_var);	
//	    printf("yi%d\n\r",key.Utwo);
//	    printf("er%d\n\r",key.Uthree[key.Utwo]);
//	    printf("san%d\n\r", (M_INDEX[1]+key.Utwo+1));
//	    printf("si%d\n\r", M_INDEX[M_INDEX[1]+key.Utwo+1]);
//	    printf("wu%d\n\r", FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]);
//	    printf("liu%d\n\r", M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]);
//	    printf("qi%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]); 
	 
//      printf("yi%d\n\r",key.Ptwo);
//	    printf("er%d\n\r",key.Pthree[key.Ptwo]);
//	    printf("san%d\n\r", (M_INDEX[4]+key.Ptwo));
//	    printf("si%d\n\r", M_INDEX[M_INDEX[4]+key.Ptwo]);
//	    printf("wu%d\n\r", FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]);
//	    printf("liu%d\n\r", M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]);
//	    printf("qi%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]]);	 
//	    printf("ba%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]]);
//	    printf("jiu%d\n\r", M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])]);
//	 	printf("shi%d\n\r", M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+INIT_IDX]);

//      printf("yi%d\n\r",key.Utwo);
//	    printf("er%d\n\r",key.Uthree[key.Utwo]);
//	    printf("san%d\n\r", (M_INDEX[1]+key.Utwo+1));
//	    printf("si%d\n\r", M_INDEX[M_INDEX[1]+key.Utwo+1]);
//	    printf("wu%d\n\r", FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]);
//	    printf("liu%d\n\r", M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]);
//	    printf("qi%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]);
	 
//      printf("yi%d\n\r",key.Ptwo);
//	    printf("er%d\n\r",key.Pthree[key.Ptwo]);
//	    printf("san%d\n\r", (M_INDEX[4]+key.Ptwo));
//	    printf("si%d\n\r", M_INDEX[M_INDEX[4]+key.Ptwo]);
//	    printf("wu%d\n\r", FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]);
//	    printf("liu%d\n\r", M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]);
//	    printf("qi%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]]);
//	    printf("ba%d\n\r", M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]]);
//	    printf("jiu%d\n\r", M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])]);
//	    printf("shi%d\n\r", M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+INIT_IDX]);
	 
// printf("shiyi%d\n\r", key.Pset_var);	
// printf("shier%d\n\r", key.Uset_var);	  
// printf("shisav%d\n\r", key.Fset_var);	
// 	printf("P%d\n\r",key.Pvar_bit);	
//	printf("U%d\n\r",key.Uvar_bit);
//	printf("F%d\n\r",key.Fvar_bit);	
