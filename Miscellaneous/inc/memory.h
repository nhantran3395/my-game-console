/******************************************************************************
 * Copyright (C) 2017 by Alex Fosdick - University of Colorado
 *
 * Redistribution, modification or use of this software in source or binary
 * forms is permitted as long as the files maintain this copyright. Users are 
 * permitted to modify this and use it to learn about the field of embedded
 * software. Alex Fosdick and the University of Colorado are not liable for any
 * misuse of this material. 
 *
 *****************************************************************************/
/**
 * @file memory.h
 * @brief Abstraction of memory read and write operations
 *
 * This header file provides an abstraction of reading and
 * writing to memory via function calls. 
 *
 * @author Alex Fosdick
 * @date April 1 2017
 *
 */

#include <stdlib.h>
#include <stdint.h>

#ifndef __MEMORY_H__
#define __MEMORY_H__

/**
 * @brief Sets a value of a data array 
 *
 * Given a pointer to a char data set, this will set a provided
 * index into that data set to the value provided.
 *
 * @param ptr Pointer to data array
 * @param index Index into pointer array to set value
 * @param value value to write the the locaiton
 *
 * @return void.
 */
void set_value(char * ptr, unsigned int index, char value);

/**
 * @brief Clear a value of a data array 
 *
 * Given a pointer to a char data set, this will clear a provided
 * index into that data set to the value zero.
 *
 * @param ptr Pointer to data array
 * @param index Index into pointer array to set value
 *
 * @return void.
 */
void clear_value(char * ptr, unsigned int index);

/**
 * @brief Returns a value of a data array 
 *
 * Given a pointer to a char data set, this will read the provided
 * index into that data set and return the value.
 *
 * @param ptr Pointer to data array
 * @param index Index into pointer array to set value
 *
 * @return Value to be read.
 */
char get_value(char * ptr, unsigned int index);

/**
 * @brief Sets data array elements to a value
 *
 * Given a pointer to a char data set, this will set a number of elements
 * from a provided data array to the given value. The length is determined
 * by the provided size parameter.
 *
 * @param ptr Pointer to data array
 * @param value value to write the the locaiton
 * @param size Number of elements to set to value
 *
 * @return void.
 */
void set_all(char * ptr, char value, unsigned int size);

/**
 * @brief Clears elements in a data array
 *
 * Given a pointer to a char data set, this will set a clear a number
 * of elements given the size provided. Clear means to set to zero. 
 *
 * @param ptr Pointer to data array
 * @param size Number of elements to set to zero
 *
 * @return void.
 */
void clear_all(char * ptr, unsigned int size);

/**
 * @brief Copy data from source memory region to destination memory region
 *
 * Given two byte pointers (source and destination) and a length of bytes, this will copy data from source memory region to destination memory region.
 * Behavior if there is overlap of source and destination is undefined. Data corruption may occur.
 *
 * @param *src Pointer to source
 * @param *dst Pointer to destination
 * @param length Length of bytes to copy
 *
 * @return Pointer to destination
 */ 
uint8_t *my_memcopy(uint8_t *src,uint8_t *dst,size_t length);

/**
 * @brief Copy data from source memory region to destination memory region
 *
 * Given two byte pointer (source and destination) and a length of byte,this will copy data from source memory region to destination memory region.
 * Overlap of source and destination is handled. Copy occur with no data corruption.
 *
 * @param *src Pointer to source
 * @param *dst Pointer to destination
 * @param length Length of bytes to copy
 *
 * @return Pointer to destination
 */ 
uint8_t *my_memmove(uint8_t *src,uint8_t *dst,size_t length);

/**
 * @brief Set given value to all bytes in memory region
 *
 * Given a pointer to a memory region, a length of bytes and value, this will set all bytes in memory region to given value.
 *
 * @param *src Pointer to memory region
 * @param length Length of bytes to set value
 *
 * @return Pointer to memory region
 */
uint8_t *my_memset(uint8_t *src,size_t length,uint8_t value);

/**
 * @brief Set zero to all bytes in memory region
 *
 * Given a pointer to a memory region,a length of bytes, this will set all bytesin memory region to zero.
 *
 * @param *src Pointer to memory location
 * @param length Length of byte to copy
 *
 * @return Pointer to source
 */
uint8_t *my_memzero(uint8_t *src,size_t length);

/**
 *  @brief Reverse order of all of the bytes in memory region
 *   
 *  Given a pointer to a memory region, a length of bytes, this will reverse theorder of all of the bytes in that memory region.
 *     
 *  @param *src Pointer to memory location
 *  @param length Length of byte to copy
 *        
 *  @return Pointer to source
 */
uint8_t *my_reverse(uint8_t *src,size_t length);

/**
 * @brief Allocate a number of words in dynamic memory
 *
 * Given a number of words, this will allocate that number of words in dynamic memory
 *
 * @param length Number of words
 *
 * @return Pointer to allocated dynamic memory region
 */
int32_t *reserve_words(size_t length);

/**
 * @brief Free a dynamic memory region
 *
 * Given a pointer to a memory region,this will free that memory region.
 *
 * @param src Pointer to memory location
 *
 * return Void
 */
void free_words(uint32_t *src);

#endif /* __MEMORY_H__ */
