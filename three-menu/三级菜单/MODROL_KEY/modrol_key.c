#include "modrol_key.h"
#include "GPIO_int.h"
#include "modrol_menu.h"
#include "OLED.h"
#include "Variables.h"
uint8_t                 keyCountTime;
static uint8_t          keyTotolNum = 0;

void Key_Gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = RUN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(RUN_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = STOP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(STOP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = UP_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(UP_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = DOWN_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(DOWN_GPIO_PORT, &GPIO_InitStructure);
	
}


/**
* @brief Read the GPIO state
* @param [in] keys :Key global structure pointer
* @return GPIO status 
*/
uint16_t getKey(keysTypedef_t *keyS)  
{
    uint8_t i = 0; 
    uint16_t readKey = 0;              //Essential init
    
    //GPIO Cyclic scan
    for(i = 0; i < keys.keyTotolNum; i++)
    {
        if(!GPIO_ReadInputDataBit((GPIO_TypeDef*)keyS->singleKey[i].keyPort,keyS->singleKey[i].keyGpio))
        {
			
            G_SET_BIT(readKey, keyS->singleKey[i].keyNum);
        }
    }
    
    return readKey;
}


/**
* @brief Read the KEY value
* @param [in] keys :Key global structure pointer
* @return GPIO status
*/
uint16_t readKeyValue(keysTypedef_t *keyS)
{
    static uint8_t keyCheck = 0;
    static uint8_t keyState = 0;
    static uint16_t keyLongCheck = 0;
    static uint16_t keyPrev = 0;      //last key

    uint16_t keyPress = 0;
    uint16_t keyReturn = 0;
    
    keyCountTime ++;
    
    if(keyCountTime >= (DEBOUNCE_TIME / KEY_TIMER_MS))          //keyCountTime 1MS+1  key eliminate jitter 10MS
    {
        keyCountTime = 0;
        keyCheck = 1;
    }
    if(1 == keyCheck)
    {
        keyCheck = 0;
        keyPress = getKey(keyS);
        switch (keyState)
        {
            case 0:
                if(keyPress != 0)
                {
                    keyPrev = keyPress;
                    keyState = 1;
                }
                break;
                
            case 1:
                if(keyPress == keyPrev)
                {
                    keyState = 2;
                    keyReturn= keyPrev | KEY_DOWN;
                }
                else                //Button lift, jitter, no response button
                {
                    keyState = 0;
                }
                break;
                
            case 2:

                if(keyPress != keyPrev)
                {
                    keyState = 0;
                    keyLongCheck = 0;
                    keyReturn = keyPrev | KEY_UP;
                    return keyReturn;
                }
                if(keyPress == keyPrev)
                {
                    keyLongCheck++;
                    if(keyLongCheck >= (PRESS_LONG_TIME / DEBOUNCE_TIME))    //Long press 3S effective
                    {
                        keyLongCheck = 0;
                        keyState = 3;
                        keyReturn= keyPress | KEY_LONG;
                        return keyReturn;
                    }
                }
                break;

            case 3:
                if(keyPress != keyPrev)
                {
                    keyState = 0;
                }
                break;
        }
    }
    return  NO_KEY;
}

/**
* @brief Key call-back function

* Detects the keys state and call the corresponding callback function
* @param [in] keys :Key global structure pointer
* @return none
*/
void keyHandle(keysTypedef_t *keyS)
{
    uint8_t i = 0;
    uint16_t key_value = 0;

    key_value = readKeyValue(keyS);

    if(!key_value) return;
    
    //Check short press button
    if(key_value & KEY_UP)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum)) 
            {
                //key callback function of short press
                if(keyS->singleKey[i].shortPress) 
                {
                    keyS->singleKey[i].shortPress(); 

                }
            }
        }
    }

    //Check short long button
    if(key_value & KEY_LONG)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum))
            {
                //key callback function of long press
                if(keyS->singleKey[i].longPress) 
                {
                    keyS->singleKey[i].longPress(); 

                }
            }
        }
    }
} 

/**
* @brief key init function

* 
* @param [in] keyRccPeriph APB2_peripheral
* @param [in] keyPort Peripheral_declaration
* @param [in] keyGpio GPIO_pins_define 
* @param [in] short_press :Short press state callback function address
* @param [in] long_press :Long press state callback function address
* @return key structure pointer
*/
keyTypedef_t keyInitOne(uint32_t keyRccPeriph, GPIO_TypeDef * keyPort, uint32_t keyGpio, gokitKeyFunction shortPress, gokitKeyFunction longPress)
{
    static int8_t key_total = -1;

    keyTypedef_t singleKey;
    
    //Platform-defined GPIO
    singleKey.keyRccPeriph = keyRccPeriph;
    singleKey.keyPort = keyPort;
    singleKey.keyGpio = keyGpio;
    singleKey.keyNum = ++key_total;
    
    //Button trigger callback type
    singleKey.longPress = longPress;
    singleKey.shortPress = shortPress;
    
    keyTotolNum++;
    
    return singleKey;
}

