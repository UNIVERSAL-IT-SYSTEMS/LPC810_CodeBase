/**************************************************************************/
/*!
    @file     main.c

    @section LICENSE

    Software License Agreement (BSD License)

    original version:
    Copyright (c) 2013, K. Townsend (microBuilder.eu)

    some basic Arduino functions introduced
    Copyright (c) 2014, ChrisMicro (https://github.com/ChrisMicro)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/
#include <stdio.h>
#include "LPC8xx.h"
#include "gpio.h"
#include "mrt.h"
#include "uart.h"

#if defined(__CODE_RED)
  #include <cr_section_macros.h>
  #include <NXP/crp.h>
  __CRP const unsigned int CRP_WORD = CRP_NO_CRP ;
#endif

#define LED_LOCATION    (2)

/* This define should be enabled if you want to      */
/* maintain an SWD/debug connection to the LPC810,   */
/* but it will prevent you from having access to the */
/* LED on the LPC810 Mini Board, which is on the     */
/* SWDIO pin (PIO0_2).                               */
// #define USE_SWD

void configurePins()
{
  /* Enable SWM clock */
	//  LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);  // this is already done in SystemInit()

  /* Pin Assign 8 bit Configuration */
  /* U0_TXD */
  /* U0_RXD */
  LPC_SWM->PINASSIGN0 = 0xffff0004UL;

  /* Pin Assign 1 bit Configuration */
  #if !defined(USE_SWD)
    /* Pin setup generated via Switch Matrix Tool
       ------------------------------------------------
       PIO0_5 = RESET
       PIO0_4 = U0_TXD
       PIO0_3 = GPIO            - Disables SWDCLK
       PIO0_2 = GPIO (User LED) - Disables SWDIO
       PIO0_1 = GPIO
       PIO0_0 = U0_RXD
       ------------------------------------------------
       NOTE: SWD is disabled to free GPIO pins!
       ------------------------------------------------ */
    LPC_SWM->PINENABLE0 = 0xffffffbfUL;
  #else
    /* Pin setup generated via Switch Matrix Tool
       ------------------------------------------------
       PIO0_5 = RESET
       PIO0_4 = U0_TXD
       PIO0_3 = SWDCLK
       PIO0_2 = SWDIO
       PIO0_1 = GPIO
       PIO0_0 = U0_RXD
       ------------------------------------------------
       NOTE: LED on PIO0_2 unavailable due to SWDIO!
       ------------------------------------------------ */
    LPC_SWM->PINENABLE0 = 0xffffffb3UL;   
  #endif  
}

#define OUTPUT 1
#define LOW 0
#define HIGH 1

void pinMode(uint8_t pin, uint8_t mode)
{
	// check if it is Arduino Uno LED pin
	if(pin==13)
	{
	    if(mode==OUTPUT) LPC_GPIO_PORT->DIR0 |= (1 << LED_LOCATION);
	}
}
void digitalWrite(uint8_t pin, uint8_t value)
{
	// check if it is Arduino uno LED pin
	if(pin==13)
	{
		if(value==1) LPC_GPIO_PORT->SET0 = 1 << LED_LOCATION;
		else LPC_GPIO_PORT->CLR0 = 1 << LED_LOCATION;
	}
}

void delay(uint32_t milliSeconds)
{
	mrtDelay(milliSeconds);
}

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(500);              // wait for a second
  digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
  delay(500);              // wait for a second
}

int main(void)
{
  /* Initialise the GPIO block */
  gpioInit();

  /* Initialise the UART0 block for printf output */
  uart0Init(115200);

  /* Configure the multi-rate timer for 1ms ticks */
  mrtInit(__SYSTEM_CLOCK/1000);

  /* Configure the switch matrix (setup pins for UART0 and GPIO) */
  configurePins();

  /* Set the LED pin to output (1 = output, 0 = input) */
  #if !defined(USE_SWD)
    LPC_GPIO_PORT->DIR0 |= (1 << LED_LOCATION);
  #endif

  setup();
  while(1) loop();
  /*
  while(1)
  {
	  int n;
    #if !defined(USE_SWD)
      // Turn LED Off by setting the GPIO pin high
	  for(n=0;n<100;n++)
	  {
		  LPC_GPIO_PORT->SET0 = 1 << LED_LOCATION;
		  mrtDelay(1);
		  LPC_GPIO_PORT->CLR0 = 1 << LED_LOCATION;
		  mrtDelay(1);
	  }

      //Turn LED Off by setting the GPIO pin high
      LPC_GPIO_PORT->SET0 = 1 << LED_LOCATION;
      mrtDelay(2000);
    #else
      // Just insert a 1 second delay
      mrtDelay(1000);
    #endif

    // Send some text (printf is redirected to UART0)
    printf("LPC810 reprogrammed \n\r");
  }*/
}
