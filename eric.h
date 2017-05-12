#include <stm32f10x.h>

int x,y,z;
int cacu_times;
int avr_val;
int low_tone[8]={1,262,293,329,349,392,440,494};
int mid_tone[8]={1,523,587,659,698,784,880,988};
int high_tone[8]={1,1046,1175,1318,1397,1568,1760,1967};
/*******************************************************************************/
/* ???  : delay_us()                                                 */
/* ??    : ???????                                                    */
/* ??    : input time_us                                                            */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
void delay_us(int time_us)
{
	TIM3->CR1|=0X01;//ENABLE TIM3
	
	TIM3->CNT=0;
	
	while((TIM3->CNT < time_us)) ;
	
}

/*******************************************************************************/
/* ???  : delay_ms()                                                 */
/* ??    : ???????                                                    */
/* ??    : input time_us                                                            */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
void delay_ms(int time_ms)
{
	for(cacu_times=0;cacu_times<time_ms;cacu_times++) delay_us(1000);
}
/*******************************************************************************/
/* ???  : Set_Dir                                                 */
/* ??    : ???????                                                    */
/* ??    : in portx,port,state                                             */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
#define OUT 1
#define IN 0
void Set_Dir(volatile GPIO_TypeDef* Port_x,int Port_num,int Dir)
{
	if(Dir==IN)
	{
		if(Port_num>7)
		{
			Port_x->CRH&=~(0X0F<<(4*(Port_num-8)));
			Port_x->CRH|=(0x04<<(4*(Port_num-8)));
		}
		else
		{
			Port_x->CRL&=~(0X0F<<(4*Port_num));
			Port_x->CRL|=(0x04<<(4*Port_num));
		}
		
	}
	else if(Dir==OUT)
	{
		if(Port_num>7)
		{
			Port_x->CRH&=~(0X0F<<(4*(Port_num-8)));
			Port_x->CRH|=(0x03<<(4*(Port_num-8)));
		}
		else
		{
			Port_x->CRL&=~(0X0F<<(4*Port_num));
			Port_x->CRL|=(0x03<<(4*Port_num));
		}
		
	}
	
}

