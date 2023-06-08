#include "types.h"
#include "tm4c123gh6pm.h"
#ifndef dio_h
#define dio_h
void DIO_Init();
void DIO_WritePin(uint32_ptr port, uint8 pin, uint8 value);
void DIO_WritePort(uint32_ptr port, uint8 value);
uint8 DIO_ReadPin(uint32_ptr port, uint8 pin);
uint32 DIO_ReadPort(uint32_ptr port);
#endif