/**
 * @file data.h
 * @brief Provide functions for data manipulation
 *
 * This header file provide fucntions for integer and ASCII string conversion
 *
 * @author Tran Thanh Nhan
 * @date 06/30/2019
 *
 */

#include <stdint.h>

/**
 *
 * @brief convert interger to ASCII string 
 *
 * Given a signed interger value, a pointer to array, and an integer specified the base to convert, this will convert the interger value to ASCII string
 *
 * @param data - signed 32bit integer value needed to be converted
 * @param *ptr - pointer to array. converted ASCII string is stored in this array
 * @param base - base to convert to
 *
 * @return length of ASCII string
 *
 */
uint8_t my_itoa(int32_t data, uint8_t *ptr, uint32_t base);

/**
 *
 * @brief Convert ASCII string to integer 
 *
 * Given an ASCII string, number of digits in the string, and an integer value specified the base, this will convert the ASCII string to integer value
 *
 * @param *ptr - pointer to the string need to be converter
 * @param digits - numbers of digits in the string
 * @param base - base of the string
 *
 * @return integer value converted from string
 *
 */
int32_t my_atoi( uint8_t *ptr, uint8_t digits, uint32_t base);

