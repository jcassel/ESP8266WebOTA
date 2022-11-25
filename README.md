# ESP8266WebOTA
Simple web based Over-the-Air (OTA) updates for ESP8265 and Esp8266 based projects

Code in this lib is based on and in some cases directly copied from Scottchief Bakers ESP-WebOTA. His version is likely much better and more versital than this one as I have really boiled it down to just what I need in my projects. His is more versital and can be used with ESP32s as well as 826x. 

It require that you create a web page file that gets stored in spiffs. (included in the examples)

It includes Authentication (Basic and Digest).
Allows for passing in the web server that way it is on the same port as the rest of your pages.  

