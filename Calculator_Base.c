/****************************************************
*  @file    Calclate.c
*
*  Functions that do mathematical calculations after getting
*  the input numbers from the keypad.
*
*  @author  Group Alpha
*
*  @date    4/12/2018 12:20 AM
*
****************************************************/ 
#include "Calculator_Base.h"
#include "Calculator.h" 


//
#define sw1_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)==0x0
#define sw1_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,4) & 0x1)!=0x0
#define sw2_on (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)==0x0
#define sw2_off (DIO_ReadPin(&GPIO_PORTF_DATA_R,0) & 0x1)!=0x0

int memcount = 0;
float mem2[10]  = {0};
volatile unsigned char key;

void init_PortfF (void){

SYSCTL_RCGCGPIO_R |= 0x20;

GPIO_PORTF_DIR_R = 0x0E;
GPIO_PORTF_DEN_R = 0x1F;
GPIO_PORTF_LOCK_R = 0x4C4F434B;
GPIO_PORTF_CR_R = 0x1F;
GPIO_PORTF_PUR_R = 0x11;

}
void init_PortD (void){

SYSCTL_RCGCGPIO_R |= 0x08;

GPIO_PORTD_DIR_R = 0x80;
GPIO_PORTD_DEN_R = 0x80;
GPIO_PORTD_LOCK_R = 0x4C4F434B;
GPIO_PORTD_CR_R = 0x80;
//GPIO_PORTD_PUR_R = 0x11;

}
void states (void){

 //states_1-- ; 
  //while (1){
    
    //////////////////////////////
  
  
  if ( sw1_on && sw2_off )
  {
  //Delay(); 
 //   LCD_command(CLEAR_DISPLAY);
//delay_ms(300);
 if ( states_1==0)
  {
    states_1=1;
   //  LCD_command(CLEAR_DISPLAY);
       loading();
    trig();
    while (sw1_on && sw2_off) ;
    
  }
   else if (states_1==1 )
  {
    states_1=2;
loading() ; 
stopwatch() ; 
    while (sw1_on && sw2_off) ;
  }
   else if (states_1==2 )
  {
    states_1=0;
   loading() ; 
     calc() ; 
    while (sw1_on && sw2_off) ;
  }
  
  //GPIO_PORTF_DATA_R =0x02;
  }

  
  else if ( sw1_off && sw2_on )
  {
       
//delay_ms(300);
   // Delay();
   
   if (  states_1==2  )
  {
    states_1=1;
  // LCD_command(CLEAR_DISPLAY);
    loading();
    trig();
    while (sw1_off && sw2_on) ;
  }
  else if ( states_1==1  )
  {
    states_1=0;
    loading();
    calc();
    while (sw1_off && sw2_on) ;
  }
  else if ( states_1==0 )
  {
    states_1=2;
    loading();
    stopwatch();
    while (sw1_off && sw2_on) ;
  }
  }

    
    
    
    



}

/**
*  @detailed  
*
*  @param     
*/
void getStarted(void)
{
  init_PortfF () ;
  init_PortD();
  calculator_menu();
  int c = 0;
  c = scanKeypad();
    switch (c)
  {
  case '1':                         ///Basic Calc
    loading();
    calc();
    break;
  case '2':                       ///Adv Calc
    loading();
    //trig();
    timer();
    break;
  case '3':                      ///Stop Watch
    loading();
    stopwatch();
    break;
  default:
    errorEntry();
    getStarted();
  }
  //delay_ms (300) ;
  /////////////////////////////////////////////////////
 
  /////////////////////////////////////////////////////////


}


/**
*  @detailed  
*
*  @param     
*/
char scanKeypad(void)
{ 
  key = keypad_getkey();
  return key;
}


/**
*  @detailed  
*
*  @param     
*/
void errorEntry(void)
{
  LCD_command(CLEAR_DISPLAY);
  LCD_setcursorRowCol(0,0);
  LCD_printString("WRONG ENTRY!!!");
  LCD_setcursorRowCol(2,0);
  LCD_printString("Try Again...");
  delay_ms(2000);
  LCD_command(CLEAR_DISPLAY);
}


/**
*  @detailed  
*
*  @param     
*/
void store(volatile float no, float* mem2)
{
  *(mem2 + memcount) = no;
  memcount++;
}


/**
*  @detailed  
*
*  @param     
*/
float memfinalize(float* mem2)
{
  const int mul[10] = {1 , 10, 100, 1000, 
                     10000, 100000, 1000000, 
                     10000000, 100000000, 1000000000};
  float memreturn = 0;
  for(int j = 0; j < 10; j++)
  {
    memreturn += ((mem2[j]) * mul[memcount-1]);
    memcount--;
    if(memcount == -1)
      break;
  }
  for(int k = 0; k < 10; k++)
    mem2[k] = 0;
  memcount = 0;
  return memreturn;
}