/** @file   stopwatch1.c
    @author M.P. Hayes
    @date   22 Sep 2011
*/

#include "system.h"
#include "button.h"
#include "task.h"
#include "navswitch.h"
#include "pio.h"
#include "pacer.h"



#define LED_PIO PIO_DEFINE (PORT_C, 2)
#define DISPLAY_TASK_RATE 250
#define BUTTON_TASK_RATE 100

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


static prev_column = 0;
uint8_t current_column = 0;
bool ball_shot = false;

static uint8_t bitmap[] =
{
    //0x30, 0x46, 0x40, 0x46, 0x30
    0x00, 0x00, 0x00, 0x00, 0b0011100

};


static void board(__unused__ void *data) {

    // pio_output_low (LEDMAT_COL5_PIO);
    // pio_output_low (LEDMAT_ROW3_PIO);
    // pio_output_low (LEDMAT_ROW4_PIO);
    // pio_output_low (LEDMAT_ROW5_PIO);

    //pacer_wait ();

    //display_column (bitmap[current_column], current_column);

    uint8_t row_pattern = bitmap[current_column];

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

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }

}

// static void display_column (uint8_t row_pattern, uint8_t current_column)
// {
//
//     /* TODO */
//     int current_row = 0;
//     pio_output_high(cols[prev_column]);
//
//     for (; current_row < 7; current_row++) {
//         if ((row_pattern >> current_row) & 1) {
//             pio_output_low(rows[current_row]);
//         } else {
//             pio_output_high(rows[current_row]);
//         }
//     }
//     pio_output_low(cols[current_column]);
//     prev_column = current_column;
//
// }

static uint8_t ballmap[] =
{
    //0x30, 0x46, 0x40, 0x46, 0x30
    0x00, 0x00, 0x00, 0b0001000, 0x00

};

static void ball(__unused__ void *data) {

    // pio_output_low (LEDMAT_COL5_PIO);
    // pio_output_low (LEDMAT_ROW3_PIO);
    // pio_output_low (LEDMAT_ROW4_PIO);
    // pio_output_low (LEDMAT_ROW5_PIO);

    //pacer_wait ();

    //display_column (bitmap[current_column], current_column);

    uint8_t row_pattern = ballmap[current_column];

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

    current_column++;

    if (current_column > (LEDMAT_COLS_NUM - 1))
    {
        current_column = 0;
    }

}



static void button_task_init (void)
{
    button_init ();
}


static void button_task (__unused__ void *data)
{
    //button_update ();
    navswitch_update ();

    if (navswitch_push_event_p (NAVSWITCH_SOUTH) && bitmap[4] < 112)
    {
      bitmap[4] = bitmap[4]*2;
      if(ball_shot == false){
          ballmap[3] = ballmap[3]*2;
      }
    }

    if (navswitch_push_event_p (NAVSWITCH_NORTH) && bitmap[4] > 7)
    {
      bitmap[4] = bitmap[4]/2;
      if(ball_shot == false){
          ballmap[3] = ballmap[3]/2;
      }
    }

    if (navswitch_push_event_p (NAVSWITCH_PUSH) && ball_shot == false)
    {

      shoot_ball();
      ballmap[3] = 0x00;
      ball_shot = true;
    }


}


void shoot_ball(void)
{
  int new_row = 2;
  int prev_row = 3;


  while(new_row >= 0){




      ballmap[new_row] = ballmap[prev_row];
      ballmap[prev_row] = 0x00;
      new_row--;
      prev_row--;


  }


}





int main (void)
{
    system_init ();
    ini();





    //
    // while (1)
    // {
    //   board();
    // }

    task_t tasks[] =
    {
        {.func = board, .period = TASK_RATE / 2000},
        {.func = ball, .period = TASK_RATE / 1250},
        {.func = button_task, .period = TASK_RATE / BUTTON_TASK_RATE}
    };

    system_init ();
    //board();
    button_task_init ();


    task_schedule (tasks, ARRAY_SIZE (tasks));
    return 0;
}

//
//
// #include "system.h"
// #include "button.h"
// #include "task.h"
//
// #include "tinygl.h"
// #include "../fonts/font3x5_1.h"
//
//
// /* Define polling rates in Hz.  */
// // #define BUTTON_TASK_RATE 100
// //
// // #define DISPLAY_TASK_RATE 250
// //
// // #define TIMER_TASK_RATE 100
//
// #define LED_PIO PIO_DEFINE (PORT_C, 2)
//
// // static bool run;
//
//
//
// static void ini(void) {
//     pio_config_set (LEDMAT_ROW1_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW2_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW3_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW4_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW5_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW6_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_ROW7_PIO, PIO_OUTPUT_HIGH);
//
//     pio_config_set (LEDMAT_COL1_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_COL2_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_COL3_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_COL4_PIO, PIO_OUTPUT_HIGH);
//     pio_config_set (LEDMAT_COL5_PIO, PIO_OUTPUT_HIGH);
// }
//
// static void board(void) {
//     ini();
//     pio_output_low (LEDMAT_COL5_PIO);
//     pio_output_low (LEDMAT_ROW3_PIO);
//     pio_output_low (LEDMAT_ROW4_PIO);
//     pio_output_low (LEDMAT_ROW5_PIO);
// }
//
//
// //
// // static void button_task_init (void)
// // {
// //     button_init ();
// // }
// //
// //
// // static void button_task (__unused__ void *data)
// // {
// //     button_update ();
// //
// //     if (button_push_event_p (BUTTON1))
// //     {
// //         run = !run;
// //     }
// // }
//
// //
// // static void display_task_init (void)
// // {
// //
// //     tinygl_init (DISPLAY_TASK_RATE);
// //     tinygl_font_set (&font3x5_1);
// //     tinygl_text_mode_set (TINYGL_TEXT_MODE_STEP);
// //     tinygl_text_dir_set (TINYGL_TEXT_DIR_ROTATE);
// // }
// //
//
// // static void display_task (__unused__ void *data)
// // {
// //     tinygl_update ();
// // }
// //
// //
// // static void timer_task_init (void)
// // {
// //     tinygl_text ("..");
// // }
// //
// //
// // static void timer_task (__unused__ void *data)
// // {
// //     static uint16_t time;
// //     char str[3];
// //
// //     if (!run)
// //     {
// //         time = 0;
// //         return;
// //     }
// //
// //     str[0] = ((time / 10) % 10) + '0';
// //     str[1] = (time % 10) + '0';
// //     str[2] = 0;
// //
// //     tinygl_text (str);
// //
// //     time++;
// // }
//
//
// int main (void)
// {
//     system_init ();
//
//
//     while (1)
//     {
//     board();
//     }
    // task_t tasks[] =
    // {
    //     {.func = display_task, .period = TASK_RATE / DISPLAY_TASK_RATE},
    //     {.func = button_task, .period = TASK_RATE / BUTTON_TASK_RATE},
    //     {.func = timer_task, .period = TASK_RATE / TIMER_TASK_RATE},
    // };
    //
    // system_init ();
    //
    // display_task_init ();
    // button_task_init ();
    // timer_task_init ();
    //
    // task_schedule (tasks, ARRAY_SIZE (tasks));
    // return 0;
// }
