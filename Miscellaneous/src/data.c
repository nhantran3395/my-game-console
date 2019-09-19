/**
 * @file data.c
 * @brief Provide functions for data manipulation
 *
 * This header file provide fucntions for integer and ASCII string conversion
 *
 * @author Tran Thanh Nhan
 * @date 06/30/2019
 *
 */

#include "../inc/data.h"
#include "../inc/memory.h"

uint32_t power(uint32_t base, uint32_t exponent);

uint8_t my_itoa(int32_t data,uint8_t *ptr,uint32_t base)
{
	_Bool isNegative = 0;
	uint32_t count=0;

	/*handle case when integer value is 0*/
	if(!data)
	{
		*(ptr+count) = '0';
		++count;
		*(ptr+count) = '\0';
		return (count+1);
	}

	if(data < 0)
	{
		isNegative = 1;
		data = -data;
	}

	while(data)
	{
		int32_t temp;
		temp = data % base;
		
		temp = (temp > 9)? temp -10 + 'A' : temp + '0';

		*(ptr + count) = temp;
		++count;
		data = data/base;
	}

	if (isNegative == 1)
	{
		*(ptr+count) = '-';
		++count;
	}

	ptr = my_reverse(ptr,count);

	*(ptr+count) = '\0';	

	return (count+1);
}

int32_t my_atoi( uint8_t *ptr, uint8_t digits, uint32_t base){
	uint32_t *tempArr;
	tempArr = (uint32_t*)malloc(digits*sizeof(uint32_t));

	_Bool isNegative = 0;
	int32_t outInteger = 0;	// output integer value

	int32_t ptrIndex = 0;

	/* Check if value to convert is negative or not. */
	/* If negative start indexing string from 2nd element */
	if(*ptr == '-'){
		isNegative = 1;
		ptrIndex = 1;
		--digits;	//exclude '-' sign 
	}

	--digits;	//exclude null operator

	for (int count=0; count < digits; count++){
		int32_t tempVal = *(ptr+ptrIndex);

		if (tempVal >= '0' && tempVal <= '9'){
			tempVal -= '0';
		}
		else if (tempVal >= 'A' && tempVal <= 'F'){
			tempVal = tempVal - 'A' + 10;
		}

		*(tempArr + count) = tempVal;
		++ptrIndex;
	}

	for (int count = 0; count < digits; count++){
		outInteger += *(tempArr + count)*power(base,(digits-count-1));
	}

	if (isNegative == 1){
		outInteger = -outInteger;
	}

	free(tempArr);
	return outInteger;
}

uint32_t power(uint32_t base, uint32_t exponent){
	int32_t result=1;
        for (int count = 0; count < exponent; count++){
	       result *= base;
        }
	return result;
}