/*******************************************************************************/
/* ???  : Set_Pin                                                 */
/* ??    : ???????                                                    */
/* ??    : in portx,port,state                                             */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
int Set_Pin(volatile GPIO_TypeDef* Port_x,int Port_num,int State)
{
	
	if(State==0)
	{
		Port_x->ODR&=~(0x01<<Port_num);
		return (1);
	}
	else if(State==1)
	{
		Port_x->ODR|=(0x01<<Port_num);
		return (1);
	}
	else return (0);
}
/*******************************************************************************/
/* ???  : Set_Pin                                                 */
/* ??    : ???????                                                    */
/* ??    : in portx,port,state                                             */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
int Get_Pin(volatile GPIO_TypeDef* Port_x,int Port_num)
{
	
	return (((Port_x->IDR>>Port_num)&1));
}
///////////////////////PINS SETIINGS 2///////////////////
#define PIN_OUT_PP 0x3
#define PIN_AF_PP 0xB
#define PIN_IN 0x4
#define PIN_IN_WITH_PULL 0x8
#define PIN_IN_AD 0x0
void Set_Pin_Dir(GPIO_TypeDef* GPIOx,int Pin_Num,int Setting)
{
	if(Pin_Num>7)
	{
		GPIOx->CRH&=~((0xf)<<((Pin_Num-8)*4));
		GPIOx->CRH|=(Setting<<((Pin_Num-8)*4));
	}
	else
	{
		GPIOx->CRL&=~((0xf)<<(Pin_Num)*4);
		GPIOx->CRL|=(Setting<<(Pin_Num)*4);
	}
}
u8 Get_Pin_Dir(GPIO_TypeDef* GPIOx,int Pin_Num)
{
	if(Pin_Num>7)
	{
		x=((GPIOx->CRH>>((Pin_Num-8)*4))&0xff);
	}
	else
	{
		x=(GPIOx->CRH>>(Pin_Num*4)&0xff);
	}
	return(x);
}
void Set_Pin_State(GPIO_TypeDef* GPIOx,int Pin_Num,int State)
{
	if(State==1)
	{
		GPIOx->ODR|=(0X01)<<Pin_Num;
	}
	else
	{
		GPIOx->ODR&=~((0X01)<<Pin_Num);
	}
	
}
int Get_Pin_State(GPIO_TypeDef* GPIOx,int Pin_Num)
{
	return(((GPIOx->IDR)>>Pin_Num)&1);
}
////////////////////////adc///////////////////////////////////
void adc1_inject_init()
{
	ADC1->CR2|=0X01;										//ENABLE ADC1
	ADC1->CR2|=(0X01<<2);								//CORRECT ADC1
	while((((ADC1->CR2)>>2)&1)!=0) ;		//WAIT FOR FINISHED
	ADC1->CR2|=(0X7<<12);								//INJECT CONVERT START WITH STARTBIT_INJECT
	ADC1->CR2|=(0X1<<15);								//START WITH OUTSIDE ACTIVE
}
u16 get_adc1(int channel)
{
	ADC1->JSQR=(channel&0XF);
	ADC1->CR2|=(0X01)<<21;
	while((((ADC1->SR)>>3)&1)==0) ;
	ADC1->SR&=~((0X01)<<2);
	while((((ADC1->SR)>>2)&1)==0) ;
	ADC1->SR&=~((0X01)<<2);
	return(ADC1->JDR1);
}
/*******************************************************************************/
/* ???  : TIM3_COUNT_INIT                                                  */
/* ??    : DIV 72 = 1000,000TIMES/SEC                 			                 */
/* ??    : 1US/TIM                                                           */
/* ??    : NEED PLL 9 : 8M * 9 / 2 *2 = 72M                                  */
/* ??    : None                                                              */
/********************************************************************************/
void TIM3_COUNT_INIT(void)
{
	TIM3->CR1=0X00; //RESET
	TIM3->CR1&=~(0X01);		//DISABLE TIM3
	TIM3->CR1&=~(0X01<<4);//DIR_UP
	TIM3->CR1|=0X01<<7; 	//PRELOAD ON TIM3 ENABLED
	
	TIM3->ARR=0XFFFF;//RELOAD AT 65535
	
	TIM3->PSC=64; //DIV 72 = 1000,000TIMES/SEC   
	
	TIM3->CR1|=0X01;//ENABLE TIM3
	
	TIM3->CNT=0;
	
	TIM3->CR1|=0X01;//ENABLE TIM3
}

/*******************************************************************************/
/* ???  : get_pluse()                                                 */
/* ??    : ???????                                                    */
/* ??    : input time_us                                                            */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
int get_pluse(GPIO_TypeDef* GPIOX,int port_num,int port_state)
{
	TIM3->CR1|=0X01;//ENABLE TIM3
	
	TIM3->CNT=0;
	
	while(((GPIOX->IDR>>port_num)&1)==port_state) ;
	
	return(TIM3->CNT);
}
/*******************************************************************************/
/* ???  : get_capacitance()                                                 */
/* ??    : ???????                                                    */
/* ??    : input time_us                                                            */
/* ??    : None                                                              */
/* ??    : None                                                              */
/********************************************************************************/
int Get_Capacitance(GPIO_TypeDef* GPIOX,int port_num)
{
	
	TIM3->CR1|=0X01;//ENABLE TIM3
	avr_val=0;
	
	//for(x=0;x<5;x++)
	//{
		
		Set_Pin_Dir(GPIOX,port_num,PIN_OUT_PP);
		Set_Pin_State(GPIOX,port_num,0);
		delay_us(35);
		
		Set_Pin_Dir(GPIOX,port_num,PIN_IN_WITH_PULL);
		TIM3->CNT=0;
		Set_Pin_State(GPIOX,port_num,1);
//		while(Get_Pin_State(GPIOX,port_num)==0) ;
		while(Get_Pin_State(GPIOX,port_num)==0) ;
		avr_val+=TIM3->CNT;
	//}
	
	//avr_val=avr_val/5;
	return(avr_val);
}

