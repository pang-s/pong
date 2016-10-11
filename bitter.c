/** @file   bitter.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Implements helper functions to manipulate bits.
*/

#include "pio.h"
#include "var.h"
#include "bitter.h"

/** Given bits n, return the reversed version of the bits. */
uint8_t reverse(uint8_t n)
{
    return (lookup[n & 0b1111] << 3) | lookup[n >> 3];
}

/** Encrypt ball bitmap into condensed form 
 * eg. takes 1000000 and turns into 110 */
uint8_t encrypt_ball(uint8_t bits)
{
    return log(bits) / log(2);
}

/** Decrypt ball bitmap into true form
 * eg. takes 110 turns into 1000000 */
uint8_t decrypt_ball(uint8_t bits)
{
    int i;
    int val = 1;
    for (i = 0; i < bits; i++) 
    {
        val = val * 2;
    }
    return val;
}
