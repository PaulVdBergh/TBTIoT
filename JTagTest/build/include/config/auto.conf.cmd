deps_config := \
	/home/paulvdbergh/esp/esp-idf/components/app_trace/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/aws_iot/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/bt/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/driver/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/esp32/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/ethernet/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/fatfs/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/freertos/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/heap/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/libsodium/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/log/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/lwip/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/mbedtls/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/openssl/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/pthread/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/spi_flash/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/spiffs/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/paulvdbergh/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/paulvdbergh/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/paulvdbergh/TBTIoT/JTagTest/main/Kconfig.projbuild \
	/home/paulvdbergh/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/paulvdbergh/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
