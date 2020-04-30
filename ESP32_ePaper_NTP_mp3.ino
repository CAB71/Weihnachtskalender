// ----------------------------------------------------
// ------- Der sprechende Weihnachtskalender ----------
// ----------------------------------------------------

// Aufgebaut mit einem ESP32, DFPlayer (MP3 Microboard) und einem Waveshare 1.54" ePaper Display 
// In Anlehnung an Audio Advent Calendar (c) by Mischka 2011-2018 (mischka@mailbox.org) / https://mischk.neocities.org

// V0.1 12/2018


// -------------- ePaper Client Waveshare 1.54"  --------------------------------------
// mapping suggestion for ESP32 
// BUSY -> 4, RST -> 16, DC -> 17, CS -> SS(5), CLK -> SCK(18), DIN -> MOSI(23), GND -> GND, 3.3V -> 3.3V
// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h

// include library, include base class, make path known
	#include <GxEPD.h>

// select the display class to use, only one
	#include <GxGDEP015OC1/GxGDEP015OC1.h>    // 1.54" b/w

	#include <GxIO/GxIO_SPI/GxIO_SPI.h>
	#include <GxIO/GxIO.h>

// FreeFonts from Adafruit_GFX
	#include <Fonts/FreeMonoBold9pt7b.h>
	#include <Fonts/FreeSans9pt7b.h>
	#include <Fonts/FreeMonoBold12pt7b.h>
	#include <Fonts/FreeMonoBold18pt7b.h>
	#include <Fonts/FreeMonoBold24pt7b.h>
	
	// GxIO binden
	GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
	GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

// --------------------------------------------------------------------------------------

// ------------- NTP-Client -------------------
	#include <WiFi.h>
	#include <NTPClient.h>
	#include <WiFiUdp.h>

// Replace with your network credentials
	const char* ssid     = "SSID";
	const char* password = "Key";

// Define NTP Client to get time
	WiFiUDP ntpUDP;
	NTPClient timeClient(ntpUDP);

// Variables to save date and time
	String formattedDate;
	String dayStamp;
	String timeStamp;
	String timeStampkurz;

	String tag;
	String monat;
	String jahr;
	
	int nochtage;
  String displaynochtage;
	int int_tag;
	int int_monat;
	int int_jahr;
// -----------------------------------------------

// --------------- DFRobot MP3 Player --------------------

	#include <Arduino.h>
	#include "DFRobotDFPlayerMini.h"

	HardwareSerial mySoftwareSerial(1);
	DFRobotDFPlayerMini myDFPlayer;

	void printDetail(uint8_t type, int value);

// -----------------------------------------------


// ------- Deepsleep Pin setzen -----------

	static const int SensorPin = 33;

// ----------------------------------------


