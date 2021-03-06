#include "OTE.hpp"
#include <Logger.h>
#include <PrivateConfig.h>

#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <SSD1306.h>

extern SSD1306Wire display;

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

void InitializeOTE()
{
  // In this case it is not OTA it's OTE "Over the Ethernet" ;-)
  //LOGM( "MDNS begin" );
  MDNS.begin( OTA_HOSTNAME );
  // LOGM( "MDNS enableWorkstation" );
  MDNS.enableWorkstation( ESP_IF_ETH ); // Default is WiFi Station Interface

  // LOGM( "MDNS setHostname" );
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  // LOGM( "MDNS setPasswordHash" );
  ArduinoOTA.setPasswordHash(OTA_PASS_HASH);

  ArduinoOTA
  .onStart([]()
  {
    String type;

    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else       // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    LOG();
    Serial.println("Start updating " + type);

    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2 - 16, "OTA Update");
    display.display();
  } )

  .onEnd([]()
  {
    Serial.println();
    LOGM("End\n\n");
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(DISPLAY_WIDTH/2, DISPLAY_HEIGHT/2, "Restart");
    display.display();
  } )

  .onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
    display.display();
  } )

  .onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);

    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else
    if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else
    if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else
    if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else
    if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  } );


  LOGM( "ArduinoOTA.begin" );
  ArduinoOTA.begin();
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_name", APP_NAME );
  MDNS.addServiceTxt("_arduino", "_tcp", "fw_version", APP_VERSION );
  LOGM( "OTE initialized");
}
