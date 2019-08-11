

   
void handleRoot() {
  const char *stat = "root";
  server.send(200, "te.t/html", strcat(ui, stat));
}

void handleStats() {
  const char *stat = "handle Stats" ;
server.send(200, "text/html", strcat(ui,stat));
}

void handleZap() {
  state="zap";
  //const char *stat = "fuck this shit" & "fuck you" ;
  server.send(200, "text/html", strcat(ui,stat));
}

void handleOn() {
  colorWipe(strip.Color(5,5,5),0);
  const char *stat = "LED On";
  server.send(200, "text/html", strcat(ui,stat));
}

void handleOff() {
  Serial.println();
  Serial.print("Args:");
  Serial.print(server.arg(0));
  
  Serial.print(server.arg(1));
  
  Serial.print(server.arg(2));
  
  Serial.print(server.arg(3));
  
  state="off";
  colorWipe(strip.Color(0,0,0),0);
  const char *stat = "LED Off";
  server.send(200, "text/html", strcat(ui,stat));

}


void handleStateFire() {
  state="fire";
  Serial.println();
  Serial.print("State changed to Fire");
   const char *stat = "State: Fire";
  server.send(200, "text/html", strcat(ui,stat));

}

void setBrightnessUp() {
  strip.setBrightness(strip.getBrightness() + 10);
  Serial.println();
  Serial.print("Brightness Set: " + strip.getBrightness());
   const char *stat = "Brightness Up";
  server.send(200, "text/html", strcat(ui,stat));

}

void setBrightnessDown() {
  strip.setBrightness(strip.getBrightness() - 10);
  Serial.println();
  Serial.print("Brightness Set: " + strip.getBrightness());
   const char *stat = "Brightness Off";
  server.send(200, "text/html", strcat(ui,stat));

}

void handleGreen () {
  state = "off";
  colorWipe(strip.Color(0,155,0),0);
   const char *stat = "Solid Green";
  server.send(200, "text/html", strcat(ui,stat));

}
void handleRed () {
  state = "off";
  colorWipe(strip.Color(155,0,0),0);
   const char *stat = "Solid Red";
  server.send(200, "text/html", strcat(ui,stat));

}void handleBlue () {
  state = "off";
  colorWipe(strip.Color(0,0,155),0);
   const char *stat = "Solid Blue";
  server.send(200, "text/html", strcat(ui,stat));
}

void handleWhite () {
  state = "off";
  colorWipe(strip.Color(155,155,155),0);
   const char *stat = "Solid White";
  server.send(200, "text/html", strcat(ui,stat));

}

void handleHazzard () {
  state = "hazzard"; 
  const char *stat = "State: Hazzard";
  server.send(200, "text/html", strcat(ui,stat));

}

void handleRedHazzard () {
  state = "redhazzard";
   const char *stat = "State: Hazzard (Red)";
  server.send(200, "text/html", strcat(ui,stat));

}

void handlePolice () {
  state = "police";
   const char *stat = "State: Police";
  server.send(200, "text/html", strcat(ui,stat));

}

void handleRainbow () {
  state = "rainbow";
  //stat = "State: Rainbow";
  server.send(200, "text/html", strcat(ui,stat));
}

void handleSetColor () {
  state = "off";
  Serial.println("Color Set:");

  int r = server.arg(0).toInt();
  int g = server.arg(1).toInt();
  int b = server.arg(2).toInt();
  Serial.print(r);
  Serial.print(g);
  Serial.print(b);
  uint32_t c = strip.Color(r,g,b);
  colorWipe(c,0);
  server.send(200, "text/html", strcat(ui,stat));
}

void handleStrobe(){
  if (server.argName(0) == "rate"){
     stroberate = server.arg(0).toInt();
    Serial.println("STROBY");
    Serial.println(server.arg(0).toInt());
    Serial.println(server.arg(1).toInt());
  }
  state="strobe";
  stat = "State: Strobe";
  server.send(200, "text/html", strcat(ui,stat));

}

void handleStripBrightness(){
  if (server.argName(0) == "brightness"){
     stroberate = server.arg(0).toInt();
    Serial.println("Brightness Set");
    Serial.println(server.arg(0).toInt());
    Serial.println(server.arg(1).toInt());
  }
  //state="strobe";
  int b = server.arg(1).toInt();
  if(b > 100){b = 100;}  //limitations on brightness
  strip.setBrightness(b);
  stat = "State: Brightness Set ";
  
  
  server.send(200, "text/html", strcat(ui,stat));

}

void handleAutoLightningStrike(){
    state="autolightning" ;
}

void handleSingleLightningStrike(){
    #define BETWEEN 2579
    #define DURATION 43 
    #define TIMES 7
    uint32_t colors[] = {};
    
    for (int i = 0; i < strip.numPixels(); i++){
      colors[i] =strip.getPixelColor(i);
    }
    
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

    for (int i = 0; i < strip.numPixels(); i++){
      strip.setPixelColor(i, colors[i]);
    }
    strip.clear();
    strip.show();
    
    
  server.send(200, "text/html", strcat(ui,stat));
}





