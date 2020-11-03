#include "2440addr.h"
#include "device_driver.h"
#include "macro.h"

void IRQ_Handler(void)
{
	Uart_Printf("IRQ-Exception!\n");
	for(;;);
}

void FIQ_Handler(void)
{
	Uart_Printf("FIQ-Exception!\n");
	for(;;);
}

void Undef_Handler(void)
{
	Uart_Printf("UND-Exception!\n");
	for(;;);
}

void Pabort_Handler(void)
{
	Uart_Printf("PABT-Exception!\n");
	for(;;);
}

void Dabort_Handler(void)
{
	Uart_Printf("DABT-Exception!\n");
	for(;;);
}

void SWI_Handler(void)
{
	Uart_Printf("SWI-Exception!\n");
	for(;;);
}