/**
* @brief key parameter init function
* Keys GPIO init,start timer detect keys state
* @param [in] keys ::Key global structure pointer
* @return none
*/
void keyParaInit(keysTypedef_t *keys)
{
//     uint8_t temI = 0; 
    
    if(NULL == keys)
    {
        return ;
    }
    
    keys->keyTotolNum = keyTotolNum;
    
    //Limit on the number keys (Allowable number: 0~12)
    if(KEY_MAX_NUMBER < keys->keyTotolNum) 
    {
        keys->keyTotolNum = KEY_MAX_NUMBER; 
    }
}
G_Key key;
/**
* key1 short press handle
* @param none
* @return none
*/
void key1ShortPress(void)  //按键1短按
{

     struct DataFields Presult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+ATTR_IDX]);  
     struct DataFields Uresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+ATTR_IDX]);      
     struct DataFields Fresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+ATTR_IDX]); 
	  key.state = RUN_short;
  	
        key.level=LEVEL5;
			switch(key.page)	 //按下保存		
		{
			case P_page:
				if(Presult.write_restriction != 3)
				{				
				  key.flag ++;		
                  if(key.flag>2&& key.Pset_var!= key.init_idx)
				  {
				  
//				   M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+INIT_IDX] = key.Pset_var;
				  
				  }
					 
				}
                else key.flag = 0;
			break;
			   
			case U_page:
					if(Uresult.write_restriction != 3)
				{				
				  key.flag ++;				
				}
                else key.flag = 0;
			break;
			
			case OP_page:

			break;
			
			case F_page:
				if(Fresult.write_restriction != 3)
				{				
				  key.flag ++;				
				}
                else key.flag = 0;
			break;		
		}	
//	    printf("yi%d\n\r",key.level);
//		printf("er%d\n\r",key.flag);
	
}

/**
* key1 long press handle
* @param none
* @return none
*/

void key1LongPress(void)  //按键1长按
{

	key.state = RUN_long;
	 
}

/**
* key2 short press handle
* @param none
* @return none
*/
void key2ShortPress(void)  //按键2短按
{

	key.state = STOP_short;
    key.page = (key.page + 1) % 4; 
	key.level = LEVEL1;
	key.flag = 0;
	printf("yi%d\n\r",key.level);
	printf("er%d\n\r",key.flag);
	key.Pvar_bit=0;
	key.Fvar_bit=0;
	key.Uvar_bit=0;
}

