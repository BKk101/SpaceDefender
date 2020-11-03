#include "2440addr.h"
#include "macro.h"

#define COUNT_FOR_CHAT 		2000
#define KEYOUT_ALL_LO() 	Macro_Clear_Area(rGPCDAT, 0x3, 8)
#define KEYOUT_ALL_HI() 	Macro_Set_Area(rGPCDAT, 0x3, 8)
#define SCAN_LINE(line) 	Macro_Write_Block(rGPCCON, 0xf, 0x1<<(line*2), 16)
#define DELAY_FOR_SCAN()	{int i = 1000; while(i--);}

void Key_Poll_Init(void)
{
	Macro_Clear_Area(rGPFCON, 0xff, 8);
	Macro_Clear_Area(rGPFCON, 0x3, 0);
	Macro_Write_Block(rGPCCON, 0xf, 0x5, 16);
	Macro_Set_Area(rGPCDAT, 0x3, 8);
}

int Key_Get_Pressed_Multi(void)
{
	unsigned int i, j, t;
	int key = 0;

	Macro_Write_Block(rGPCCON, 0xf, 0x5, 16);
	KEYOUT_ALL_LO();
	DELAY_FOR_SCAN();

	t=rGPFDAT;

	if((Macro_Extract_Area(t,0xf,4)==0xf) && Macro_Extract_Area(t,1,0))
	{
		return 0;
	}

	for(i=0;i<=1;i++)
	{
		SCAN_LINE(i);
		DELAY_FOR_SCAN();

		t=rGPFDAT&((0xf<<4)|(0x1<<0));

		for(j=0;j<COUNT_FOR_CHAT; j++)
		{
			if(t != (rGPFDAT&((0xf<<4)|(0x1<<0)))) return key;
		}

		if(t==0xf1) continue;
		if((t>>4) != 0xf) key |= ((~t>>4)&0xf)<<(4*i);
		if(!(t&1)) key |= 1<<(9-1);
	}

	return key;
}

void Key_Wait_Key_Released_Multi(void)
{
	while(Key_Get_Pressed_Multi());
}

int Key_Wait_Key_Pressed_Multi(void)
{
	int k;

	while((k=Key_Get_Pressed_Multi())==0);
	return k;
}

int Key_Get_Pressed(void)
{
	unsigned int i, j, t;
	static unsigned char key_code[] = {1,2,0,3,0,0,0,4};

	Macro_Write_Block(rGPCCON, 0xf, 0x5, 16);
	KEYOUT_ALL_LO();
	DELAY_FOR_SCAN();

	t=rGPFDAT;
	
	if((Macro_Extract_Area(t,0xf,4)==0xf) && Macro_Extract_Area(t,1,0))
	{
		return 0;
	}

	for(i=0;i<=1;i++)
	{
		SCAN_LINE(i);
		DELAY_FOR_SCAN();

		t=rGPFDAT&((0xf<<4)|(0x1<<0));

		for(j=0;j<COUNT_FOR_CHAT; j++)
		{
			if(t != (rGPFDAT&((0xf<<4)|(0x1<<0)))) return 0;
		}

		if(t==0xf1) continue;
		if((t>>4) != 0xf) return (key_code[14-(t>>4)])?(key_code[14-(t>>4)]+4*i):(0);
		if(!(t&1)) return 9;
	}

	return 0;
}

void Key_Wait_Key_Released(void)
{
	while(Key_Get_Pressed());
}

int Key_Wait_Key_Pressed(void)
{
	int k;
	
	while((k=Key_Get_Pressed())==0);
	return k;
}
