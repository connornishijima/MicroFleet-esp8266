/*
	MicroFleet.cpp - Library for usage tracking of ESP8266 projects!
	Created by Connor Nishijima, Sebtember 27th 2017.
	Released under the GPLv3 license.
*/

#include "MicroFleet.h"
void (*config_func)();  // User-set function to call when WiFi configuration portal goes live

MicroFleet::MicroFleet(char* host, char* sku, char* ver, uint16_t poll){
	fleet_host = host;     // Example: "http://connor-n.com/"
	fleet_sku = sku;       // User's Product SKU
	fleet_ver = ver;       // User's Product Revision/Version
	fleet_poll_sec = poll; // How often should we check in?
}

// Called as often as possible, checks to see if we need to check in with MicroFleet web
// float_accuracy: how many decimal positions do we want for floats? 0 if all int
void MicroFleet::handle(uint8_t float_accuracy, float var1, float var2, float var3, float var4, float var5, float var6, float var7, float var8, float var9, float var10){
	if(auto_check_in == true){
		if (millis() >= last_check_in + (fleet_poll_sec * 1000)) {
			
			// we can't sprintf floats for sending.....ugh. We divide back on the web end...
			uint32_t push = 1;
			for(byte i = 0; i < float_accuracy; i++){
				push*=10;
			}
			
			// Update tracked variables
			fleet_vars[0] = var1*push;
			fleet_vars[1] = var2*push;
			fleet_vars[2] = var3*push;
			fleet_vars[3] = var4*push;
			fleet_vars[4] = var5*push;
			fleet_vars[5] = var6*push;
			fleet_vars[6] = var7*push;
			fleet_vars[7] = var8*push;
			fleet_vars[8] = var9*push;
			fleet_vars[9] = var10*push;
			
			check_in(); // Send request to web server
			
			last_check_in+=(fleet_poll_sec*1000);
		}
	}
}


// Enable checking in through handle():
void MicroFleet::enable(){
	auto_check_in = true;
}

// Disable checking in through handle():
void MicroFleet::disable(){
	auto_check_in = false;
}

// Let MicroFleet Web know we're online, and send tracked variables
void MicroFleet::check_in(){
	char fleet_request[fleet_buffer_size]; // char array to hold request url created by sprintf below
	sprintf(fleet_request,"%scheck.php?i=%lu&v=%s&s=%s&p=%d&var=%ld_%ld_%ld_%ld_%ld_%ld_%ld_%ld_%ld_%ld",fleet_host,fleet_chip_id,fleet_ver,fleet_sku,fleet_poll_sec,fleet_vars[0],fleet_vars[1],fleet_vars[2],fleet_vars[3],fleet_vars[4],fleet_vars[5],fleet_vars[6],fleet_vars[7],fleet_vars[8],fleet_vars[9]);

	if (WiFi.status() == WL_CONNECTED) { // If we're still connected
		HTTPClient http;
		http.begin(fleet_request);
		uint16_t httpCode = http.GET(); // Send check_in
		http.end(); // Don't need to care about response in this case!
	}
}

void MicroFleet::wifi_config_callback(void (*config_action)()){
	config_func = config_action; // Set config function to user's own
}

//                       Fuck this argument
void configCallback (WiFiManager *myWiFiManager) {
	config_func(); // Call user's own config_mode function from the dumbass WiFiManager one
}

void MicroFleet::connect(const char* portal_ssid){
	WiFiManager wifiManager; // Fuck you WiFiManager. You're a pain in the ass about configModeCallbacks and the WiFiManager *myWiFiManager thing.
	wifiManager.setAPCallback(configCallback); // Fuck you
	if(!wifiManager.autoConnect(portal_ssid)) { // Good, we're done here goodbye
		ESP.reset(); 
		delay(1000);
	}
}