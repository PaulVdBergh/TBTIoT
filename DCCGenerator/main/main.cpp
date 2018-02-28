#include "freertos/FreeRTOS.h"
//#include "esp_wifi.h"
#include "esp_system.h"
//#include "esp_event.h"
//#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "DCCGen.h"

#include "iostream"

//	to start openocd for ESP-WROVER-KIT :
//	$ cd ~/esp/openocd
//	$ bin/openocd -s share/openocd/scripts -f interface/ftdi/esp32_devkitj_v1.cfg -f board/esp32-wrover.cfg

extern "C" void app_main(void)
{
	nvs_flash_init();
	std::cout << "Starting DCC Generator\n";
	TBTIoT::DCCGen Generator;

	while(1)
	{
		sleep(1000);
	}
}

