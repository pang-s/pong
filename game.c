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


#define LED_PIO PIO_DEFINE (PORT_C, 2)
#define DISPLAY_TASK_RATE 250
#define BUTTON_TASK_RATE 100

int prev_column = 0;
bool flyout = false;
uint8_t current_column = 0;
uint8_t flybit;
uint8_t direction = 0;
bool ball_shot = false;
int bounce_straight_to;
int bounce_straight_from;
int bounce_left_to;
int bounce_left_from;
int bounce_right_to;
int bounce_right_from;
int prev_shot_row = 3;
int new_shot_row = 2;
bool received = false;
bool reflect_right = false;
bool reflect_left = false;

int receive_go_to = 1;
int receive_prev = 0;
bool bounce_straight = false;
bool bounce_left = false;
bool bounce_right = false;
int communicated = 0;
bool opp_start = false;
bool bounce = false;
bool receiving_left = false;
bool receiving_right = false;
uint8_t rec_ball;
bool delete_col_0 = false;

/** Define PIO pins driving LED matrix rows.  */
static const pio_t rows[] = 
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns.  */
static const pio_t cols[] = 
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


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



static uint8_t bitmap[] = 
{
    0x00, 0x00, 0x00, 0x00, 0b0011100
};

static unsigned char lookup[16] = 
{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
};




uint8_t reverse(uint8_t n)
{
    return (lookup[n & 0b1111] << 3) | lookup[n >> 3];
}



// encrypt ball bitmap into condensed form
// eg. takes 1000000 and turns into 110
uint8_t encrypt_ball(uint8_t bits)
{
    return log(bits) / log(2);
}


// decrypt ball bitmap into true form
// eg takes 110 turns into 1000000
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




void display_column(uint8_t row_pattern, uint8_t current_column)
{
    int current_row = 0;
    pio_output_high(cols[prev_column]);

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
    pio_output_low(cols[current_column]);
    prev_column = current_column;
}

void send_ball_msg(void)
{
	// construct message then send
	uint8_t ball_msg = encrypt_ball(bitmap[0]);
	uint8_t message = direction + ball_msg;

	ir_uart_putc(message);  // send message
	delete_col_0 = true;    // because ball has left your screen
	ball_shot = false;
	bounce = false;
}

void receive_opp_signal(void)
{
	uint8_t rec_char = ir_uart_getc();
	// if receive signal from opp, you get the ball
	if (rec_char == 'S') 
	{
		communicated = 2;       // made contact with opp
		// give ball
		bitmap[3] = 0b0001000;
		// tell opp you got ball
		ir_uart_putc('B');
	}

	if (rec_char == 'B') 
	{
		// opponent has the ball
		opp_start = true;
		communicated = 2;
	}
}

void receive_game_msg(void)
{
	// should receive a message
	uint8_t rec_msg = ir_uart_getc();
	// so decrypt ball, get last three bits
	rec_ball = rec_msg & 0b111;

	// get direction of ball
	uint8_t rec_direct = rec_msg & 0x18;
	if (rec_direct == 0b10000) 
	{
		// receive ball flying left
		received = true;
		receiving_left = true;

	} 
	else if (rec_direct == 0b1000) 
	{
		// receive ball flying right
		received = true;
		receiving_right = true;
	}

	if (rec_ball >= 0 && rec_ball <= 6) 
	{
		bitmap[0] = reverse(decrypt_ball(rec_ball));
		received = true;
	}
}

static void board(__unused__ void *data)
{
    display_column(bitmap[current_column], current_column);
    current_column++;
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
        if (communicated == 1) 
        {
			receive_opp_signal();
        } 
        else 
        {
			receive_game_msg();
        }
    }
}




static void button_task_init(void)
{
    button_init();
}


