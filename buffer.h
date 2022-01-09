#ifndef BUFFER_H
#define BUFFER_H


#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"


typedef struct 
{
	volatile int8_t *_Data;
	volatile int8_t *_WriteAddr;
	volatile int8_t *_ReadAddr;
	volatile size_t _DataSize;
	volatile size_t _ItemSize;	
	volatile size_t _ItemCount;
	volatile size_t _MaxItemCount;
	
} Buffer_t;


void Buffer_InitDynamic(Buffer_t *Buffer, size_t ItemCount, size_t ItemSize);
void Buffer_InitStatic(Buffer_t *Buffer, volatile int8_t* Data, size_t DataSize, size_t ItemCount, size_t ItemSize);
void Buffer_Reset(Buffer_t *Buffer);
bool Buffer_Read(Buffer_t *Buffer, void *Item);
bool Buffer_Write(Buffer_t *Buffer, void *Item);
bool Buffer_IsEmpty(Buffer_t *Buffer);
bool Buffer_IsFull(Buffer_t *Buffer);
size_t Buffer_GetCount(Buffer_t *Buffer);


#endif
