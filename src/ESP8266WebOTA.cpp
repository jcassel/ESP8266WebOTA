//#define _debugOTA //uncomment to get some serial debug messages.
#define  VERSION "1.0.2"
#include "ESP8266WebOTA.h"
#include <Arduino.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>


ESP8266WebServer* server;
ESP8266WebOTA ESP8266webota;


String ESP8266WebOTA::ip2string(IPAddress ip) {
	return String(ip[0]) + "." +  String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
}



int ESP8266WebOTA::init(ESP8266WebServer* WebServer,const char* path,const char* UserName,const String Password) {
	server=WebServer;
	this->path = path;
	this->port = port;
	this->userName = UserName;
	this->password = Password; 
	
	// Only run this once
	if (this->init_has_run) {
		return 0;
	}

	add_routes(server, path);
	#ifdef _debugOTA
	Serial.print("EOTA url   : http://");
	Serial.print(WiFi.localIP());
	if(port != 80){
		Serial.printf(":%d%s\r\n\r\n",port, path);
	}else{
		Serial.println(path);
	}
	#endif 
	// Store that init has already run
	this->init_has_run = true;

	return 1;
}

// WebServer param
int ESP8266WebOTA::init(ESP8266WebServer* WebServer,const char* UserName,const String Password) {
	return ESP8266WebOTA::init(WebServer,"/update",UserName,Password);
}	



long ESP8266WebOTA::getFreeSketchSpace() {
	return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
}



int ESP8266WebOTA::add_routes(ESP8266WebServer *WebServer, const char *path) {

// Upload firmware page
WebServer->on(path, HTTP_GET, [WebServer,this,path]() {
	if(!WebServer->authenticate(userName.c_str(),password.c_str())){ 
		#ifdef USE_DIGESTAUTH
			String authFailResponse = "";
			WebServer->requestAuthentication(DIGEST_AUTH, "OTARealm", authFailResponse);
			Serial.printf(authFailResponse);
		#else
			WebServer->requestAuthentication();
		#endif
	}
	
	if(filePathOverRide.length() == 0){
		if (SPIFFS.exists(path)) {
			File file = SPIFFS.open(path, "r");
			WebServer->streamFile(file, "text/html");
		}
	}else{
		if (SPIFFS.exists(filePathOverRide)) {
			File file = SPIFFS.open(filePathOverRide, "r");
			WebServer->streamFile(file, "text/html");
		}
	}
	
	
	WebServer->send_P(200, "text/html", "OTA page source does not exist, Possible corupt file system" );
			
	
});


// Handling uploading firmware file
WebServer->on(path, HTTP_POST, [WebServer,this]() {
	WebServer->send(200, "text/plain", (Update.hasError()) ? "Update: fail\n" : "Update: OK!\n");
	delay(500);
	ESP.restart();
	}, [WebServer,this]() {
		HTTPUpload& upload = WebServer->upload();

		if (upload.status == UPLOAD_FILE_START) {
			Serial.printf("Firmware update initiated: %s\r\n", upload.filename.c_str());

			uint32_t maxSketchSpace = this->getFreeSketchSpace();

			if (!Update.begin(maxSketchSpace)) { //start with max available size
				Update.printError(Serial);
			}
		} else if (upload.status == UPLOAD_FILE_WRITE) {
			/* flashing firmware to ESP*/
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
				Update.printError(Serial);
			}

			// Store the next milestone to output
			uint16_t chunk_size  = 51200;
			static uint32_t next = 51200;

			// Check if we need to output a milestone (100k 200k 300k)
			if (upload.totalSize >= next) {
				Serial.printf("%dk ", next / 1024);
				next += chunk_size;
			}
		} else if (upload.status == UPLOAD_FILE_END) {
			if (Update.end(true)) { //true to set the size to the current progress
				Serial.printf("\r\nFirmware update successful: %u bytes\r\nRebooting...\r\n", upload.totalSize);
			} else {
				Update.printError(Serial);
			}
		}
	});
	return 1;
}