void check_control(void)
{
	// go left
	if (navswitch_push_event_p(NAVSWITCH_SOUTH) && bitmap[4] < 112) 
	{
		bitmap[4] = bitmap[4] * 2;  // move the bat to the left
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] * 2;
		}
	}
	// go right
	if (navswitch_push_event_p(NAVSWITCH_NORTH) && bitmap[4] > 7) 
	{
		bitmap[4] = bitmap[4] / 2;  // move the bat to the right
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] / 2;
		}
	}
	
	// push button and ball hasn't been shot yet and opp hasnt got ball at start
	if (opp_start == false && ball_shot == false
		&& navswitch_push_event_p(NAVSWITCH_PUSH)) 
	{
		ball_shot = true;   // shoot ball
	}
}

void ball_shot_path(void)
{
	// if ball shot, show ball across screen til col 0
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

void check_flyout(void)
{
	if (flyout) // if activated, show ball flying out of screen
	{
		// make ball fly out of screen
		bitmap[4] = flybit;
		flyout = false;
	}
}


void ball_bounce_left(void)
{
	// hits right of bat so bounce to the left
	bounce_left_from = 3;
	bounce_left_to = 2;
	bounce_left = true;
	bounce = true;
	reflect_right = false;
	direction = 0b10000;       // fly to the right
}

void ball_bounce_middle(void)
{
	// hits middle go middle                                
	bounce_straight_from = 3;
	bounce_straight_to = 2;
	bounce_straight = true;
	bounce = true;
	direction = 0;
}

void ball_bounce_right(void)
{
	// hits left of bat so bounce to the right
	bounce_right_from = 3;
	bounce_right_to = 2;
	bounce_right = true;
	bounce = true;
	reflect_left = false;
	direction = 0b1000;        // fly to the left
}

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

void reset_receiver(void)
{
	//reset to receive later
	receive_go_to = 1;
	receive_prev = 0;
	received = false;
	receiving_left = false;
	receiving_right = false;	
}

void bounce_straight_back(void)
{
	bitmap[bounce_straight_to] = bitmap[bounce_straight_from];
	bitmap[bounce_straight_from] = 0x00;
	bounce_straight_from--;
	bounce_straight_to--;
}

void bounce_back_left(void)
{
	if (bitmap[bounce_left_from] != 64 && reflect_right == false) 
	{
		bitmap[bounce_left_to] = 2 * bitmap[bounce_left_from];
	} else 
	{
		bitmap[bounce_left_to] = bitmap[bounce_left_from] / 2;
		reflect_right = true;
		direction = 0b10000;   // fly to the right
	}
	bitmap[bounce_left_from] = 0x00;
	bounce_left_from--;
	bounce_left_to--;
}

void bounce_back_right(void)
{
	if (bitmap[bounce_right_from] != 1 && reflect_left == false) 
	{
		bitmap[bounce_right_to] = bitmap[bounce_right_from] / 2;
	} else 
	{
		bitmap[bounce_right_to] = bitmap[bounce_right_from] * 2;
		reflect_left = true;
		direction = 0b1000;    // fly to the left
	}
	bitmap[bounce_right_from] = 0x00;
	bounce_right_from--;
	bounce_right_to--;
}

void check_receiving_direction(void)
{
	// received a ball so show ball fly into screen
	if (received && receive_go_to <= 3) 
	{
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

void check_conditions(void)
{
	check_control();
	ball_shot_path();
	check_flyout();
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
	check_receiving_direction();
}
static void button_task(__unused__ void *data)
{
    button_update();
    navswitch_update();

    // if havent made contact with opp
    if (communicated == 0) 
    {
        // send signal to opp
        ir_uart_putc('S');
        communicated = 1;       // sent signal

    }
    // has sent or received connection with opp
    if (communicated == 2) 
    {
		check_conditions();
    }

}



int main(void)
{
    system_init();
    ir_uart_init();
    ini();

    task_t tasks[] = 
    {
        {.func = board,.period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = button_task,.period = TASK_RATE / 10}
    };

    button_task_init();
    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
