/** @file   game.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
*/

#include <avr/io.h>
#include "system.h"
#include "button.h"
#include "task.h"
#include "navswitch.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "ir_uart.h"
#include "var.h"
#include "message.h"
#include "bitter.h"


#define LED_PIO PIO_DEFINE (PORT_C, 2)
#define DISPLAY_TASK_RATE 250
#define BUTTON_TASK_RATE 7


//~ // used in board
//~ uint8_t current_column = 0;
//~ uint8_t rec_ball;


//~ // used in button
//~ int prev_shot_row = 3;
//~ int new_shot_row = 2;
//~ uint8_t flybit;
//~ bool flyout = false;
//~ int bounce_straight_to;
//~ int bounce_straight_from;
//~ int bounce_left_to;
//~ int bounce_left_from;
//~ int bounce_right_to;
//~ int bounce_right_from;
//~ int receive_go_to = 1;
//~ int receive_prev = 0;
//~ bool reflect_right = false;
//~ bool reflect_left = false;
//~ bool bounce_straight = false;
//~ bool bounce_left = false;
//~ bool bounce_right = false;
//~ bool receiving_left = false;
//~ bool receiving_right = false;


//~ // used in both
//~ int prev_column = 0;
//~ uint8_t direction = 0;
//~ bool ball_shot = false;
//~ bool received = false;
//~ int communicated = 0;
//~ bool opp_start = false;
//~ bool bounce = false;
//~ bool delete_col_0 = false;

//~ /** Define PIO pins driving LED matrix rows. */
//~ static const pio_t rows[] = 
//~ {
    //~ LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    //~ LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    //~ LEDMAT_ROW7_PIO
//~ };


//~ /** Define PIO pins driving LED matrix columns. */
//~ static const pio_t cols[] = 
//~ {
    //~ LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    //~ LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
//~ };



//~ /** Initially place bat at bottom middle of screen. */
//~ uint8_t bitmap[] = 
//~ {
    //~ 0x00, 0x00, 0x00, 0x00, 0b0011100
//~ };

//~ /** Lookup table needed for reversing bits. */
//~ static unsigned char lookup[16] = 
//~ {
    //~ 0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    //~ 0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
//~ };

