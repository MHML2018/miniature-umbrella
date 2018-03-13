#include "sensors.h"

/*const int RESISTOR = 46500;
const int MAX_ADC_VAL = 4096;
const int  MAX_OUTPUT_VAL = 500000;
const int  DIVISION_CONSTANT = 10; // MAX_OUTPUT_VAL/DIVISION_CONSTANT MUST BE BELOW uint16 (65535)

const int BUTT_ROW_SIZE = 3;
const int BUTT_COL_SIZE = 3;

const int  BACK_ROW_SIZE = 2;
const int  BACK_COL_SIZE = 2;
const int BACK_START_MUX = 3;

//Global Circular Buffer for averaging sensor readings
const int CIRCULAR_BUFFER_SIZE = 16;
const int NUM_BUTT_SENSORS = 9;
const int NUM_BACK_SENSORS = 4;
const int NUM_SIM_DATA = 5;
const int NUM_MUX_OUTPUTS = 5;
const int NUM_MUX_PINS = 3;*/









uint16_t buttBuffer[CIRCULAR_BUFFER_SIZE][9] = {0};

uint16_t backBuffer[CIRCULAR_BUFFER_SIZE][4] = {0};

uint32_t muxPins[NUM_MUX_PINS] = {13,12,11};

uint32_t backColPins[BACK_COL_SIZE] = {22,23}; 

uint32_t buttColPins[BUTT_COL_SIZE] = {14,15,16};

uint32_t muxEnablePin=24;



//Simulate some fake pressure data for the two classes

    uint8_t simButt[5][18]           = {{0x12, 0x34, 0x56, 0x78, 0x90, 0xCA,0xFE, 0xF0, 0x0D, 0xC0, 0xFF, 0xEE, 0xBE, 0xEF, 0x87, 0x65, 0x43, 0x21},\
                                    {0x12, 0x34, 0x56, 0x78, 0x90, 0xCA,0xFE, 0xF0, 0x0D, 0xC0, 0xFF, 0xEE, 0xBE, 0xEF, 0x87, 0x65, 0x43, 0x21},\
                            {0x12, 0x34, 0x56, 0x78, 0x90, 0xCA,0xFE, 0xF0, 0x0D, 0xC0, 0xFF, 0xEE, 0xBE, 0xEF, 0x87, 0x65, 0x43, 0x21},\
                            {0x12, 0x34, 0x56, 0x78, 0x90, 0xCA,0xFE, 0xF0, 0x0D, 0xC0, 0xFF, 0xEE, 0xBE, 0xEF, 0x87, 0x65, 0x43, 0x21},\
                            {0x12, 0x34, 0x56, 0x78, 0x90, 0xCA,0xFE, 0xF0, 0x0D, 0xC0, 0xFF, 0xEE, 0xBE, 0xEF, 0x87, 0x65, 0x43, 0x21}};\


    uint8_t simBack[5][8]           = {{0x12,0x34,0x56,0x78,0x90,0x00,0x13,0x37},\
                                        {0x12,0x34,0x56,0x78,0x90,0x00,0x13,0x37},\
                                        {0x12,0x34,0x56,0x78,0x90,0x00,0x13,0x37},\
                                        {0x12,0x34,0x56,0x78,0x90,0x00,0x13,0x37},\
                                        {0x12,0x34,0x56,0x78,0x90,0x00,0x13,0x37}};





void get_butt_matrix(uint16_t outputmatrix[BUTT_ROW_SIZE*BUTT_COL_SIZE]){
	nrf_saadc_value_t butt_row_reading[BUTT_ROW_SIZE][BUTT_COL_SIZE]={0};
	float butt_row_sum[BUTT_ROW_SIZE]={0};
	uint32_t large_output[BUTT_ROW_SIZE][BUTT_COL_SIZE]={0};
	
	//nrf_saadc_value_t result=0;
	
	for(int i=0; i<BUTT_ROW_SIZE; i++){
		select_mux(i);
		butt_row_sum[i]=0;
		for(int j=0; j<BUTT_COL_SIZE; j++){
			select_butt_col(j);
			nrf_drv_saadc_sample_convert(7, &butt_row_reading[i][j]);
			if(butt_row_reading[i][j]<0) butt_row_reading[i][j]=0;
			butt_row_sum[i]+=(float)butt_row_reading[i][j]/(float)MAX_ADC_VAL;
		}
	
		for(int j=0; j<BUTT_COL_SIZE; j++){
			large_output[i][j]=(1.0f - (float)butt_row_reading[i][j]/(float)MAX_ADC_VAL)*(float)RESISTOR/butt_row_sum[i];
			if(large_output[i][j]>MAX_OUTPUT_VAL){
				large_output[i][j]=MAX_OUTPUT_VAL;
			}
			outputmatrix[i*BUTT_COL_SIZE + j]=large_output[i][j]/DIVISION_CONSTANT;
		}
	}
}


