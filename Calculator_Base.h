/**************************************************
*  @file     Calculate.h
*
*  @author   Group Alpha
*
*  @date     4/12/2018 10:30 PM
*
**************************************************/
#ifndef __CALCULATOR_BASE_H__
#define __CALCULATOR_BASE_H__
#include "Calculator.h"
#include "Basic_Calc.h"
#include "Adv_Calc.h"
#include "stopwatch_2.h"
#include "LCD.h"
#include "Keypad.h"
#include "delay.h"
//int states_s=0;
//volatile int *statee =&states_s;
#define states_1 (*((volatile unsigned long *)0x20007FF0))
//static int states_1=2;
#define states_2 (*((volatile unsigned long *)0x20007FF4))
#define mode_flag (*((volatile bool *)0x20007FF8))
#define switch_to_timer_flag (*((volatile bool *)0x20007FFC))
void states (void);
void  getStarted   (void);
char  scanKeypad  (void);
void  errorEntry   (void);
void  store        (volatile float, float*);
float memfinalize  (float*);

#endif