/*
	MicroFleet.h - Library for usage tracking of ESP8266 projects!
	Created by Connor Nishijima, Sebtember 27th 2017.
	Released under the GPLv3 license.
*/

#ifndef microfleet_h
#define microfleet_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

class MicroFleet{
	public:
		MicroFleet(char* host, char* sku, char* ver, uint16_t poll);
		void connect(const char* portal_ssid);
		void wifi_config_callback(void (*config_action)());
		void handle(uint8_t float_accuracy, float var1 = 0,float var2 = 0,float var3 = 0,float var4 = 0,float var5 = 0,float var6 = 0,float var7 = 0,float var8 = 0,float var9 = 0,float var10 = 0);
		void check_in();
		void enable();
		void disable();
		
	private:
		char*          fleet_host;
		const uint32_t fleet_chip_id = ESP.getChipId();
		char*          fleet_ver;
		char*          fleet_sku;
		uint16_t       fleet_poll_sec;
		uint32_t       last_check_in = 0;
		const uint16_t fleet_buffer_size = 200;
		int32_t        fleet_vars[10] = {0,0,0,0,0,0,0,0,0,0};
		bool           auto_check_in = true;
};

#endif