// --------- Anlage benutzte Grafiken --------------
const unsigned char Baum[1365] = { 0X00,0X01,0X45,0X00,0X97,0X00,
0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X30,0X00,0X00,0X00,0X00,0X1E,0X30,0X00,0X00,0X38,0X00,0X00,
0X00,0X00,0X3F,0X38,0X00,0X00,0X38,0X00,0X00,0X00,0X00,0X3B,0XB8,0X00,0X00,0X3B,
0XFE,0X1E,0X1F,0X1E,0X1B,0XB8,0X1E,0X38,0X3B,0XFF,0X3F,0X3F,0XBF,0X07,0XB8,0X3F,
0X78,0X3B,0XBB,0X9F,0XBB,0XBF,0X8F,0X38,0X39,0XF0,0X3B,0XBB,0X9F,0XBB,0XBF,0X9E,
0X38,0X38,0X70,0X3B,0XBB,0XBF,0XBB,0XB8,0X3C,0X38,0X38,0X70,0X3B,0XBB,0XBB,0XBB,
0XBB,0X38,0X38,0X3B,0X70,0X3B,0XFB,0X9F,0X9F,0X9F,0XBF,0X3F,0X9F,0XB8,0X19,0XF9,
0X8F,0X8F,0X8F,0X1F,0X9F,0XCF,0X18,0X00,0X30,0X00,0X33,0X80,0X00,0X00,0X00,0X00,
0X00,0X18,0X00,0X1F,0X80,0X00,0X00,0X00,0X00,0X00,0X0C,0X00,0X0F,0X00,0X00,0X00,
0X00,0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X80,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,0X00,
0X00,0X00,0X00,0XC0,0X07,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0X0F,0X80,0X00,
0X00,0X00,0X00,0X00,0X00,0XFC,0X3F,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X1F,0XFF,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X03,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X47,0X80,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XE0,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X78,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X1E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X07,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X01,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X78,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X38,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X03,0XC0,0X00,0X00,0X00,0X00,0X03,0XFE,0X00,0X01,0XE0,0X00,0X00,0X00,
0X00,0X1F,0XFF,0X80,0X01,0XE0,0X00,0X00,0X00,0X00,0X7F,0XFF,0XF0,0X03,0XF0,0X00,
0X00,0X00,0X00,0XFF,0X00,0X3F,0XFF,0XF0,0X00,0X00,0X00,0X00,0XF8,0X00,0X07,0XFF,
0XE0,0X00,0X00,0X00,0X00,0XE0,0X00,0X00,0XFF,0XC0,0X00,0X00,0X00,0X00,0X80,0X00,
0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X07,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X01,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X70,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,
0X00,0X00,0X00,0X00,0X00,0X3F,0X00,0XE0,0X00,0X00,0X00,0X80,0X00,0X00,0XFF,0XC0,
0X70,0X00,0X00,0X00,0XE0,0X00,0X00,0XF8,0X70,0X78,0X00,0X00,0X00,0X78,0X00,0X01,
0XF0,0X1F,0XFC,0X00,0X00,0X00,0X1F,0X00,0X01,0XE0,0X07,0XFC,0X00,0X00,0X00,0X03,
0XC0,0X01,0XE0,0X07,0XA0,0X00,0X00,0X00,0X00,0XF0,0X03,0XC0,0X03,0X80,0X00,0X00,
0X00,0X00,0X3E,0X03,0XC0,0X03,0X80,0X00,0X00,0X00,0X00,0X0F,0XCF,0XC0,0X01,0XC0,
0X00,0X00,0X00,0X00,0X03,0XFF,0X80,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X7F,0X00,
0X00,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X70,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X30,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X0E,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XE0,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X70,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X38,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X1C,0X00,0X00,0X00,0X00,0X00,0X3F,
0XE0,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0XFF,0XFC,0X00,0X07,0X00,0X00,0X00,0X00,
0X01,0XFF,0XFF,0X00,0X03,0X80,0X00,0XC0,0X00,0X07,0XF8,0X0F,0XC0,0X01,0XC0,0X00,
0XF0,0X00,0X0F,0XE0,0X01,0XF0,0X01,0XC0,0X00,0XFC,0X00,0X3F,0X80,0X00,0X3E,0X03,
0XE0,0X00,0X3F,0X00,0X7F,0X00,0X00,0X0F,0XFF,0XE0,0X00,0X0F,0XC1,0XFC,0X00,0X00,
0X03,0XFF,0XE0,0X00,0X03,0XFF,0XF8,0X00,0X00,0X01,0XFF,0XC0,0X00,0X00,0XFF,0XE0,
0X00,0X00,0X00,0XDF,0X00,0X00,0X00,0X3F,0X80,0X00,0X00,0X00,0X60,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X70,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X30,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X0C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0E,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X06,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X01,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XC0,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X60,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X30,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X0C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X06,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X03,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X01,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,
0XC0,0X00,0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0XE0,0X00,0X00,0X00,0X00,0X01,0XFF,
0XE0,0X00,0XF0,0X00,0X00,0X00,0X00,0X0F,0XFF,0XFC,0X01,0XF0,0X00,0X00,0X00,0X00,
0X1F,0XFF,0XFF,0XC3,0XF8,0X00,0X00,0X00,0X00,0X7F,0XF0,0X3F,0XFF,0XF8,0X0F,0XC0,
0X00,0X00,0XFF,0X80,0X03,0XFF,0XF0,0X7F,0XFE,0X00,0X03,0XFE,0X00,0X00,0X7F,0XF0,
0XFF,0XFF,0XE0,0X1F,0XF8,0X00,0X00,0X0F,0XC0,0XFF,0XFF,0XFF,0XFF,0XF0,0X00,0X00,
0X00,0X00,0XF8,0X0F,0XFF,0XFF,0XC0,0X00,0X00,0X00,0X00,0X80,0X00,0X7F,0XFF,0X00,
0X00,0X00,0X00,0X00,0X80,0X00,0X0F,0XF8,0X00,0X00,0X00,0X00,0X00,0X80,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,};









// -------------------------------------------------