void get_back_matrix(uint16_t outputmatrix[BACK_ROW_SIZE*BACK_COL_SIZE]){
	nrf_saadc_value_t back_row_reading[BACK_ROW_SIZE][BACK_COL_SIZE]={0};
	float back_row_sum[BACK_ROW_SIZE]={0};
	uint32_t large_output[BACK_ROW_SIZE][BACK_COL_SIZE]={0};
	
	//nrf_saadc_value_t result=0;
	
	for(int i=0; i<BACK_ROW_SIZE; i++){
		select_mux(BACK_START_MUX+i);
		back_row_sum[i]=0;
		for(int j=0; j<BACK_COL_SIZE; j++){
			select_back_col(j);
			nrf_drv_saadc_sample_convert(7, &back_row_reading[i][j]);
			if(back_row_reading[i][j]<0) back_row_reading[i][j]=0;
			back_row_sum[i]+=(float)back_row_reading[i][j]/(float)MAX_ADC_VAL;
		}
	
		for(int j=0; j<BACK_COL_SIZE; j++){
			large_output[i][j]=(1.0f - (float)back_row_reading[i][j]/(float)MAX_ADC_VAL)*(float)RESISTOR/back_row_sum[i];
			if(large_output[i][j]>MAX_OUTPUT_VAL){
				large_output[i][j]=MAX_OUTPUT_VAL;
			}
			outputmatrix[i*BACK_COL_SIZE +j]=large_output[i][j]/DIVISION_CONSTANT;
		}
	}
}

void null_function(nrf_drv_saadc_evt_t const * p_event){}

void saadc_init(void)
{
    ret_code_t err_code;
	
	    err_code = nrf_drv_ppi_init();
    APP_ERROR_CHECK(err_code);

	
    nrf_saadc_channel_config_t channel_config_7 =
            NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN7);	
    channel_config_7.acq_time = NRF_SAADC_ACQTIME_20US;			
	channel_config_7.gain = NRF_SAADC_GAIN1_4;
	channel_config_7.reference = NRF_SAADC_REFERENCE_VDD4; 
	
	nrf_drv_saadc_config_t saadc_config=NRF_DRV_SAADC_DEFAULT_CONFIG;
	
	saadc_config.resolution=NRF_SAADC_RESOLUTION_12BIT ;
	
	
    err_code = nrf_drv_saadc_init(&saadc_config, null_function);
    APP_ERROR_CHECK(err_code);

	    err_code = nrf_drv_saadc_channel_init(7, &channel_config_7);
    APP_ERROR_CHECK(err_code);

}

void select_mux(uint8_t output){
    if(output>NUM_MUX_OUTPUTS){
        //Silently throw error
        return;
    }
    for(size_t i=0;i<NUM_MUX_PINS;i++){
        nrf_gpio_pin_write(muxPins[i], output & 1<<i);
    }
	
	
}

void select_back_col(uint8_t output){
    if(output>BACK_COL_SIZE){
        //Silently throw error
        return;
    }
    for(size_t i=0; i<BACK_COL_SIZE; i++){
        if(output == i){
            nrf_gpio_pin_set(backColPins[i]);
        }
        else{
            nrf_gpio_pin_clear(backColPins[i]);
        }
    }
    return;
}

void select_butt_col(uint8_t output){
    if(output>BACK_COL_SIZE){
        //Silently throw error
        return;
    }
    for(size_t i=0; i<BUTT_COL_SIZE; i++){
        if(output == i){
            nrf_gpio_pin_set(buttColPins[i]);
        }
        else{
            nrf_gpio_pin_clear(buttColPins[i]);
        }
    }
    return;
}

void sensors_init(){
    for(size_t i=0; i<NUM_MUX_PINS; i++){
        nrf_gpio_cfg_default(muxPins[i]);
        nrf_gpio_cfg_output(muxPins[i]);
    }

    //nrf_gpio_cfg_default(11);
    //nrf_gpio_cfg_output(11);
    //nrf_gpio_cfg_default(12);
    //nrf_gpio_cfg_default(12);
    //nrf_gpio_cfg_output(13);
    //nrf_gpio_cfg_output(13);
	
	nrf_gpio_cfg_default(muxEnablePin);
    nrf_gpio_cfg_output(muxEnablePin);
	nrf_gpio_pin_set(muxEnablePin);
    
    for(size_t i=0; i<BACK_COL_SIZE; i++){
        nrf_gpio_cfg_default(backColPins[i]);
        nrf_gpio_cfg_output(backColPins[i]);
    }


    //nrf_gpio_cfg_default(22);
    //nrf_gpio_cfg_default(22);
    //nrf_gpio_cfg_output(23);
    //nrf_gpio_cfg_output(23);

    for(size_t i=0; i<BUTT_COL_SIZE; i++){
        nrf_gpio_cfg_default(buttColPins[i]);
        nrf_gpio_cfg_output(buttColPins[i]);
    }
    
    //nrf_gpio_cfg_default(14);
    //nrf_gpio_cfg_output(14);
    //nrf_gpio_cfg_default(15);
    //nrf_gpio_cfg_default(15);
    //nrf_gpio_cfg_output(16);
    //nrf_gpio_cfg_output(16);

    saadc_init();
}
