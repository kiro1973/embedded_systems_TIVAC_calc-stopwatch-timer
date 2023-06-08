#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "Systick_Timer.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "tm4c123gh6pm.h"
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
#include "LCD.h"
#include "delay.h"
#include "keypad.h"
#include "Calculator_Base.h"


int flag=0;                                              //timer flag is 0 since we do no want it to start its th interrupt until all the timer digits are filled
#define counter (*((volatile int *)0x2000774C))
int m , s ;


void start_timer(void){                                  // starting the timer on the LCD
  LCD_command(CURSOR_START_2ND_LINE);
  LCD_printString("00:00");
}
void
Timer(void)                                            // ISR for the push button wo switch between modes
{  
if ( GPIO_PORTF_MIS_R &0x10 )                        //if enters interrupt because of SW1 (PF4)
  {

    states_2=2;                                       //change the state so we will be in the stopwatch state
   
       
   mode_flag=true;                                 //to let the keypad() know if we need a keypad input or just return because we want to switch modes
    counter =0;                                   // to switch from the timer and clear the value
        MAP_IntDisable(INT_TIMER1A);              // to prevent the timer from interrupting other functions because we will switch the mode
  }
  else if  ( GPIO_PORTF_MIS_R &0x01 )
  {
    states_2=0;                                 //change the state so we will be in the stopwatch state

       
    mode_flag=true;                           //to let the keypad() know if we need a keypad input or just return because we want to switch modes
    counter=0;                                // to switch from the timer and clear the value
    MAP_IntDisable(INT_TIMER1A);              // to prevent the timer from interrupting other functions because we will switch the mode
    
  }   
  
  
  
  
  GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);            //clear the interrupt pin
   GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);           //clear the interrupt pin
    MAP_IntDisable(INT_GPIOF);
  // MAP_IntMasterDisable();
   
}

void TimerReal(void)
{
  if (counter != 0){
      
    --counter;                                            //decrement the timer
    LCD_command(CLEAR_DISPLAY);                           //the following lines are to display the timer in the format 00:00
       LCD_setcursorRowCol(0,5); 
    LCD_printInt((counter/60)/10);
    LCD_setcursorRowCol(0,6); 
    LCD_printInt((counter/60)%10);
    LCD_setcursorRowCol(0,7); 
    LCD_printString(":");
    LCD_setcursorRowCol(0,8); 
    LCD_printInt((counter-(counter/60)*60)/10);
    LCD_setcursorRowCol(0,9); 
    LCD_printInt((counter-(counter/60)*60)%10);
    printf("%d \n", counter);
  }
  if (counter ==0){
    flag=0;


for(int i = 0; i < 10; ++i) {
    	GPIO_PORTF_DATA_R ^= 0x2;
		delay_ms(500);
    }
                  }
  // Clear the timer interrupt.
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT); // clear the interrupt flag

   
      
}

void Timer0_Init(void){
    // Enable the timer peripheral.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);  // Enable the COMP module.                                
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);   //configures the operating mode of the timer(s). The timer module is disabled before being configured and is left in the disabled state.                                 
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()); //This function configures the timer load value; if the timer is running then the value is immediately loaded into the timer.   
    TimerIntRegister(TIMER1_BASE, TIMER_A,TimerReal);//This function registers the handler to be called when a timer interrupt occurs
    IntEnable(INT_TIMER1A);              //This function enables the indicated timer interrupt sources
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);// Enable the timer 
    IntPrioritySet(INT_TIMER1A,4);

    
    //Enable global interrupts
     IntMasterEnable();

}


void timer(){
  
  

mode_flag=false;                // to let the other functions that enters that I am in function not switch for interrupt mode
  states_2 =1;                  //I am in the timer state
  states_1=1;                   //I am in the timer state
  
  
      MAP_IntEnable(INT_GPIOF);                 // enable the interrupt to portf from NVIC
     IntMasterEnable();                         //This function allows the processor to respond to interrupts.
  __asm("CPSIE I");                             // processor status interrupt enable
         GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4) ;          //Enables the specified GPIO interrupts.
       GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0) ;          
IntPrioritySet(INT_GPIOF , 0) ;                                   //Sets the priority of an interrupt.
 GPIOIntRegister(GPIO_PORTF_BASE,Timer);                         //ensures that the interrupt handler specified by pfnIntHandler is called when an interrupt is detected from the selected GPIO port.
         

 Timer0_Init();                                        //intialize the timer

 while(1){
   int m1 ,m2,s1,s2 ;                                   //digit 1 for minute ,digit 0for minute,digit 1 for seconds ,digit 0for seconds
   
   if (switch_to_timer_flag ==true){                    //check if we have to cahnge the mdoe
      if (states_2==0){                                 //change state,loading screen ,go to calc
                    loading();
                   calc();
                  }
  else if(states_2==2){
  loading();
  stopwatch();
  }
         }
   
   
   while(flag ==0){                                            //the user have not yet have not filled the 4 values xx:xx
      start_timer();                                            
      
        if (switch_to_timer_flag ==true){                    //we know if the mode have changed
      if (states_2==0){
                    loading();
                   calc();
                  }
  else if(states_2==2){
  loading();
  stopwatch();
  }
         } 


      while (keypad_getkey()==0);              // wait while ther is no a keypad input
             
      
       m1 = keypad_getkey() - 48;             //ASCII Compensation 
 //adjust the screen
      LCD_setcursorRowCol(1, 0);
      LCD_printInt(m1);
      delay_ms(250);
      
      while (keypad_getkey()==0);            // wait while ther is no a keypad input   
       m2 = keypad_getkey() - 48;            //ASCII Compensation
      //adjust the screen
       LCD_setcursorRowCol(1, 1);                
      LCD_printInt(m2);
      m = m1*10+m2;                        //put each digit in its mathematical place;
      delay_ms(250);
      
      while (keypad_getkey()==0);       //waiting for the seconds input
       s1 = keypad_getkey()- 48;
      LCD_setcursorRowCol(1, 3);
      LCD_printInt(s1);
      delay_ms(250);
      
      while (keypad_getkey()==0); 
       s2 = keypad_getkey() - 48;
      LCD_setcursorRowCol(1, 4);
      LCD_printInt(s2);
      s = s1*10+s2;
      
      flag =1;
      
      }
      if(flag == 1)
      {
      while (keypad_getkey()==0); 
      int key = keypad_getkey();
      
      if (key == '='){
  //      counter = m*60 + s ; 
      counter=((m1*10+m2)*60)+(s1*10+s2);
       printf("%d \n", counter);
      TimerEnable(TIMER1_BASE, TIMER_A);
      flag++;
      }
      }
}


  
}