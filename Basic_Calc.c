/****************************************************
*  @file    Basic_Calc.c
*
*  Functions that 
*
*  @author  Group Alpha
*
*  @date    11/12/2018 12:10 AM
*
****************************************************/ 
#include "Basic_Calc.h"
#include "DIO.h"

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


#define sw1_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)==0x0

#define sw2_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)==0x0

#define sw1_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)!=0x0

#define sw2_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)!=0x0
bool isEnter = true  ;

//int memcount = 0;
//const int mul[10] = {1 , 10, 100, 1000, 
//                     10000, 100000, 1000000, 
//                     10000000, 100000000, 1000000000};
float mem[10]  = {0};           //to store the number of different digits 10,100,1000,...
float result   = 0;
//float num[10]  = {0};
//char sign[9]   = {0};
float num1 = 0;         //first operand of calculation
float num2 = 0;         //second operand of calculation
char opsympo  = '0';
char addition  = '0';
char subtract  = '0';
char multiply  = '0';
char division  = '0';
volatile unsigned char key1;
volatile float floatkey;



/**
*  @detailed  
*
*  @param     
*/
void start_calc (void)                  //start the calculator screen shown after selecting option 1 from the home  
{
  LCD_command(DISPLAY_ON_CURSOR_ON);
  delay_ms(50);
  LCD_command(CLEAR_DISPLAY);
  delay_ms(50);
  LCD_command(CLEAR_DISPLAY);
  delay_ms(50);
  LCD_command(CLEAR_DISPLAY);
  delay_ms(50);
  LCD_command(CLEAR_DISPLAY);
  LED_blue();
  LCD_printString("Let's Go:");
  LCD_command(CURSOR_START_2ND_LINE);
  LCD_command(DISPLAY_ON_CURSOR_ON);
  LCD_command(DISPLAY_ON_CURSOR_BLINK);
}




void
Timer0IntHandler(void)                           //ISR for the buttons to switch between modes
{   
  if ( GPIO_PORTF_MIS_R & 0x10 )                 //is the interrupt from SW1 PF4?
  {
    

    states_2=1;                                  //change the state to go to the timer

    mode_flag=true;                              // in t
    
  }
  else if  ( GPIO_PORTF_MIS_R & 0x01 )           //is the interrupt from SW2 PF0?
  {
    states_2=2;

    mode_flag=true;
  }
  
  GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_0);         //Clears the specified interrupt sources ,by clearing the interupt flag
   GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4);        //Clears the specified interrupt sources, by clearing the interupt flag
//not to listen for interrupts as its job is done  
   MAP_IntDisable(INT_GPIOF);                                   
   MAP_IntMasterDisable();
     
}







/**
*  @detailed  
*
*  @param     
*/
void calc(void)
{ 
  states_2=0;
  mode_flag=false;

     
  states_1=0;

    //
  MAP_IntDisable(INT_GPIOF);             // to prevent the timer from interrupting other functions because we will switch the mode
   
 
   MAP_IntMasterDisable();              
//
    MAP_IntEnable(INT_GPIOF);                   // enable the interrupt to portf from NVIC
//
  
  
     IntMasterEnable();                         //Enables the processor interrupt.
     __asm("CPSIE I"); 
      GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4) ;           //Enables the specified GPIO interrupts.
       GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0) ;

                  if (isEnter == true ){
 
 GPIOIntRegister(GPIO_PORTF_BASE,Timer0IntHandler);             //Registers an interrupt handler for a GPIO port.


                           }
 
  if (states_2==1){                                             //check the states to go for
                    loading();
                    timer();
  }
  else if(states_2==2){
  loading();
  stopwatch();
  }
