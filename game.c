/** @file   stopwatch1.c
    @author M.P. Hayes
    @date   22 Sep 2011
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


static prev_column = 0;
uint8_t current_column = 0;
bool ball_shot = false;
bool start_shot = false;
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
    0x00, 0x00, 0x00, 0b0001000, 0b0011100
};



int prev_shot_row = 3;
int new_shot_row = 2;
bool received = false;
static void board(__unused__ void *data) {

    display_column (bitmap[current_column], current_column);

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }
    
    if (ball_shot && bitmap[0] !=  0) {
		ir_uart_putc(bitmap[0]);
		bitmap[0] = 0x00;
	}
	
	if(ir_uart_read_ready_p()){
		bitmap[0] = ir_uart_getc();
		// received ball at col 0 so keep receiving ball
		received = true;

	}


}


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


static void button_task_init (void)
{
    button_init ();
}



int receive_go_to = 1;
int receive_prev = 0;
bool bounce = false;
static void button_task (__unused__ void *data)
{
    //button_update ();
    navswitch_update ();

    if (navswitch_push_event_p (NAVSWITCH_SOUTH) && bitmap[4] < 112)
    {
      bitmap[4] = bitmap[4]*2;
      if(ball_shot == false){
          bitmap[3] = bitmap[3]*2;
      }
    }

    if (navswitch_push_event_p (NAVSWITCH_NORTH) && bitmap[4] > 7)
    {
      bitmap[4] = bitmap[4]/2;
      if(ball_shot == false){
          bitmap[3] = bitmap[3]/2;
      }
    }

    if (ball_shot == false && navswitch_push_event_p (NAVSWITCH_PUSH))
    {
      start_shot = true;
      ball_shot = true;

    }

    if(ball_shot && new_shot_row >= 0){
        bitmap[new_shot_row] = bitmap[prev_shot_row];
        bitmap[prev_shot_row] = 0x00;
        prev_shot_row--;
        new_shot_row--;
    }
    
    if(received){
		bitmap[receive_go_to] = bitmap[receive_prev];
        bitmap[receive_prev] = 0x00;
        receive_prev++;
        receive_go_to++;
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
	}
	
	if(bounce){
		bitmap[new_shot_row] = bitmap[prev_shot_row];
        bitmap[prev_shot_row] = 0x00;
		prev_shot_row--;
        new_shot_row--;
        if (prev_shot_row == 0) {
			bounce = false;
		}
	}

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