void setup() {
	
	// ---- Allgemein -----------------------------------
   Serial.begin(115200);
    pinMode(SensorPin, INPUT_PULLDOWN);
	Serial.println("setup done");
   
   
   // ------ Display init -------------------------------
    display.init();  
		display.setRotation(0);
		display.fillRect(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_WHITE);
		display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false); 

		
	// ------ WiFi init ---------------------------------
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	
	// Print local IP address and start web server
	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	
	// ------- NTP Client -------------------------------
 
	timeClient.begin();
	// Set offset time in seconds to adjust for your timezone, for example:
	// GMT +1 = 3600
	// GMT +8 = 28800
	// GMT -1 = -3600
	// GMT 0 = 0
	timeClient.setTimeOffset(3600);
	
	
	// --------- MP3 Player ----------
	mySoftwareSerial.begin(9600, SERIAL_8N1, 27, 26);  // speed, type, RX, TX 18 19
	  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
		
		Serial.println(myDFPlayer.readType(),HEX);
		Serial.println(F("Unable to begin:"));
		Serial.println(F("1.Please recheck the connection!"));
		Serial.println(F("2.Please insert the SD card!"));
		while(true);
	  }
	Serial.println(F("DFPlayer Mini online."));
	
	// -------- Parameter setzen ---------
	  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
	  myDFPlayer.volume(20);  //Set volume value (0~30).
	  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);//----Set different EQ----
	  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);   //----Set device we use SD as default----

	//------- Read information and print to serial --------
//	  Serial.println(F("readState--------------------"));
//	  Serial.println(myDFPlayer.readState()); //read mp3 state
//	  
//	  Serial.println(F("readFileCounts--------------------"));
//	  Serial.println(myDFPlayer.readFileCounts()); //read all file counts in SD card
//	  Serial.println(F("readCurrentFileNumber--------------------"));

	  int delayms=100;
}

void loop() { 
// -------------------------------------
// ---------- Hauptprogramm ------------
// -------------------------------------
  
zeit(); // Startet die Zeitabfrage des NTP Client

xmas(); // Berechnet die Tage bis Weihnachten
Serial.println(myDFPlayer.readState());
  if (myDFPlayer.readState() != 513) {
        myDFPlayer.volume(20);  //Set volume value (0~30).
        Serial.println(int_tag);
        myDFPlayer.play(int_tag);  //Play the first mp3
        delay(500);
        //Serial.println(myDFPlayer.readCurrentFileNumber()); //read current play file number
  }

showDate();
//showBaum();





delay(60000);

}

void xmas(){
	// --------------------------------------------------------------------------------------
	// --- Wie lange ist es bis Weihnachten und welcher Track muss gespielt werden ----------
	// --------------------------------------------------------------------------------------
Serial.println("XMas wirds ausgeführt");
    if (int_tag < 32){ // Ist es in Tagen noch vor Weihnachten?
      if (int_monat == 12) {
       nochtage = 24 - int_tag;  
       Serial.print("Tage bis Weihnachten: ");
       Serial.println(nochtage); 
      }
      else{
        //Serial.println("Kein Advendstag");  
        nochtage = 31;
        //Serial.println(nochtage);
      }
    }
}

void zeit(){ 
	// -----------------------------
	// -------- NTP-Client ---------
	// -----------------------------
	
	while(!timeClient.update()) {
		timeClient.forceUpdate();
	}
	
	// The formattedDate comes with the following format:
	// 2018-05-28T16:00:13Z
	// We need to extract date and time
	formattedDate = timeClient.getFormattedDate();


	// Extract date
	int splitT = formattedDate.indexOf("T");
		dayStamp = formattedDate.substring(0, splitT);
		jahr = formattedDate.substring(0, 4);
		monat = formattedDate.substring(5, 7);
		tag = formattedDate.substring(8, 10);
		
		// Zum Rechnen muss ein Integer vorliegen, schreibe die String-Variablen in eine Integervariable
		int_tag = tag.toInt();
		int_monat = monat.toInt();
		int_jahr = jahr.toInt();
  
//		  Serial.println();
//		  Serial.print("DATE: ");
//		  Serial.println(dayStamp);
  
		// Extract time
		  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1); // HH:MM:SS 
		  timeStampkurz = formattedDate.substring(splitT+1, formattedDate.length()-4); // HH:MM 
		  Serial.print("HOUR: ");
		  Serial.println(timeStamp);
}

void showTime()
// ------------------------------------------------------
// ----- Schreibe aktuelle Uhrzeit auf den Screen -------
// ------------------------------------------------------

