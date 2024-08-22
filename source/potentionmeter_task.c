/*
 * potentiometer.c
 *
 *  Created on: Feb 14, 2023
 *      Author: nipon
 */


#include "potentionmeter_task.h"


QueueHandle_t xQueuePoten = NULL;

static cyhal_adc_t adc_obj;
static cyhal_adc_channel_t adc_chan_0_obj;
static int adc_out;

static void potentionmeter_init(){
	cy_rslt_t result;

	/* ADC conversion result. */


	/* Initialize ADC. The ADC block which can connect to pin 10[6] is selected */
	result = cyhal_adc_init(&adc_obj, P10_6, NULL);
	CY_ASSERT(CY_RSLT_SUCCESS == result);

	// ADC configuration structure
	const cyhal_adc_config_t ADCconfig ={
		.continuous_scanning = false,
		.resolution = 12,
		.average_count = 1,
		.average_mode_flags = 0,
		.ext_vref_mv = 0,
		.vneg = CYHAL_ADC_VNEG_VREF,
		.vref = CYHAL_ADC_REF_VDDA,
		.ext_vref = NC,
		.is_bypassed = false,
		.bypass_pin = NC
	};

	// Configure to use VDD as Vref
	result = cyhal_adc_configure(&adc_obj, &ADCconfig);
	CY_ASSERT(CY_RSLT_SUCCESS == result);

	/* Initialize ADC channel, allocate channel number 0 to pin 10[6] as this is the first channel initialized */
	const cyhal_adc_channel_config_t channel_config = { .enable_averaging = false, .min_acquisition_ns = 220, .enabled = true };
	result = cyhal_adc_channel_init_diff(&adc_chan_0_obj, &adc_obj, P10_6, CYHAL_ADC_VNEG, &channel_config);

}

void potentionmeter_task(void* param){

//	  uint32_t counter = 0;
	  /* Suppress warning for unused parameter */
	  (void) param;

	  xQueuePoten = xQueueCreate( 2, sizeof(uint16_t));

	  potentionmeter_init();

	  /* Repeatedly running part of the task */
	  for (;;) {

		  /* Read the ADC conversion result for corresponding ADC channel. Repeat as necessary. */
		  adc_out = cyhal_adc_read_uv(&adc_chan_0_obj);

//		  printf("ADC (24V) = %d\r\n", (int) ((adc_out*24)/3304866) );
		  /* For Serial Studio */
//		  printf("/*%ld,%d*/\r\n", counter++, (int) ((adc_out*24)/3304866) );


			uint16_t adc_24v = (uint16_t) ((adc_out*24)/3304866) ;
			if (xQueuePoten != NULL) {
				if (!xQueueSend(xQueuePoten, &adc_24v, pdMS_TO_TICKS(100))) {
					// puts("Failed to send item to queue within 100ms");
				}
			}


		  vTaskDelay(pdMS_TO_TICKS(1));
	  }


	  vTaskDelete(NULL);
}
