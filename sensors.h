#ifndef _SENSORS_H_
# define _SENSORS_H_

#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"
#include "nrf_drv_gpiote.h"
#include "app_util_platform.h"
//#include <vector>



/*extern const int RESISTOR;
extern const int MAX_ADC_VAL;
extern const int  MAX_OUTPUT_VAL;
extern const int  DIVISION_CONSTANT; // MAX_OUTPUT_VAL/DIVISION_CONSTANT MUST BE BELOW uint16 (65535)

extern const int BUTT_ROW_SIZE;
extern const int BUTT_COL_SIZE;

extern const int  BACK_ROW_SIZE;
extern const int  BACK_COL_SIZE;
extern const int BACK_START_MUX;

//Global Circular Buffer for averaging sensor readings
extern const int CIRCULAR_BUFFER_SIZE;
extern const int NUM_BUTT_SENSORS;
extern const int NUM_BACK_SENSORS;
extern const int NUM_SIM_DATA;
extern const int NUM_MUX_OUTPUTS;
extern const int NUM_MUX_PINS;*/


//Simulate some fake pressure data for the two classes

    extern uint8_t simButt[5][18];
    extern uint8_t simBack[5][8];





void select_mux(uint8_t output);
void select_back_col(uint8_t output);
void select_butt_col(uint8_t output);

void sensors_init();
void saadc_init();


extern uint16_t buttBuffer[CIRCULAR_BUFFER_SIZE][9];

extern uint16_t backBuffer[CIRCULAR_BUFFER_SIZE][4];

extern uint32_t muxPins[NUM_MUX_PINS];
extern uint32_t muxEnablePin;

extern uint32_t backColPins[BACK_COL_SIZE];
extern uint32_t buttColPins[BUTT_COL_SIZE];


void get_butt_matrix(uint16_t outputmatrix[BUTT_ROW_SIZE*BUTT_COL_SIZE]);
void get_back_matrix(uint16_t outputmatrix[BACK_ROW_SIZE*BACK_COL_SIZE]);

#endif //_SENSORS_H_
