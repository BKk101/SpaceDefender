#include "2440addr.h"
#include "macro.h"
#include "option.h"
#include "device_driver.h"

void Timer0_Init(void)
{
	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 0);
}

void Timer0_Repeat(int time)
{
	rTCNTB0 = time * 50;
	rSRCPND = BIT_TIMER0;

	Macro_Write_Block(rTCON,0xF, 0x2, 0);
	Macro_Write_Block(rTCON,0xF, 0x9, 0);
}

int Timer0_Check_Expired(void)
{
	if(Macro_Check_Bit_Set(rSRCPND, 10))
	{
		rSRCPND = BIT_TIMER0;
		return 1;
	}

	return 0;
}

void Timer0_Stop(void)
{
	Macro_Clear_Bit(rTCON, 0);
}

void Timer0_Change_Delay(int msec)
{
	rTCNTB0 = msec * 50;
	rTCMPB0 = msec * 50;
}

void Timer0_ISR_Enable(int en)
{
	if(en) 
	{
		Macro_Clear_Bit(rINTMSK, TIMER0);
		Macro_Set_Bit(rTCON, 0);
	}
	else
	{
		Macro_Set_Bit(rINTMSK, TIMER0);
		Macro_Clear_Bit(rTCON, 0);
	}
}

#define TIMER1_MAX	0xffff
#define TIMER1_TICK	20

void Timer1_Stopwatch_Run(void)
{
	rTCNTB1 = TIMER1_MAX;

	Macro_Write_Block(rTCFG0, 0xff, PCLK/(50000 * 8)-1, 0);
	Macro_Write_Block(rTCFG1, 0xf, 2, 4);
	Macro_Write_Block(rTCON,0x3, 2, 8);
	Macro_Write_Block(rTCON,0x3, 1, 8);
}						

int Timer1_Stopwatch_Stop(void)
{
	Macro_Clear_Bit(rTCON, 8);
	return (TIMER1_MAX - rTCNTO1) * TIMER1_TICK;
}

void Timer4_Init(void)
{
	Macro_Write_Block(rTCFG0, 0xff, 0, 8);
	Macro_Write_Block(rTCFG1, 0xf, 3, 16);
	Macro_Clear_Area(rTCFG1, 0xf, 20);
}

void Timer4_Delay(unsigned int msec)
{
	int i;
	unsigned int temp = msec * ((PCLK/1000)/16); 
	
	for(i = 0; i<(temp/0x10000); i++)
	{
		rTCNTB4 = 0xffff;

		Macro_Write_Block(rTCON, 0x7, 0x2, 20);
		Macro_Write_Block(rTCON, 0x7, 0x1, 20);
		while(Macro_Check_Bit_Clear(rSRCPND, 14));
		rSRCPND = BIT_TIMER4;
	}
	
	rTCNTB4 = temp%0x10000;

	Macro_Write_Block(rTCON, 0x7, 0x2, 20);
	Macro_Write_Block(rTCON, 0x7, 0x1, 20);
	while(Macro_Check_Bit_Clear(rSRCPND, 14));
	rSRCPND = BIT_TIMER4;
	
	Macro_Clear_Bit(rTCON, 20);
}
