/** @file   message.h
    @author Pang Suwanaposee psu43 33330630 and Alex Lie lli134 66969353
    @date   7 Oct 2016
    @brief This implements functions to do with sending and receiving
    messages.
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "system.h"
#include "pio.h"

/** Construct a message then send a message with ball information. */
void send_ball_msg(void);

/** Send signal to opponent. */
void send_signal(void);

/** Tell opponent they won */
void give_medal(void);

/** Ready to receive a signal from opponent. */
void receive_opp_signal(void);

/** Ready to receive a game message from opponent. */
void receive_game_msg(void);

/** Ready to send messages to opponent. */
void ready_to_send(void);

/** Ready to receive messages from opponent. */
void ready_to_receive(void);

#endif
