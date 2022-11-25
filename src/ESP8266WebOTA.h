#include <Arduino.h>
#include <ESP8266WebServer.h>


class ESP8266WebOTA {
	public:
		unsigned int port = 80;
		String userName = "";
		String password = "";
		String path = "";
		String filePathOverRide = "";


		int init(ESP8266WebServer* WebServer,const char* pathconst,const char* UserName,const String Password);
		int init(ESP8266WebServer* WebServer,const char* UserName,const String Password);
		int add_routes(ESP8266WebServer *WebServer, const char *path);



		
	private:
		bool init_has_run;
		long getFreeSketchSpace();
		String ip2string(IPAddress ip);
};

extern ESP8266WebOTA ESP8266webota;
