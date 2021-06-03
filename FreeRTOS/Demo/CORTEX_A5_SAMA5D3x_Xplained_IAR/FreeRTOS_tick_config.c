/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Library includes. */
#include "board.h"

/*
 * The FreeRTOS tick handler.  This function must be installed as the handler
 * for the timer used to generate the tick interrupt.  Note that the interrupt
 * generated by the PIT is shared by other system peripherals, so if the PIT is
 * used for Tick generation then FreeRTOS_Tick_Handler() can only be installed
 * directly as the PIT handler if no other system interrupts need to be
 * serviced.  If system interrupts other than the PIT need to be serviced then
 * install System_Handler() as the PIT interrupt handler in place of
 * FreeRTOS_Tick_Handler() and add additional interrupt processing into the
 * implementation of System_Handler().
 */
extern void FreeRTOS_Tick_Handler( void );
static void System_Handler( void );

/*-----------------------------------------------------------*/

static void System_Handler( void )
{
	/* See the comments above the function prototype in this file. */
	FreeRTOS_Tick_Handler();
}
/*-----------------------------------------------------------*/

/*
 * The application must provide a function that configures a peripheral to
 * create the FreeRTOS tick interrupt, then define configSETUP_TICK_INTERRUPT()
 * in FreeRTOSConfig.h to call the function.  This file contains a function
 * that is suitable for use on the Atmel SAMA5.
 */
void vConfigureTickInterrupt( void )
{
	/* NOTE:  The PIT interrupt is cleared by the configCLEAR_TICK_INTERRUPT()
	macro in FreeRTOSConfig.h. */

	/* Enable the PIT clock. */
	PMC->PMC_PCER0 = 1 << ID_PIT;

	/* Initialize the PIT to the desired frequency - specified in uS. */
	PIT_Init( 1000000UL / configTICK_RATE_HZ, BOARD_MCK / 1000000 );

	/* Configure interrupt on PIT.  Note this is on the system interrupt, which
	is shared with other system peripherals, so System_Handler() must be
	installed in place of FreeRTOS_Tick_Handler() if other system handlers are
	required.  The tick must be given the lowest priority (0 in the SAMA5 AIC) */
	IRQ_ConfigureIT( ID_PIT, AIC_SMR_SRCTYPE_EXT_POSITIVE_EDGE, FreeRTOS_Tick_Handler );
	/* See commend directly above IRQ_ConfigureIT( ID_PIT, 0, System_Handler ); */
	IRQ_EnableIT( ID_PIT );
	PIT_EnableIT();

	/* Enable the pit. */
	PIT_Enable();

	/* Prevent compiler warnings in the case where System_Handler() is not used
	as the handler.  See the comments above the System_Handler() function
	prototype at the top of this file. */
	( void ) System_Handler;
}
/*-----------------------------------------------------------*/