/**
* key2 long press handle
* @param none
* @return none
*/
void key2LongPress(void)  //按键2长按
{
     struct DataFields Presult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+ATTR_IDX]);  
     struct DataFields Uresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+ATTR_IDX]);      
     struct DataFields Fresult = parse_data(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+ATTR_IDX]);

	key.state = STOP_long;
	if( key.level<4) { key.level = (key.level + 1) % 4; }
	
	if(key.level ==4 && key.flag >1)
	{
		if(key.page == P_page)
		{
								
			if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]>9999)
			{											
				 key.Pvar_bit = (key.Pvar_bit + 1) % 5; 

				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]>999 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]<=9999)
			{											
				 key.Pvar_bit = (key.Pvar_bit + 1) % 4; 
				
			}

			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]>99 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]<=999)
			{											
				 key.Pvar_bit = (key.Pvar_bit + 1) % 3; 
				
			}
			
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]>9 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]<=99)
			{											
				 key.Pvar_bit = (key.Pvar_bit + 1) % 2; 
				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]>=0 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX]<=9)
			{											
				 key.Pvar_bit = (key.Pvar_bit + 1) % 1; 
				
			}						

		}
	   else if (key.page == U_page)
		{
											
			if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]>9999)
			{											
				 key.Uvar_bit = (key.Uvar_bit + 1) % 5; 
				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]>999 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]<=9999)
			{											
				 key.Uvar_bit = (key.Uvar_bit + 1) % 4; 
				
			}

			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]>99 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]<=999)
			{											
				 key.Uvar_bit = (key.Uvar_bit + 1) % 3; 
				
			}
			
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]>9 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]<=99)
			{											
				 key.Uvar_bit = (key.Uvar_bit + 1) % 2; 
				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]>=0 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX]<=9)
			{											
				 key.Uvar_bit = (key.Uvar_bit + 1) % 1; 
				
			}	
		}
	   else if (key.page == OP_page)
	    {
               key.OPvar_bit = (key.OPvar_bit + 1) % 4;
	    }
	   
	   else if (key.page == F_page)
	    {	
		       	if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]>9999)
			{											
				 key.Fvar_bit = (key.Fvar_bit + 1) % 5; 
				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]>999 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]<=9999)
			{											
				 key.Fvar_bit = (key.Fvar_bit + 1) % 4; 
				
			}

			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]>99 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]<=999)
			{											
				 key.Fvar_bit = (key.Fvar_bit + 1) % 3; 
				
			}
			
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]>9 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]<=99)
			{											
				 key.Fvar_bit = (key.Fvar_bit + 1) % 2; 
				
			}
			
			else if(M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]>=0 && M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX]<=9)
			{											
				 key.Fvar_bit = (key.Fvar_bit + 1) % 1; 
				
			}	
	    }
	     
	
	
	}
	

}
/**
* key1 short press handle
* @param none
* @return none
*/
void key3ShortPress(void)  //按键3短按 //加
{

	key.state = UP_short;

		
	if(key.level == LEVEL2) 
	{	  
		switch(key.page)			
		{
			case P_page:
				key.Ptwo++;
				if(key.Ptwo < 1) key.Ptwo = M_INDEX[M_INDEX[4]];	
				else if(key.Ptwo > M_INDEX[M_INDEX[4]]) key.Ptwo = 1;					
			break;
			   
			case U_page:
				 key.Utwo++;
			     if(key.Utwo <=0)  key.Utwo = M_INDEX[M_INDEX[1]]-1;
				 else if(key.Utwo > M_INDEX[M_INDEX[1]]-1) key.Utwo = 0;	
			break;
			
			case OP_page:
				 key.OPtwo++;
			     if(key.OPtwo < 1) key.OPtwo = 9;
				 else if(key.OPtwo > 9) key.OPtwo = 1;	
			break;
			
			case F_page:
				 key.Ftwo++;
			     if(key.Ftwo < 0) key.Ftwo = M_INDEX[M_INDEX[3]]-1;	
				else if(key.Ftwo > M_INDEX[M_INDEX[3]]-1) key.Ftwo = 0;	
			break;		
		}			  
	 }
	
	 else if(key.level == LEVEL3)
	{	  
		switch(key.page)			
		{
			case P_page:			 
			
			
			break;
			   
			case U_page:
			 	 key.Uthree[key.Utwo]+=10;
			     if(key.Uthree[key.Utwo]/10%10 < 0) key.Uthree[key.Utwo] = (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]/10%10)+key.Uthree[key.Utwo]%10;
                 else if(key.Uthree[key.Utwo]/10%10 > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]/10%10)) key.Uthree[key.Utwo] = key.Uthree[key.Utwo]%10;	
			    	
			break;
			
			case F_page:
                 key.Fthree[key.Ftwo]+=10;
				if(key.Fthree[key.Ftwo]/10%10 < 0) key.Fthree[key.Ftwo] = (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]]/10%10)+key.Fthree[key.Ftwo]%10;
                 else if(key.Fthree[key.Ftwo]/10%10 > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]]/10%10)) key.Fthree[key.Ftwo] = key.Fthree[key.Ftwo]%10;	
                 
			break;		
		}			  
	 }

	 else if(key.level == LEVEL4)
	{	  
		switch(key.page)			
		{
			case P_page:			 
			     key.Pthree[key.Ptwo]++;
                 if(key.Pthree[key.Ptwo] < 1) key.Pthree[key.Ptwo] =  M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]];			
			    else if(key.Pthree[key.Ptwo] > M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]]) key.Pthree[key.Ptwo] = 1;	
				
			
			break;
			   
			case U_page:
			 	 key.Uthree[key.Utwo]++;
		            if(key.Uthree[key.Utwo] < 1) key.Uthree[key.Utwo] = M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]];
                 else if(key.Uthree[key.Utwo] > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]])) key.Uthree[key.Utwo] = 0;	
			    	
			break;
			
			case F_page:
                 key.Fthree[key.Ftwo]++;
	             if(key.Fthree[key.Ftwo] < 1) key.Fthree[key.Ftwo] = M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]];
                 else if(key.Fthree[key.Ftwo] > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]])) key.Fthree[key.Ftwo] = 0;	
			break;		
		}			  
	 }	 

	 else if(key.level == LEVEL5 && key.flag >1)
	{	  
		switch(key.page)			
		{
			case P_page:			 
                         if(key.Pvar_bit==4)	 { key.Pset_var +=10000;   }
					else if(key.Pvar_bit==3)	 { key.Pset_var +=1000;   }
					else if(key.Pvar_bit==2)	 { key.Pset_var +=100;   }
					else if(key.Pvar_bit==1)	 { key.Pset_var +=10;   }
					else if(key.Pvar_bit==0)	 { key.Pset_var +=1;   }	
					if(key.Pset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX])
					{
					 key.Pset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX];					
					}
					
					else if(key.Pset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MIN_IDX])
					{
					 key.Pset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MIN_IDX];					
					}
			break;
			   
			case U_page:
		                 if(key.Uvar_bit==4)	 { key.Uset_var +=10000;   }
					else if(key.Uvar_bit==3)	 { key.Uset_var +=1000;   }
					else if(key.Uvar_bit==2)	 { key.Uset_var +=100;   }
					else if(key.Uvar_bit==1)	 { key.Uset_var +=10;   }
					else if(key.Uvar_bit==0)	 { key.Uset_var +=1;   }		
			    	 if(key.Uset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX])
					{
					 key.Uset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX];					
					}
					
					else if(key.Uset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MIN_IDX])
					{
					 key.Uset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MIN_IDX];					
					}
			break;
			
			case F_page:
                         if(key.Fvar_bit==4)	 { key.Fset_var +=10000;   }
					else if(key.Fvar_bit==3)	 { key.Fset_var +=1000;   }
					else if(key.Fvar_bit==2)	 { key.Fset_var +=100;   }
					else if(key.Fvar_bit==1)	 { key.Fset_var +=10;   }
					else if(key.Fvar_bit==0)	 { key.Fset_var +=1;   }
			    	 if(key.Fset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX])
					{
					 key.Fset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX];					
					}
					
					else if(key.Fset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MIN_IDX])
					{
					 key.Fset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MIN_IDX];					
					}					
			break;		
		}			  
	 }	 
	 

}

