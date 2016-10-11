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
#include "joystick.h"
#include "getball.h"
#include "hitball.h"

#include "tinygl.h"
#include "../fonts/font3x5_1.h"

/* Define polling rate in Hz.  */
#define LOOP_RATE 300

/* Define text update rate (characters per 10 s).  */
#define MESSAGE_RATE 10


#define LED_PIO PIO_DEFINE (PORT_C, 2)
#define DISPLAY_TASK_RATE 250
#define BUTTON_TASK_RATE 7


/** Initially set all PIO rows and cols to high. */
static void pio_init(void)
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

/** Display bitmap on the board. */
void display_board(void)
{
	display_column(bitmap[current_column], current_column);
	current_column++;
	// check if current column needs to loop back to 0
	if (current_column > (LEDMAT_COLS_NUM - 1)) 
	{
		current_column = 0;
	}	
}

/** Board to be updated. */
static void board(__unused__ void *data)
{
	if(lose || win)
	{
		// display text on screen
		tinygl_update ();
		tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
	}
	else
	{
		display_board();
		ready_to_send();
		ready_to_receive();
	}

}

/** Check game conditions. */
void check_conditions(void)
{
	check_control(); // check controls from user
	ball_shot_path(); // check if need to show ball shot path
	check_flyout(); // check if need to show ball fly off screen
	check_received_ball(); // check if ball is received
	check_bounce_back(); // check direction ball needs to bounce
	check_receiving_direction(); // check direction of received ball
}

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

/** Tinygl initilisation. */
void tiny_init(void)
{
	tinygl_init (LOOP_RATE);
    tinygl_font_set (&font3x5_1);
    tinygl_text_speed_set (MESSAGE_RATE);
    tinygl_text_mode_set (TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
    tinygl_text ("YOU WON :)");
}

/** Main function of program. */
int main(void)
{
    system_init();
    ir_uart_init();
    pio_init();
    tiny_init();

    task_t tasks[] = 
    {
        {.func = board, .period = TASK_RATE / DISPLAY_TASK_RATE},
        {.func = button_task, .period = TASK_RATE / BUTTON_TASK_RATE}
    };

    button_init();
    task_schedule(tasks, ARRAY_SIZE(tasks));
    return 0;
}