isEnter = true ;                        // flag to stopr the interrupt

  start_calc ();
 
  while(1)
  {

    mode_flag=false;                    // we have to set the mode flag to false so the functions know we are making a function not interrupted by a switch yet
    key1 = keypad_getkey();             //wait for first operand of calculation 

    if (key1=='f'){                     //this if statement checks, if the character 'f' is returned then we have entered the interrupt and need to change the state
      if (states_2==1){                 //wanna Go to Timer?
                    loading();
                    timer();
      
      }
  else if(states_2==2){                // wanna go to topwatch?
  loading();
  stopwatch();
  }
    }

    else if      (key1 != '.')          // wanna reset?
    {
      if      (addition == '1')
      {
        LCD_data(key1);
        if(key1 == '=')
        {
          num2 = memfinalize(mem);     //to tranform the number from separate digits to a single number value  
          add(num1,num2);
          clear_data();
        }
        else
        {
          floatkey = key1 - 48;       //////////ASCI Compensation
          store(floatkey, mem);
        }
        delay_ms(500);
      }
      else if (subtract == '1')
      {
        LCD_data(key1);
        if(key1=='=')
        {
          num2 = memfinalize(mem);      //to tranform the number from separate digits to a single number value
          sub(num1, num2);
          clear_data();
        }
        else
        {
          floatkey = key1 - 48;      //////////ASCI Compensation
          store(floatkey, mem);
        }
        delay_ms(500);  
      }
      else if (multiply == '1')
      {
        LCD_data(key1);
        if(key1 == '=')
        {
          num2 = memfinalize(mem);      //to tranform the number from separate digits to a single number value
          mult(num1,num2);
          clear_data();
        }
        else
        {
          floatkey = key1 - 48;     //////////ASCI Compensation
          store(floatkey, mem);
        }
        delay_ms(500);
      }
      else if (division == '1')
      {
        LCD_data(key1);
        if(key1=='=')
        {
          num2 = memfinalize(mem);              //to tranform the number from separate digits to a single number value
          divi(num1,num2);
          clear_data();
        }
        else
        {
          floatkey = key1 - 48;   //////////ASCI Compensation
          store(floatkey, mem);
        }
        delay_ms(500); 
      }
      else 
      {
        LCD_data(key1);
        switch (key1)
        {
      case '+':
            addition = '1';
            num1 = memfinalize(mem);            //to tranform the number from separate digits to a single number value
            break;
      case '-':
            subtract = '1';
            num1 = memfinalize(mem);            //to tranform the number from separate digits to a single number value
            break;
      case '*':
            multiply = '1';
            num1 = memfinalize(mem);            //to tranform the number from separate digits to a single number value
            break;
      case '/':
            division = '1';
            num1 = memfinalize(mem);           //to tranform the number from separate digits to a single number value
            break;
      default:
          floatkey = key1 - 48;   //////////ASCI Compensation
          store(floatkey, mem);
        }
        delay_ms(500);
      }
    }
    else if (key1 == '.')
    {
      start_calc();
      clear_data();
      delay_ms(50);
      clear_data();
      delay_ms(50);
      clear_data();
      delay_ms(50);
      clear_data();
    }
  }
}



/**
*  @detailed  
*
*  @param     
*/
void fini_calc(float result)            // after finishiing the calculation to print the result in their specific places shown below
{
  LCD_command(DISPLAY_ON_CURSOR_OFF);
  LCD_command(CURSOR_START_2ND_LINE);
  LCD_printFloat(result);
  LED_green();
  LCD_command(CURSOR_START_1ST_LINE);
  LCD_printString("Press * to clear");
}




/**
*  @detailed  
*
*  @param     
*/
void add(float num1, float num2)                // make addition by the 2 entered numbers
{
  //result = num[0] + num[1];
  result = num1 + num2;
  fini_calc(result);
}


/**
*  @detailed  
*
*  @param     
*/
void sub(float num1, float num2)                // make subtraction by the 2 entered numbers
{
  //result = num[0] - num[1];
  result = num1 - num2;
  fini_calc(result);
}


/**
*  @detailed  
*
*  @param     
*/
void mult(float num1, float num2)               //// make multiplication by the 2 entered numbers
{
  //result = num[0] * num[1];
  result = num1 * num2;
  fini_calc(result);
}


/**
*  @detailed  
*
*  @param     
*/
void divi(float num1, float num2)              // make division by the 2 entered numbers
{
  if(num2 == 0)                                 // error message if the denominator is 0
  {
    LCD_command(DISPLAY_ON_CURSOR_OFF);
    LCD_command(CURSOR_START_2ND_LINE);
    LCD_errormsg();
    LED_red(); 
    LCD_command(CURSOR_START_1ST_LINE);
    LCD_printString("Press * to clear");
  }
  
  
  else if(num1 == 0)                            //divide 0 by any number do not require calculatin
  {
    LCD_command(DISPLAY_ON_CURSOR_OFF);
    LCD_command(CURSOR_START_2ND_LINE);
    LCD_data('0');
    LED_green();
    LCD_command(CURSOR_START_1ST_LINE);
    LCD_printString("Press * to clear");
  }
  
  
  else
  {
    //result = num[0] / num[1];
    result = num1 / num2;
    fini_calc(result);
  }
}

void clear_data()
{
//  for(int i = 0; i < 10; i++)
//    num[i] = 0;
  num1 = 0;
  num2 = 0;
  result    = 0;
  floatkey  = 0;
  addition  = '0';
  subtract  = '0';
  multiply  = '0';
  division  = '0';
}