/** Initially set all PIO rows and cols to high. */
static void ini(void)
{
    pio_config_set(LEDMAT_ROW1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW5_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW6_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_ROW7_PIO, PIO_OUTPUT_HIGH);

    pio_config_set(LEDMAT_COL1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set(LEDMAT_COL5_PIO, PIO_OUTPUT_HIGH);
}

//~ /** Given bits n, return the reversed version of the bits. */
//~ uint8_t reverse(uint8_t n)
//~ {
    //~ return (lookup[n & 0b1111] << 3) | lookup[n >> 3];
//~ }

//~ /** Encrypt ball bitmap into condensed form 
 //~ * eg. takes 1000000 and turns into 110 */
//~ uint8_t encrypt_ball(uint8_t bits)
//~ {
    //~ return log(bits) / log(2);
//~ }

//~ /** Decrypt ball bitmap into true form
 //~ * eg takes 110 turns into 1000000 */
//~ uint8_t decrypt_ball(uint8_t bits)
//~ {
    //~ int i;
    //~ int val = 1;
    //~ for (i = 0; i < bits; i++) 
    //~ {
        //~ val = val * 2;
    //~ }
    //~ return val;
//~ }

/** Given the row pattern and current column, display column. */
void display_column(uint8_t row_pattern, uint8_t current_column)
{
    int current_row = 0;
    pio_output_high(cols[prev_column]);    // set previous column high

	// loop through until current_row is less than 7
    for (; current_row < 7; current_row++) 
    {
        if ((row_pattern >> current_row) & 1) 
        {
            pio_output_low(rows[current_row]);
        } else 
        {
            pio_output_high(rows[current_row]);
        }
    }
    pio_output_low(cols[current_column]); // set current column low
    prev_column = current_column;
}


/** Construct a message then send a message with ball information. */
//~ void send_ball_msg(void)
//~ {
	//~ uint8_t ball_msg = encrypt_ball(bitmap[0]);
	//~ uint8_t message = direction + ball_msg;
	//~ ir_uart_putc(message);  // send message
	//~ delete_col_0 = true;    // because ball has left screen
	//~ ball_shot = false;
	//~ bounce = false;
//~ }

/** Ready to receive a signal from opponent. */
//~ void receive_opp_signal(void)
//~ {
	//~ uint8_t rec_char = ir_uart_getc();
	//~ // if receive 'S' from opp, add ball to bat
	//~ if (rec_char == 'S') 
	//~ {
		//~ communicated = GOT_SIGNAL;	// made contact with opp
		//~ bitmap[3] = 0b0001000;		// give ball
		//~ ir_uart_putc('B');		// tell opp you got ball
	//~ }

	//~ // if receive 'B' from opp, opponent has ball
	//~ if (rec_char == 'B') 
	//~ {
		//~ // opponent will start with ball
		//~ opp_start = true;
		//~ communicated = GOT_SIGNAL;
	//~ }
//~ }

//~ /** Ready to receive a game message from opponent. */
//~ void receive_game_msg(void)
//~ {
	//~ // should receive a message
	//~ uint8_t rec_msg = ir_uart_getc();
	//~ // so decrypt ball, get last three bits
	//~ rec_ball = rec_msg & 0b111;

	//~ // get direction of ball
	//~ uint8_t rec_direct = rec_msg & 0x18;
	//~ if (rec_direct == 0b10000) 
	//~ {
		//~ // receive ball flying left
		//~ received = true;
		//~ receiving_left = true;

	//~ } 
	//~ else if (rec_direct == 0b1000) 
	//~ {
		//~ // receive ball flying right
		//~ received = true;
		//~ receiving_right = true;
	//~ }

	//~ // check that the receieved ball shows one dot
	//~ if (rec_ball >= 0 && rec_ball <= 6) 
	//~ {
		//~ // show received ball on screen
		//~ bitmap[0] = reverse(decrypt_ball(rec_ball));
		//~ received = true;
	//~ }
//~ }


/** Board to be updated. */
static void board(__unused__ void *data)
{
    display_column(bitmap[current_column], current_column);
    current_column++;
    // check if current column needs to loop back to 0
    if (current_column > (LEDMAT_COLS_NUM - 1)) 
    {
        current_column = 0;
    }
    // send ball to opponent once the ball is at the edge of the screen
    if ((ball_shot && bitmap[0] != 0) || (bounce && bitmap[0] != 0)) 
    {
		send_ball_msg();
    }
    // ready to receive 
    if (ir_uart_read_ready_p()) 
    {
        if (communicated == SENT_SIGNAL) 
        {
			receive_opp_signal();
        } 
        else 
        {
			receive_game_msg();
        }
    }
}


/** Initialise button. */
static void button_task_init(void)
{
    button_init();
}


/** Check player controls. */
void check_control(void)
{
	// Check if player wants to go left
	if (navswitch_push_event_p(NAVSWITCH_SOUTH) && bitmap[4] < 112) 
	{
		bitmap[4] = bitmap[4] * 2;  // move the bat to the left
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] * 2; // move ball with bat.
		}
	}
	
	// Check if player wants to go right
	if (navswitch_push_event_p(NAVSWITCH_NORTH) && bitmap[4] > 7) 
	{
		bitmap[4] = bitmap[4] / 2;  // move the bat to the right
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] / 2; // move ball with bat.
		}
	}
	
	// Check button has been pushed and ball hasn't been shot yet 
	// and opponent do not have ball initially.
	if (opp_start == false && ball_shot == false
		&& navswitch_push_event_p(NAVSWITCH_PUSH)) 
	{
		ball_shot = true;   // shoot ball
	}
}

/** If ball shot, show ball path across screen. */
void ball_shot_path(void)
{
	// check if ball shot and ball is not at column less than 0
	if (ball_shot && new_shot_row >= 0) 
	{
		bitmap[new_shot_row] = bitmap[prev_shot_row];
		bitmap[prev_shot_row] = 0x00;
		prev_shot_row--;
		new_shot_row--;
	}
	
	// activated when ball is at col 0
	if (delete_col_0) 
	{
		bitmap[0] = 0x00; // delete ball from col 0
		delete_col_0 = false;
	}
}

/** Check if ball needs to be shown flying out of screen. */
void check_flyout(void)
{
	if (flyout) // if activated, show ball flying out of screen
	{
		// make ball fly out of screen
		bitmap[4] = flybit;
		flyout = false;
	}
}

/** Bounce ball to the left. */
void ball_bounce_left(void)
{
	// hits right of bat so bounce to the left
	bounce_left_from = 3;
	bounce_left_to = 2;
	bounce_left = true;
	bounce = true;
	reflect_right = false;
	direction = FLY_RIGHT;       // fly to the right
}

/** Bounce ball to the middle. */
void ball_bounce_middle(void)
{
	// hits middle go middle                                
	bounce_straight_from = 3;
	bounce_straight_to = 2;
	bounce_straight = true;
	bounce = true;
	direction = 0;
}

/** Bounce ball to the right. */
void ball_bounce_right(void)
{
	// hits left of bat so bounce to the right
	bounce_right_from = 3;
	bounce_right_to = 2;
	bounce_right = true;
	bounce = true;
	reflect_left = false;
	direction = FLY_LEFT;        // fly to the left
}

