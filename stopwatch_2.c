//#include "tm4c123gh6pm.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "Keypad.h"
//#include "types.h"
//#include "Port_INIT.h"
#include "Systick_Timer.h"
#include "LCD.h"
#include "stopwatch_2.h"
#include "DIO.h"
#include "Stopwatch.h"
#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "Calculator_Base.h"


#define sw1_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)==0x0

#define sw2_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)==0x0


#define sw1_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)!=0x0

#define sw2_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)!=0x0
bool isEnter3 = true ; 

void
Timer1IntHandler(void)
{  

 if ( GPIO_PORTF_MIS_R &0x10)                   //is Sw 1 the interrupt source?
  {

    states_2=0;                                 //change the state to calculator

       
   mode_flag=true;                               // tell the functions taht I am in the states mode
    
  }
  else if  ( GPIO_PORTF_MIS_R &0x01 )           // is SW2 the interrupt source
  {
    states_2=1;

    mode_flag=true;                            // tell the functions taht I am in the states mode
    
  }   
 
 

    //Clear the interrupts flags so that when we accomplish the ISR 
 
GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
MAP_IntDisable(INT_GPIOF);
MAP_IntMasterDisable();



}


void oneShotConfigure(void){
	
	SYSCTL_RCGCTIMER_R |= 0x01;             //activate Timer 0      
    TIMER0_CTL_R = 0x0;                         //diasble timer first 
    TIMER0_CFG_R = 0x4;                         //16 bit mode for timer then we use timer a only
    TIMER0_TAMR_R = 0x11;                       // D4=1 ->  COUNT UP       //D1D0=01 -->one shot 
    TIMER0_TAPR_R |= 0xF4;                      //making the prescale = 244
    TIMER0_TAILR_R = 0xffff;                    //making the Interval Load register =65535 then we have 244*65535~16000000 MHZ = 1 second
}
	
void enableTimer(){
	TIMER0_CTL_R |= 0x03;                   //TASTALL=1  TAEN=1 
	return;
	
}

void disableTimer(){
	TIMER0_CTL_R &= ~(0x01);                // TAEN=0 clearinng the enable bit
	return;
}


bool getOvrFlwFlg(){
	if((TIMER0_RIS_R & 0x1) != 0){          //checking if the timer value has reached time out or not
		TIMER0_ICR_R = 0x1;             // to clear RIS
		return true;
	}
	else
		return false;

}

void displayTime_1(int s, int m, int h){

  LCD_setcursorRowCol(0, 3);
  LCD_printInt(h/10);
  LCD_setcursorRowCol(0, 4);
  LCD_printInt(h%10);
  LCD_setcursorRowCol(0, 5);
  LCD_data(':');
  LCD_setcursorRowCol(0, 6);
  LCD_printInt(m/10);
  LCD_setcursorRowCol(0, 7);
  LCD_printInt(m%10);
  LCD_setcursorRowCol(0, 8);
  LCD_data(':');
  LCD_setcursorRowCol(0, 9);
  LCD_printInt(s/10);
  LCD_setcursorRowCol(0, 10);
  LCD_printInt(s%10);
}

void displaySecFractions(){
	
	// val of current timer with prescaler
	unsigned int Val = TIMER0_TAV_R;
	double x = 0;
	int y = 0;
	
                                                // val / 244(prescaler) * 65535(max value of 16 bit timer)
	x = (Val / 15990540.0);
	LCD_printString(".");
	
                                                // taking only 5 decimal points to display on lcd
	y = (int)(x * 100000);
	LCD_printInt(y);
}

void BlinkLed (void){           
	for(int i = 0; i < 10; ++i) {
    	GPIO_PORTF_DATA_R ^= 0x2;               //alternating the DATA_R bit 1 to light the led
		SysTick_Wait1ms(500);
    }
}


void stopwatch (void)
{

  states_2=2;                                   // set the state to 2 because we are in timer

  mode_flag=false;                              //we are making a function not alternating between modes
 states_1 =2;
   states_1=2;

 //disable and re Enable to make sure no interrupts already running
  MAP_IntDisable(INT_GPIOF);                    
   MAP_IntMasterDisable();

    MAP_IntEnable(INT_GPIOF);

  
  
     IntMasterEnable();
     __asm("CPSIE I");                                           //Enables the processor interrupt.
      GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4) ;           //Enables the specified GPIO interrupts.
       GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0) ;  



 GPIOIntRegister(GPIO_PORTF_BASE,Timer1IntHandler);             //Registers an interrupt handler for a GPIO port




  isEnter3=true;
  
  LCD_command(CLEAR_DISPLAY);
 
  Configure_SysTick();                                           //systick configurations described in the function;
	oneShotConfigure();                                     //decribed above
	// sec ,minute and hour are initially 0
        int sec = 0;
	int min = 0;
	int hour = 0;
	while(1){
          
                //adjust LCD
		LCD_command(CLEAR_DISPLAY);
		displayTime_1(sec, min, hour);
		unsigned int sw1 = GPIO_PORTF_DATA_R & 0x10;            
		unsigned int sw2 = GPIO_PORTF_DATA_R & 0x01;
                  mode_flag=false;              //we are in a function not alternating between modes
               char c=keypad_getkey();          //waiting for the start
               if (c=='f'){                     // returned when a sw  pressed and we are in need to transit between modes
                if (states_2==0){               //Go to calculator
                    loading();
                    calc();
                  }
                else if(states_2==1){           // returned when a sw  pressed and we are in need to transit between modes
                loading();                      //Load and Go to timer
                timer();
                                 }
                            }

		else if(c=='1'){                                      //1 pressed to start
			while(1){	
                          
			
				displayTime_1(sec, min, hour);       //display the tome value in the xx:xx format on LCD
				enableTimer();
                                                                    //flags for reset and stop conditions
				bool stopped = false;                  
                                bool reset_flag=false;
				while(getOvrFlwFlg() == false){         // check the timer has timed out 
					
                                   char k =keypad_getkey_2();           //similar to keypad() but without a (  while(1){}  )
                                   if (k=='f'){
                                    if (states_2==0){

                                      loading();
                                      calc();
                                                    }
                                    else if(states_2==1){
                                      loading();
                                      
                                      timer();
                                                    }
                                   }

                                    
                                    
                                  else if(k == '4'){                    //press 4 to stop
						disableTimer();
						displaySecFractions();  //****** when stopeed the stopwatch the fractions are shown****(not the case while counting)
						stopped = true;         // seting the stopped flag ,Because It will be checked below
						break;
					}
                                  else if( k=='.'){                             //the user wants to reset
                                
                                      disableTimer();                           //disabe the timer to reset 
                                      sec=0;
                                      min=0;
                                      hour=0;
                                      reset_flag=true;
                                      break;
                                      
                                  }
				}
				if(stopped){                            //if the user has pessed 4 then we want to stop here by checking the stoppped flag
					BlinkLed();                     //Blinking the red led an beeping when stoppped
					break;
				}
                                if (reset_flag==true)                   // if the reset "*" is pressed do not count ,just break 
                                {
                                  break;
                                }
                                  // increment the stopwatch 
				if((++sec) == 60) {
					sec = 0;
					if((++min) == 60){
						min = 0;
						hour++;
					}
				}
                  
                                
			}

		}
               
                else if (c=='.'){               // this condtidion is true when reset pressed before start counting 
                  disableTimer();
                  sec=0;
                  min=0;
                  hour=0;
                  
                
                } 
	
        }
}