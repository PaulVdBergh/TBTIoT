//#include "freertos/FreeRTOS.h"
//#include "esp_wifi.h"
//#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
//#include "nvs_flash.h"
//#include "driver/gpio.h"

#include "DCCGen.h"

extern "C" void app_main(void)
{
	TBTIoT::DCCGen Generator(GPIO_NUM_18);
}