{
	  // Koordinaten der Textbox
	  uint16_t box_x = 10;
	  uint16_t box_y = 15;
	  
	  // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
	  uint16_t box_w = 140; //18pt
	  uint16_t box_h = 40; //18pt
	  uint16_t cursor_y = box_y + box_h - 12; //18pt

	  // Set Font/Color
	  display.setFont(&FreeMonoBold18pt7b); // &FreeMonoBold12pt7b  / &FreeMonoBold9pt7b
	  display.setTextColor(GxEPD_BLACK);
	  display.setRotation(0);
	  //display.update();

	  // partial update to full screen to preset for partial update of box window
	  // (this avoids strange background effects)
	  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);


	  // Schreibe Zeit in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      display.print(timeStampkurz); // Zeit in die Box schreiben
      
	  // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);
}

void showDate()
// ---------------------------------------------------------------------
// ----- Schreibe anzahl der Tage bis Weihnachten auf den Screen -------
// ---------------------------------------------------------------------

{
    // Koordinaten der Textbox
    uint16_t box_x = 30;
    uint16_t box_y = 15;
    
    // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
    uint16_t box_w = 150; //18pt
    uint16_t box_h = 27; //18pt
    uint16_t cursor_y = box_y + box_h - 10; //12pt

    // Set Font/Color
    display.setFont(&FreeMonoBold12pt7b); // &FreeMonoBold12pt7b  / &FreeMonoBold9pt7b / &FreeMonoBold18pt7b
    display.setTextColor(GxEPD_BLACK);
    display.setRotation(0);
    //display.update();

    // partial update to full screen to preset for partial update of box window
    // (this avoids strange background effects)
    display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);


    // Schreibe Datum in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      display.print(dayStamp); // Zeit in die Box schreiben
      
    // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);
      
    // ---- Nächste Textbox "noch"-----
    // Koordinaten der Textbox
    box_x = 75;
    box_y = 45;
    
    // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
    box_w = 62; //18pt
    box_h = 25; //18pt
    cursor_y = box_y + box_h - 9; //12pt

    // Schreibe Text in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      display.print("noch"); // Zeit in die Box schreiben
      
    // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);

   // ---- Nächste Textbox "x Tage"-----
    // Koordinaten der Textbox
    box_x = 30;
    box_y = 70;
    
    // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
    box_w = 140; //18pt
    box_h = 33; //18pt
    cursor_y = box_y + box_h - 9; //12pt
    display.setFont(&FreeMonoBold18pt7b); // &FreeMonoBold12pt7b  / &FreeMonoBold9pt7b / &FreeMonoBold18pt7b
    

    // Schreibe Text in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      displaynochtage = String(nochtage) + " Tage";
      Serial.println(nochtage);
      display.print(displaynochtage); // Zeit in die Box schreiben
      
    // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);

    // ---- Nächste Textbox "bis"-----
    // Koordinaten der Textbox
    box_x = 76;
    box_y = 110;
    
    // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
    box_w = 45; //18pt
    box_h = 25; //18pt
    cursor_y = box_y + box_h - 9; //12pt
    display.setFont(&FreeMonoBold12pt7b); // &FreeMonoBold12pt7b  / &FreeMonoBold9pt7b / &FreeMonoBold18pt7b   

    // Schreibe Text in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      display.print("bis"); // Zeit in die Box schreiben
      
    // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);

    // ---- Nächste Textbox "Weihnachten"-----
    // Koordinaten der Textbox
    box_x = 20;
    box_y = 140;
    
    // Größe der Textbox x/y/cursor (9pt: 70/20/-6 ; 12pt: 90/27/-10 ; 18pt: 140/40/-12)
    box_w = 160; //18pt
    box_h = 25; //18pt
    cursor_y = box_y + box_h - 9; //12pt
    display.setFont(&FreeMonoBold12pt7b); // &FreeMonoBold12pt7b  / &FreeMonoBold9pt7b / &FreeMonoBold18pt7b   

    // Schreibe Text in Cache 
      display.fillRect(box_x, box_y, box_w, box_h, GxEPD_WHITE); // Box mit weißem Hintergrund füllen
      display.setCursor(box_x, cursor_y); // Cursor in die Box setzen
      display.print("Weihnachten"); // Zeit in die Box schreiben
      
    // Schreibe Cache auf das Display
      display.updateWindow(box_x, box_y, box_w, box_h, true);
    
}

void showBaum(){
  // ---------------------------------------------------
  // ------ Zeichne Weihnachtsbaum aufs Display --------
  // ---------------------------------------------------
  
  display.fillScreen(GxEPD_WHITE);
  display.drawExampleBitmap(Baum, 0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, GxEPD_BLACK);
  display.update();
  delay(5000);
}
