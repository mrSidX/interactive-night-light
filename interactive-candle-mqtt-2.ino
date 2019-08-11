
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x20 for a 16 chars and 2 line display

#include <Arduino.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRac.h>
#include <IRutils.h>

// ==================== start of TUNEABLE PARAMETERS ====================
// An IR detector/demodulator is connected to GPIO pin 14
// e.g. D5 on a NodeMCU board.
const uint16_t kRecvPin = 13;

// The Serial connection baud rate.
// i.e. Status message will be sent to the PC at this baud rate.
// Try to avoid slow speeds like 9600, as you will miss messages and
// cause other problems. 115200 (or faster) is recommended.
// NOTE: Make sure you set your Serial Monitor to the same speed.
const uint32_t kBaudRate = 115200;

// As this program is a special purpose capture/decoder, let us use a larger
// than normal buffer so we can handle Air Conditioner remote codes.
const uint16_t kCaptureBufferSize = 1024;

// kTimeout is the Nr. of milli-Seconds of no-more-data before we consider a
// message ended.
// This parameter is an interesting trade-off. The longer the timeout, the more
// complex a message it can capture. e.g. Some device protocols will send
// multiple message packets in quick succession, like Air Conditioner remotes.
// Air Coniditioner protocols often have a considerable gap (20-40+ms) between
// packets.
// The downside of a large timeout value is a lot of less complex protocols
// send multiple messages when the remote's button is held down. The gap between
// them is often also around 20+ms. This can result in the raw data be 2-3+
// times larger than needed as it has captured 2-3+ messages in a single
// capture. Setting a low timeout value can resolve this.
// So, choosing the best kTimeout value for your use particular case is
// quite nuanced. Good luck and happy hunting.
// NOTE: Don't exceed kMaxTimeoutMs. Typically 130ms.
#if DECODE_AC
// Some A/C units have gaps in their protocols of ~40ms. e.g. Kelvinator
// A value this large may swallow repeats of some protocols
const uint8_t kTimeout = 50;
#else   // DECODE_AC
// Suits most messages, while not swallowing many repeats.
const uint8_t kTimeout = 15;
#endif  // DECODE_AC
// Alternatives:
// const uint8_t kTimeout = 90;
// Suits messages with big gaps like XMP-1 & some aircon units, but can
// accidentally swallow repeated messages in the rawData[] output.
//
// const uint8_t kTimeout = kMaxTimeoutMs;
// This will set it to our currently allowed maximum.
// Values this high are problematic because it is roughly the typical boundary
// where most messages repeat.
// e.g. It will stop decoding a message and start sending it to serial at
//      precisely the time when the next message is likely to be transmitted,
//      and may miss it.

// Set the smallest sized "UNKNOWN" message packets we actually care about.
// This value helps reduce the false-positive detection rate of IR background
// noise as real messages. The chances of background IR noise getting detected
// as a message increases with the length of the kTimeout value. (See above)
// The downside of setting this message too large is you can miss some valid
// short messages for protocols that this library doesn't yet decode.
//
// Set higher if you get lots of random short UNKNOWN messages when nothing
// should be sending a message.
// Set lower if you are sure your setup is working, but it doesn't see messages
// from your device. (e.g. Other IR remotes work.)
// NOTE: Set this value very high to effectively turn off UNKNOWN detection.
const uint16_t kMinUnknownSize = 12;
// ==================== end of TUNEABLE PARAMETERS ====================

// Use turn on the save buffer feature for more complete capture coverage.
IRrecv irrecv(kRecvPin, kCaptureBufferSize, kTimeout, true);
decode_results results;  // Somewhere to store the results


#define PIN 15

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
//#include "Adafruit_MQTT.h"
//#include "Adafruit_MQTT_Client.h"


#define MDNS_NAME  "lightproj1"
#define AIO_SERVER      "AIO.SERVER.ADDRESS.URL"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "candlelightmqtt"
#define AIO_KEY         "romanticmqtt"

/* Set these to your desired credentials. */
const char *ssid = "SomeWifiHotSpot";
const char *password = "WIFIPASSWORD";

char *state = "rainbow";
const char *stat = "";
int stroberate = 1000;
int rainbowspeed = 35;
int brightness = 3; 
bool lcd_light = true;
ESP8266WebServer server(80);
//Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);


const char *pixelArrayType = "feather4x8";

 char *ui = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" \
"<!DOCTYPE HTML><style>a {background-color: red;color: white;padding: 1em 1.5em;text-decoration: none;text-transform: uppercase;}</style><html><h3>states:</h3><br><p><a href=\"http://lightproj1.local/led/off\">OFF</a><p><br><a href=\"http://lightproj1.local/state/fire\">Fire Effect</a><p><br><a href=\"http://lightproj1.local/state/police\">Police (Red Blue) Effect</a><p><br><a href=\"http://lightproj1.local/state/hazzard\">Hazzard (Yellow) Effect</a><p><br><a href=\"http://lightproj1.local/state/rainbow\">Rainbow Effect</a><p><br><a href=\"http://lightproj1.local/state/strobe\">Strobe</a><p><br><a href=\"http://lightproj1.local/green\">Solid Green</a><p><br><a href=\"http://lightproj1.local/red\">Solid Red</a><p><br><a href=\"http://lightproj1.local/blue\">Solid Blue</a><p><br><a href=\"http://lightproj1.local/state/singleLightningStrike\">Single Lightning Strike</a><p><br>  </html>";



