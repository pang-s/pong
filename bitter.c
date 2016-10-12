/** @file   bitter.c
    @author Pang Suwanaposee psu43 33330630 and Alex Lie lli134 66969353
    @date   7 Oct 2016
    @brief Implements helper functions to manipulate bits.
*/

#include "pio.h"
#include "var.h"
#include "bitter.h"
#include <math.h>

/** Lookup table needed for reversing bits. */
unsigned char lookup[16] = 
{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
};

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
