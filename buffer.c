#include "buffer.h"


void Buffer_InitDynamic(Buffer_t *Buffer, size_t ItemCount, size_t ItemSize)
{
	Buffer->_ItemCount = 0; 
	Buffer->_MaxItemCount = ItemCount;
	Buffer->_ItemSize = ItemSize;
	Buffer->_DataSize = ItemCount * ItemSize;
	
	Buffer->_Data = (int8_t *)malloc(Buffer->_DataSize);
	
	Buffer->_ReadAddr = Buffer->_Data;
	Buffer->_WriteAddr = Buffer->_Data;
}

void Buffer_InitStatic(Buffer_t *Buffer, volatile int8_t* Data, size_t DataSize,
					   size_t ItemCount, size_t ItemSize)
{
	Buffer->_ItemCount = 0; 
	Buffer->_MaxItemCount = ItemCount;
	Buffer->_ItemSize = ItemSize;
	Buffer->_DataSize = DataSize;	
	Buffer->_Data = Data;
	Buffer->_ReadAddr = Data;
	Buffer->_WriteAddr = Data;
}

void Buffer_Reset(Buffer_t *Buffer)
{
	Buffer->_ItemCount = 0;
	Buffer->_ReadAddr = Buffer->_Data;
	Buffer->_WriteAddr = Buffer->_Data;
}

bool Buffer_Read(Buffer_t *Buffer, void *Item)
{
	size_t i;
	
	if (Buffer_IsEmpty(Buffer)) return false;

	int8_t * Veri = (int8_t *)Item;

	for (i=Buffer->_ItemSize; i; --i)
	{
		*(Veri++) = *(Buffer->_ReadAddr++);
	}
	
	if (Buffer->_ReadAddr >= (Buffer->_Data + Buffer->_DataSize))
	{
		Buffer->_ReadAddr = Buffer->_Data;
	}
	
	Buffer->_ItemCount--;

	return true;
}

bool Buffer_Write(Buffer_t *Buffer, void *Item)
{
	size_t i;
	
	if (Buffer_IsFull(Buffer)) return false;
	
	int8_t *Veri = (int8_t *)Item;
	
	for (i=Buffer->_ItemSize; i; --i)
	{
		*(Buffer->_WriteAddr++) = *(Veri++);
	}
	
	if (Buffer->_WriteAddr >= (Buffer->_Data + Buffer->_DataSize))
	{
		Buffer->_WriteAddr = Buffer->_Data;
	}
	
	Buffer->_ItemCount++;
	
	return true;
}

bool Buffer_IsEmpty(Buffer_t *Buffer)
{
	return (Buffer->_ItemCount == 0);
}

bool Buffer_IsFull(Buffer_t *Buffer)
{
	return (Buffer->_ItemCount == Buffer->_MaxItemCount);
}

size_t Buffer_GetCount(Buffer_t *Buffer)
{
	return (Buffer->_ItemCount);
} 