/** Check actions after receiving ball. */
void receive_ball(void)
{
	// check for clashing/hitting bat
	if ((bitmap[4] | bitmap[3]) != bitmap[4]) 
	{
		// not on bat, make ball show next to bat
		flybit = bitmap[4];
		bitmap[4] = bitmap[4] | bitmap[3];
		bitmap[3] = 0x00;
		flyout = true;
	} else 
	{
		// get ball ready to bounce back
		// check where ball hits bat
		uint8_t bat = bitmap[4];
		uint8_t ball = bitmap[3];
		if (bat / 7 == ball) 
		{
			ball_bounce_left();
		} else if (2 * (bat / 7) == ball) 
		{
			ball_bounce_middle();

		} else 
		{
			ball_bounce_right();
		}
	}
}

/** Reset variables so board can receive balls later. */
void reset_receiver(void)
{
	receive_go_to = 1;
	receive_prev = 0;
	received = false;
	receiving_left = false;
	receiving_right = false;	
}

/** Bounce ball straight back. */
void bounce_straight_back(void)
{
	// update ball to show in next column
	bitmap[bounce_straight_to] = bitmap[bounce_straight_from];
	bitmap[bounce_straight_from] = 0x00; // clear ball from prev col
	bounce_straight_from--;
	bounce_straight_to--;
}

/** Bounce ball back to the left. */
void bounce_back_left(void)
{
	// check if ball is reflected 
	if (bitmap[bounce_left_from] != 64 && reflect_right == false) 
	{
		// update ball to show in next column
		bitmap[bounce_left_to] = 2 * bitmap[bounce_left_from];
	} 
	else 
	{
		// ball reflected, update ball to show in next column
		bitmap[bounce_left_to] = bitmap[bounce_left_from] / 2;
		reflect_right = true;
		direction = FLY_RIGHT;   // fly to the right
	}
	bitmap[bounce_left_from] = 0x00; // clear ball from prev col
	bounce_left_from--;
	bounce_left_to--;
}

/** Bounce ball back to the right. */
void bounce_back_right(void)
{
	// check if ball is reflected 
	if (bitmap[bounce_right_from] != 1 && reflect_left == false) 
	{
		// update ball to show in next column
		bitmap[bounce_right_to] = bitmap[bounce_right_from] / 2;
	} 
	else 
	{
		// ball reflected, update ball to show in next column
		bitmap[bounce_right_to] = bitmap[bounce_right_from] * 2;
		reflect_left = true;
		direction = FLY_LEFT;    // fly to the left
	}
	bitmap[bounce_right_from] = 0x00; // clear ball from prev col
	bounce_right_from--;
	bounce_right_to--;
}

/** Display the received ball in the correct direction. */
void check_receiving_direction(void)
{
	// received a ball so show ball fly into screen
	if (received && receive_go_to <= 3) 
	{
		// check if ball should travel to the left, right or middle
		// then update the bitmap corresponding to the direction
		if (receiving_left) 
		{
			bitmap[receive_go_to] = bitmap[receive_prev] * 2;
		} else if (receiving_right) 
		{
			bitmap[receive_go_to] = bitmap[receive_prev] / 2;
		} else 
		{
			bitmap[receive_go_to] = bitmap[receive_prev];
		}
		bitmap[receive_prev] = 0x00;
		receive_prev++;
		receive_go_to++;
	}
}

/** Check if ball has been received and undergo actions to ball. */
void check_received_ball(void)
{
	// received ball and check if ball hits the bat
	if (received && receive_go_to == 4) 
	{
		receive_ball();
		reset_receiver();
	}
	
	// bounce straight back
	if (bounce_straight && bounce_straight_to >= 0) 
	{
		bounce_straight_back();
	}

	// bounce to the left
	if (bounce_left && bounce_left_to >= 0) 
	{
		bounce_back_left();
	}

	// bounce to the right
	if (bounce_right && bounce_right_to >= 0) 
	{
		bounce_back_right();
	}
}

/** Connected with opponent so check conditions and actions to do. */
void check_conditions(void)
{
	check_control(); // check controls from user
	ball_shot_path(); // check if need to show ball shot path
	check_flyout(); // check if need to show ball fly off screen
	check_received_ball(); // check if ball is received
	check_receiving_direction(); // check direction of received ball
}


//~ /** Send signal to opponent. */
//~ void send_signal(void)
//~ {
	//~ ir_uart_putc('S'); // send signal
	//~ communicated = SENT_SIGNAL;       // sent signal
//~ }

/** Tasks related to button is updated. */
static void button_task(__unused__ void *data)
{
    button_update();
    navswitch_update();

    // if havent made contact with opp
    if (communicated == NO_CONTACT) 
    {
		send_signal(); // send signal to opp
    }
    // has sent or received connection with opp
    if (communicated == GOT_SIGNAL) 
    {
		check_conditions();
    }

}

/** Main function of program. */
int main(void)
{
    system_init();
    ir_uart_init();
    ini(); // initialise PIO pins

    task_t tasks[] = 
    {
        {.func = board,.period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = button_task,.period = TASK_RATE / BUTTON_TASK_RATE}
    };

    button_task_init();
    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