/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */



void checkStates(){

if (state == "fire") {
    delay(2);
    doFire(strip.Color(random(200),random(45),random(0)),60);
  }
  if (state == "hazzard") {
    delay(2);
    doHazzard(strip.Color(250, 250,0),500);
  }
  if (state == "redhazzard") {
    delay(2);
    doHazzard(strip.Color(200, 0,0),500);
  }
  if (state == "police") {
    delay(2);
    doPolice(4);
  }
  if (state == "rainbow") {
    delay(2);
    doRainbow();
  }
  if (state == "strobe") {
    delay(2);
    doStrobe(strip.Color(200,200,200),stroberate);
  }

  if (state == "autolightning") {
    delay(2);
    doAutoLightning();
  }

}


///////////SETUP//////////////////
void setup() {


#if defined(ESP8266)
  Serial.begin(kBaudRate, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(kBaudRate, SERIAL_8N1);
#endif  // ESP8266
  while (!Serial)  // Wait for the serial connection to be establised.
    delay(50);
  Serial.printf("\nIRrecvDumpV2 is now running and waiting for IR input on Pin "
                "%d\n", kRecvPin);
#if DECODE_HASH
  // Ignore messages with less than minimum on or off pulses.
  irrecv.setUnknownThreshold(kMinUnknownSize);
#endif                  // DECODE_HASH
  irrecv.enableIRIn();  // Start the receiver


  
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP("CandleLight", "romantic");
  //WiFi.softAPdisconnect (true);
  WiFi.begin(ssid, password);
  
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("MrSid's Interactive Night Light - A Work in Progress!");
  lcd.setCursor(5,4);
  lcd.print("2019");
  lcd.noBacklight();
  delay(3000);
  lcd.backlight();

  //IPAddress myIP = WiFi.softAPIP();
  IPAddress myIP = WiFi.localIP();

  Serial.print("AP IP address: ");
  
  
  Serial.println(myIP);
  Serial.println( MDNS_NAME);


  if ( MDNS.begin ( MDNS_NAME ) ) {
    Serial.println ( "MDNS responder started" );
  }
  
  server.begin();
  Serial.println("HTTP server started");
  
  strip.begin();
  colorWipe(strip.Color(10,55,10),0);
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(20);
  strip.show();
  delay(2000);
  strip.setBrightness(100);
  strip.show();
  delay(2000);
  strip.setBrightness(2);
  strip.show();
  delay(2000);
  strip.setBrightness(200);
  strip.show();
  //delay(2000);
  strip.setBrightness(3);
  strip.show();

//setup server
////////HANDLER FUNCTIONS/////////////////

  server.on("/", handleRoot);
  server.on("/led/on", handleOn);
  server.on("/led/off", handleOff);
  server.on("/state/fire", handleStateFire);
  server.on("/state/police", handlePolice);
  server.on("/state/hazzard", handleHazzard);
  server.on("/state/redhazzard", handleRedHazzard);
  server.on("/state/rainbow", handleRainbow);
  server.on("/green", handleGreen);
  server.on("/red", handleRed);
  server.on("/blue", handleBlue);
  server.on("/blue", handleWhite);
  server.on("/brightness/up", setBrightnessUp);
  server.on("/brightness/down", setBrightnessDown);
  server.on("/state/strobe", handleStrobe);
  server.on("/state/singleLightningStrike", handleSingleLightningStrike);
  server.on("/state/autoLightningStrike", handleAutoLightningStrike);
  server.on("/setcolor", handleSetColor);
  server.on("/stats", handleStats);
  server.on("/stripBrightness", handleStripBrightness);

  
}

void loop() {

 // Check if the IR code has been received.
  if (irrecv.decode(&results)) {
    // Display a crude timestamp.
    uint32_t now = millis();
    Serial.printf("Timestamp : %06u.%03u\n", now / 1000, now % 1000);
    // Check if we got an IR message tha was to big for our capture buffer.
    if (results.overflow)
      Serial.printf(
          "WARNING: IR code is too big for buffer (>= %d). "
          "This result shouldn't be trusted until this is resolved. "
          "Edit & increase kCaptureBufferSize.\n",
          kCaptureBufferSize);
    // Display the library version the message was captured with.
    Serial.println("Library   : v" _IRREMOTEESP8266_VERSION_ "\n");
    // Display the basic output of what we found.
    Serial.print(resultToHumanReadableBasic(&results));
    // Display any extra A/C info if we have it.
    String description = IRAcUtils::resultAcToString(&results);
    if (description.length()) Serial.println("Mesg Desc.: " + description);
    yield();  // Feed the WDT as the text output can take a while to print.
    // Output RAW timing info of the result.
    Serial.println(resultToTimingInfo(&results));
    yield();  // Feed the WDT (again)
    // Output the results as source code
    Serial.println(resultToSourceCode(&results));
    Serial.println();    // Blank line between entries
    yield();             // Feed the WDT (again)

    Serial.println(resultToHumanReadableBasic(&results));

    
  if (results.value == 0xFFA25D){
    if(strip.getBrightness()-20 < 0) {
        strip.setBrightness(0);
    }else{
        strip.setBrightness(strip.getBrightness()-20);
    }
  }
  
   if (results.value == 0xFF629D){
    if(strip.getBrightness()+20 > 100) {
        strip.setBrightness(100);
    }else{
        strip.setBrightness(strip.getBrightness()+ 20);
    }
  }

  //LCD LED OFF ON
  if (results.value == 0xFFE21D) {
      lcd_light = !(lcd_light);
      if(lcd_light == true){
        lcd.backlight();
      }else {
        lcd.noBacklight();
      }
      
  }
  
  }



//CHECK REMOTE CODES:::/ /////////////////



  
MDNS.update(); // Updates the MDNS


  

checkStates();
//  MQTT_connect();
//
//  // this is our 'wait for incoming subscription packets' busy subloop
//  // try to spend your time here
//
//  Adafruit_MQTT_Subscribe *subscription;
//  while ((subscription = mqtt.readSubscription(5000))) {
//    // Check if its the onoff button feed
//    if (subscription == &onoffbutton) {
//      Serial.print(F("On-Off button: "));
//      Serial.println((char *)onoffbutton.lastread);
//      
//      if (strcmp((char *)onoffbutton.lastread, "ON") == 0) {
//        digitalWrite(LED, LOW); 
//      }
//      if (strcmp((char *)onoffbutton.lastread, "OFF") == 0) {
//        digitalWrite(LED, HIGH); 
//      }
//    }
//    
//    // check if its the slider feed
//    if (subscription == &slider) {
//      Serial.print(F("Slider: "));
//      Serial.println((char *)slider.lastread);
//      uint16_t sliderval = atoi((char *)slider.lastread);  // convert to a number
//      analogWrite(PWMOUT, sliderval);
//    }
//  }
//
//  // ping the server to keep the mqtt connection alive
//  if(! mqtt.ping()) {
//    mqtt.disconnect();
//  }

  yield();

  server.handleClient();

//  irListen();

  server.handleClient();
  delay(2);
    

}

void doAutoLightning(){
    #define BETWEEN 2579
    #define DURATION 43 
    #define TIMES 7
    unsigned long lastTime = 0;
    int waitTime = 0;

    for (int i=0; i< random(TIMES); i++)
    {
      Serial.println(millis());
      //digitalWrite(LEDPIN, HIGH);
      colorWipe(strip.Color(255,255,255),0);
      delay(20 + random(DURATION));
      colorWipe(strip.Color(0,0,0),0);
      //digitalWrite(LEDPIN, LOW);
      delay(10);
    } 
}


void doRainbow(){
    delay(1);
    rainbow(rainbowspeed);  
}

void doPolice(int wait) {
  delay(10);
  colorWipe(strip.Color(200,0,0),wait); //Red
  colorWipe(strip.Color(0,0,0),wait); //Blank
  colorWipe(strip.Color(0,0,200),wait); //Blue
  colorWipe(strip.Color(0,0,0),wait); //Blank
  delay(10);
  colorWipe(strip.Color(200,0,0),wait); //Red
  colorWipe(strip.Color(0,0,0),wait); //Blank
  colorWipe(strip.Color(0,0,200),wait); //Blue
  colorWipe(strip.Color(0,0,0),wait); //Blank
  delay(10);
  colorWipe(strip.Color(200,0,0),wait); //Red
  colorWipe(strip.Color(0,0,0),wait); //Blank
  colorWipe(strip.Color(0,0,200),wait); //Blue
  colorWipe(strip.Color(0,0,0),wait); //Blank
}



void doHazzard (uint32_t c, uint8_t wait) {
  static bool stat = true ; 

  if(stat == true) {
    stat = false;
    delay(1);
    colorWipe(c,0);
  } else {
    stat = true;
    c= strip.Color(0,0,0);
    delay(1);
    colorWipe(c,0);
  }

  delay(wait);
}

void doFire(uint32_t c, uint8_t wait) {
    strip.setPixelColor(random(strip.numPixels()), c);
    c = strip.Color(0,0,0);
    delay(1);
    strip.show();
    
    strip.setPixelColor(random(strip.numPixels()), c);
    delay(1);
    strip.show();
    delay(wait);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  delay(1);
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}


uint32_t Wheel(byte WheelPos) {
  delay(1);
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbow(uint8_t wait) {
  delay(1);
  static uint16_t i, j;
    if(j > 256) {j = 0;}
    j++;
    if (i > strip.numPixels()){ i =0;}
      strip.setPixelColor(i, Wheel((i+j) & 255));
      i++;
    strip.show();
    delay(wait);
}

void doStrobe(uint32_t c, uint8_t wait){
  delay(1);
  colorWipe(strip.Color(0,0,0),0);
  strip.show();
  delay(wait);
  
  colorWipe(c,0);
  strip.show();
  delay(wait);
}