/**
* key1 long press handle
* @param none
* @return none
*/
void key3LongPress(void)  //按键3长按
{

	key.state = UP_long;
	



}

/**
* key2 short press handle
* @param none
* @return none
*/
void key4ShortPress(void)  //按键4短按  //减
{

	key.state = DOWN_short;

	
	if(key.level == LEVEL2)
	{	  
		switch(key.page)			
		{
			case P_page:
				key.Ptwo--;
				if(key.Ptwo < 1) key.Ptwo = M_INDEX[M_INDEX[4]];	
				else if(key.Ptwo > M_INDEX[M_INDEX[4]]) key.Ptwo = 1;					
			break;
			   
			case U_page:
				 key.Utwo--;
				 if(key.Utwo <0) key.Utwo = M_INDEX[M_INDEX[1]]-1;
				 else if(key.Utwo > M_INDEX[M_INDEX[1]]-1) key.Utwo = 0;		
			break;
			
			case OP_page:
				 key.OPtwo--;
				   if(key.OPtwo < 1) key.OPtwo = 9;
				 else if(key.OPtwo > 9) key.OPtwo = 1;	
			break;
			
			case F_page:
				 key.Ftwo--;
				if(key.Ftwo < 0) key.Ftwo = M_INDEX[M_INDEX[3]]-1;	
				else if(key.Ftwo > M_INDEX[M_INDEX[3]]-1) key.Ftwo = 0;	
			break;		
		}			  
	 }
	
		 else if(key.level == LEVEL3)
	{	  
		switch(key.page)			
		{
			case P_page:			 
			
			break;
			   
			case U_page:
				 key.Uthree[key.Utwo]-=10;
     			     if(key.Uthree[key.Utwo]/10%10 < 0) key.Uthree[key.Utwo] = (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]/10%10)+key.Uthree[key.Utwo]%10;
                 else if(key.Uthree[key.Utwo]/10%10 > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]]/10%10)) key.Uthree[key.Utwo] = key.Uthree[key.Utwo]%10;	
			break;
			
			case F_page:
                 key.Fthree[key.Ftwo]-=10;
			     	 if(key.Fthree[key.Ftwo]/10%10 < 0) key.Fthree[key.Ftwo] = (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]]/10%10)+key.Fthree[key.Ftwo]%10;
                 else if(key.Fthree[key.Ftwo]/10%10 > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]]/10%10)) key.Fthree[key.Ftwo] = key.Fthree[key.Ftwo]%10;	
			break;		
		}			  
	 } 

		 else if(key.level == LEVEL4)
	{	  
		switch(key.page)			
		{
			case P_page:			 
			     key.Pthree[key.Ptwo]--;	
			      if(key.Pthree[key.Ptwo] < 1) key.Pthree[key.Ptwo] =  M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]];			
			    else if(key.Pthree[key.Ptwo] > M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]]) key.Pthree[key.Ptwo] = 1;	
			
			break;
			   
			case U_page:
				 key.Uthree[key.Utwo]--;
                  if(key.Uthree[key.Utwo] < 1) key.Uthree[key.Utwo] = M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo]]];
                 else if(key.Uthree[key.Utwo] > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]])) key.Uthree[key.Utwo] = 0;	
			break;
			
			case F_page:
				key.Fthree[key.Ftwo]--;
          	      if(key.Fthree[key.Ftwo] < 1) key.Fthree[key.Ftwo] = M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo]]];
                 else if(key.Fthree[key.Ftwo] > (M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]])) key.Fthree[key.Ftwo] = 0;	
			break;		
		}			  
	 }

	 else if(key.level == LEVEL5 && key.flag >1)
	{	  
		switch(key.page)			
		{
			case P_page:			 
                         if(key.Pvar_bit==4)	 { key.Pset_var -=10000;   }
					else if(key.Pvar_bit==3)	 { key.Pset_var -=1000;   }
					else if(key.Pvar_bit==2)	 { key.Pset_var -=100;   }
					else if(key.Pvar_bit==1)	 { key.Pset_var -=10;   }
					else if(key.Pvar_bit==0)	 { key.Pset_var -=1;   }	
					if(key.Pset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX])
					{
					 key.Pset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MAX_IDX];					
					}
					
					else if(key.Pset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MIN_IDX])
					{
					 key.Pset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[4]+key.Ptwo]]+key.Pthree[key.Ptwo]])+MIN_IDX];					
					}
			break;
			   
			case U_page:
		                 if(key.Uvar_bit==4)	 { key.Uset_var -=10000;   }
					else if(key.Uvar_bit==3)	 { key.Uset_var -=1000;   }
					else if(key.Uvar_bit==2)	 { key.Uset_var -=100;   }
					else if(key.Uvar_bit==1)	 { key.Uset_var -=10;   }
					else if(key.Uvar_bit==0)	 { key.Uset_var -=1;   }		
			    	 if(key.Uset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX])
					{
					 key.Uset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MAX_IDX];					
					}
					
					else if(key.Uset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MIN_IDX])
					{
					 key.Uset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[1]+key.Utwo+1]]+key.Uthree[key.Utwo]])+MIN_IDX];					
					}
			break;
			
			case F_page:
                         if(key.Fvar_bit==4)	 { key.Fset_var -=10000;   }
					else if(key.Fvar_bit==3)	 { key.Fset_var -=1000;   }
					else if(key.Fvar_bit==2)	 { key.Fset_var -=100;   }
					else if(key.Fvar_bit==1)	 { key.Fset_var -=10;   }
					else if(key.Fvar_bit==0)	 { key.Fset_var -=1;   }
			    	 if(key.Fset_var>M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX])
					{
					 key.Fset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MAX_IDX];					
					}
					
					else if(key.Fset_var<M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MIN_IDX])
					{
					 key.Fset_var = M_FORMAT[ FORMAT_COL*(M_INDEX[M_FORMAT[FORMAT_COL*M_INDEX[M_INDEX[3]+key.Ftwo+1]]+key.Fthree[key.Ftwo]])+MIN_IDX];					
					}					
			break;		
		}			  
	 }	 


	 	 
}

/**
* key2 long press handle
* @param none
* @return none
*/
void key4LongPress(void) //按键4长按
{

	key.state = DOWN_long;

}


#define GPIO_KEY_NUM 4                ///< Defines the total number of key member
keyTypedef_t singleKey[GPIO_KEY_NUM]; ///< Defines a single key member array pointer
keysTypedef_t keys;

/**
* Key init function
* @param none
* @return none
*/
void keyInit(void)
{
    singleKey[0] = keyInitOne(NULL, RUN_GPIO_PORT, RUN_GPIO_PIN, key1ShortPress, key1LongPress);
    singleKey[1] = keyInitOne(NULL, STOP_GPIO_PORT, STOP_GPIO_PIN, key2ShortPress, key2LongPress);
    singleKey[2] = keyInitOne(NULL, UP_GPIO_PORT, UP_GPIO_PIN, key3ShortPress, key3LongPress);
    singleKey[3] = keyInitOne(NULL, DOWN_GPIO_PORT, DOWN_GPIO_PIN, key4ShortPress, key4LongPress);
	
    keys.singleKey = (keyTypedef_t *)&singleKey;
    keyParaInit(&keys); 
}











