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
bool ball_shot = false;
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


static void ini(void) {
        pio_config_set (LEDMAT_ROW1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW5_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW6_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_ROW7_PIO, PIO_OUTPUT_HIGH);

    pio_config_set (LEDMAT_COL1_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_COL2_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_COL3_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_COL4_PIO, PIO_OUTPUT_HIGH);
    pio_config_set (LEDMAT_COL5_PIO, PIO_OUTPUT_HIGH);
}



static uint8_t bitmap[] =
{
	// 0x00, 0x00, 0x00, 0b0001000, 0b0011100
    0x00, 0x00, 0x00, 0x00, 0b0011100
};

static unsigned char lookup[16] = {
0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf, };

int bounce_to;
int bounce_from;
int prev_shot_row = 3;
int new_shot_row = 2;
bool received = false;

int receive_go_to = 1;
int receive_prev = 0;
bool bounce = false;
int communicated = 0;
bool opp_start = false;

/*
uint8_t reverse(uint8_t x)
{
    x = ((x >> 1) & 0x55555555u) | ((x & 0x55555555u) << 1);
    x = ((x >> 2) & 0x33333333u) | ((x & 0x33333333u) << 2);
    x = ((x >> 4) & 0x0f0f0f0fu) | ((x & 0x0f0f0f0fu) << 4);
    return x;
}
*/

uint8_t reverse(uint8_t n) {
  //int intSize = 8;
	return (lookup[n&0b1111] << 3) | lookup[n>>3];
}


/*
// encrypt ball bitmap into condensed form
// eg. takes 1000000 and turns into 111
uint8_t encrypt_ball(uint8_t bits)
{
	return log(bits) / log(2) + 1;
}

// decrypt ball bitmap into true form
// eg takes 111 turns into 1000000
uint8_t decrypt_ball(uint8_t bits)
{
	return pow(2,bits);
}
* */



void display_column (uint8_t row_pattern, uint8_t current_column)
{

    int current_row = 0;
    pio_output_high(cols[prev_column]);

    for (; current_row < 7; current_row++) {
        if ((row_pattern >> current_row) & 1) {
            pio_output_low(rows[current_row]);
        } else {
            pio_output_high(rows[current_row]);
        }
    }
    pio_output_low(cols[current_column]);
    prev_column = current_column;
    
    

}

static void board(__unused__ void *data) {

    display_column (bitmap[current_column], current_column);

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }
    
    // send ball to opponent once the ball is at the edge of the screen
    if ((ball_shot && bitmap[0] !=  0) || (bounce && bitmap[0] != 0) ) {
		ir_uart_putc(bitmap[0]); // send ball
		//uint8_t e = encrypt_ball(bitmap[0]);
		//ir_uart_putc(e);
		bitmap[0] = 0x00; // because ball has left your screen
		ball_shot = false;
		bounce = false;
	}
	
	// ready to receive 
	if(ir_uart_read_ready_p()){
		if(communicated == 1){
			
			uint8_t rec_char = ir_uart_getc();
			// if receive signal from opp, you get the ball
			if(rec_char == 'S'){
				communicated = 2; // made contact with opp
				// give ball
				bitmap[3] = 0b0001000;
				// tell opp you got ball
				ir_uart_putc('B');
			}
			
			if(rec_char == 'B'){
				// opponent has the ball
				opp_start = true;
				communicated = 2;
			}
			
		}
		else{
			// should receive a ball
			//bitmap[0] = log(ir_uart_getc())/log(2);
			bitmap[0] = reverse(ir_uart_getc());
			// this shall convert the read bitmap
			//bitmap[0] = log(reverse(ir_uart_getc()))/log(2) + 1;
			received = true;
		}
		/*
		else{
			bitmap[0] = rec_char;
		}
		* */
		// received ball at col 0 so keep receiving ball
		//received = true;
		
	}


}








static void button_task_init (void)
{
    button_init ();
}




static void button_task (__unused__ void *data)
{
    button_update ();
    navswitch_update ();
    
    // if havent made contact with opp
	if (communicated == 0)
	{
		// send signal to opp
		ir_uart_putc('S');
		communicated = 1; // sent signal
		
	}
	
	// has sent or received connection with opp
	if (communicated == 2){
		
		
		// go left
		if (navswitch_push_event_p (NAVSWITCH_SOUTH) && bitmap[4] < 112)
		{
		  bitmap[4] = bitmap[4]*2; // move the bat to the left
		  // if ball hasnt been shot yet, move the ball with the bat
		  if(ball_shot == false){
			  bitmap[3] = bitmap[3]*2;
		  }
		}

		// go right
		if (navswitch_push_event_p (NAVSWITCH_NORTH) && bitmap[4] > 7)
		{
		  bitmap[4] = bitmap[4]/2; // move the bat to the right
		  // if ball hasnt been shot yet, move the ball with the bat
		  if(ball_shot == false){ 
			  bitmap[3] = bitmap[3]/2;
		  }
		}

		// push button and ball hasn't been shot yet and opp hasnt got ball at start
		if (opp_start == false && ball_shot == false && navswitch_push_event_p (NAVSWITCH_PUSH))
		{
		  ball_shot = true; // shoot ball
		}

		// if ball shot, show ball across screen til col 0
		if(ball_shot && new_shot_row >= 0){
			bitmap[new_shot_row] = bitmap[prev_shot_row];
			bitmap[prev_shot_row] = 0x00;
			prev_shot_row--;
			new_shot_row--;
		}
		

		
		if(flyout){
			bitmap[4] = flybit;
			flyout = false;
		}
		
		// received ball and check if ball clashed
		if(received && receive_go_to == 4){
			// check for clashing
			if((bitmap[4]|bitmap[3]) != bitmap[4]){
				// not on bat, make ball fly out
				flybit = bitmap[4];
				bitmap[4] = bitmap[4]|bitmap[3];
				bitmap[3] = 0x00;
				flyout = true;
			}
			else{
				// get ball ready to bounce back
				bounce_from = 3;
				bounce_to = 2;
				bounce = true;
			}
			
			//reset to receive later
			receive_go_to = 1;
			receive_prev = 0;
			received = false;
		
		}
		
		if(bounce && bounce_to >= 0){
			bitmap[bounce_to] = bitmap[bounce_from];
			bitmap[bounce_from] = 0x00;
			bounce_from--;
			bounce_to--;
		}
		
				// received a ball so show ball fly into screen
		if(received && receive_go_to <= 3){
			
			bitmap[receive_go_to] = bitmap[receive_prev];
			bitmap[receive_prev] = 0x00;
			receive_prev++;
			receive_go_to++;
			
			/*
			if (receive_go_to == 3) {
				int bounce_back = bitmap[4] | bitmap[3];
				if (bounce_back % 7 == 0) {
					bounce = true;
					new_shot_row = 2;
					prev_shot_row = 3;
					receive_go_to = 1;
					receive_prev = 0;
				}
			}
			*/
		
		}
	
		
	}
/*	
	if(bounce){
		bitmap[new_shot_row] = bitmap[prev_shot_row];
        bitmap[prev_shot_row] = 0x00;
		prev_shot_row--;
        new_shot_row--;
        if (prev_shot_row == 0) {
			bounce = false;
		}
	}
*/
}



int main (void)
{
    system_init ();
    ir_uart_init();
    ini();

    task_t tasks[] =
    {
        {.func = board, .period = TASK_RATE / 2000},
        {.func = button_task, .period = TASK_RATE / 10}
    };

    button_task_init ();
    task_schedule (tasks, ARRAY_SIZE (tasks));
    return 0;
}
