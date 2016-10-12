/** @file   bitter.h
    @author Pang Suwanaposee psu43 and Alex Lie lli134
    @date   7 Oct 2016
    @brief Implements helper functions to manipulate bits.
*/
#ifndef BITTER_H
#define BITTER_H

/** Lookup table needed for reversing bits. */
unsigned char lookup[16];

/** Given bits n, return the reversed version of the bits. */
uint8_t reverse(uint8_t n);

/** Encrypt ball bitmap into condensed form 
 * eg. takes 1000000 and turns into 110 */
uint8_t encrypt_ball(uint8_t bits);

/** Decrypt ball bitmap into true form
 * eg takes 110 turns into 1000000 */
uint8_t decrypt_ball(uint8_t bits);


#endif
