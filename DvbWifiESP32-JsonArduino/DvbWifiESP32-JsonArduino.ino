#include <ArduinoJson.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <SPI.h>

#define TFT_CS     4
#define TFT_RST    16
#define TFT_DC     17 //A0

#define BACK_COLOR ST7735_BLACK

#define DELAY_MILLIS 10000
#define MAX_ERRORS 5
#define MAX_DIR_LENGTH 12
#define LINE_HEIGHT 13
#define CONTENT_START_POSITION 18

// For esp32 sda (mosi) = 23, sck (sclk) = 18
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

uint16_t TXT_COLOR = tft.Color565(255, 102, 0);
char directionPrintout[MAX_DIR_LENGTH + 1];
WiFiMulti wifiMulti;

/**
 * Connections Settings:
 * 
 * ssid: id of your wifi
 * pwd: wifi pwd
 * url: request url e.g. http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=conertplatz&vz=0&ort=Dresden&lim=5 
 */
const char* ssid = "";
const char* pwd = "";
const char* url = "http://widgets.vvo-online.de/abfahrtsmonitor/Abfahrten.do?hst=conertplatz&vz=0&ort=Dresden&lim=5";

void setup() {
  Serial.begin(9600);
  Serial.println();
  initLcd();
  wifiMulti.addAP(ssid, pwd);
}

void initLcd(){
  Serial.print("init lcd...");
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.fillScreen(BACK_COLOR);
  tft.setTextSize(1);
  tft.setTextColor(TXT_COLOR);
  tft.setTextWrap(false);
  //Rotates the screen 180 degrees (2 times)
  tft.setRotation(2);
  drawHeader();
  Serial.println("done");  
}

int failCounter = 0;
int statusCode = 0;

void loop() {
  if((wifiMulti.run() == WL_CONNECTED)) {
    Serial.print("getting data...");
    statusCode = getDataAndShowOnDisplay();
    if(statusCode == 0){
      Serial.println("done");
      failCounter = 0;
    }
    else{
      failCounter++;    
    }            
  }
  else{
    Serial.print("not connected");
    failCounter++;
  }

  if(failCounter > MAX_ERRORS){
    showError();    
  }
  
  delay(DELAY_MILLIS);
}

void showError(){
  Serial.print("connection failed multiple times");  
  tft.fillScreen(BACK_COLOR);
  tft.setCursor(1, CONTENT_START_POSITION);
  tft.print("Verbindungsproblem");
  failCounter = 0;      
}

// returns 1 if something went wrong, otherwise 0
int getDataAndShowOnDisplay(){
  // https://bblanchon.github.io/ArduinoJson/assistant/
  //[["12","Leutewitz","8"],["12","Striesen","14"],["12","Leutewitz","35"],["12","Striesen","44"],["12","Leutewitz","65"]]
  const size_t bufferSize = 5*JSON_ARRAY_SIZE(3) + JSON_ARRAY_SIZE(5) + 280;
  DynamicJsonBuffer jsonBuffer(bufferSize);
  
  JsonArray& data = jsonBuffer.parseArray(getData());  
  if(data.size() == 0){
    Serial.println("nothing received");
    return 1;
  }

  int row = CONTENT_START_POSITION;
  for(int i = 0; i < data.size(); i++){
    JsonArray& root_i = data[i];
    writeArival(root_i[0], root_i[1], root_i[2], row); 
    row += LINE_HEIGHT;   
  }
  
  tft.fillRect(0, row, 128, 160-row, BACK_COLOR);
  return 0;
}

String getData(){
  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  
  if(httpCode != 200){
    Serial.println("failed at get request");
    http.end();
    return "[]";    
  }
  String data = http.getString();
  http.end();
  return data;
} 

void writeArival(const char *line, const char *direct, const char *minutes, int row){
  //TODO only update the values which have changed
  //only deleting the current row will reduce flickering
  strncpy(directionPrintout, direct, MAX_DIR_LENGTH);
  tft.fillRect(0, row, 128, LINE_HEIGHT, BACK_COLOR);
  tft.setCursor(2, row);
  tft.print(line);   
  tft.setCursor(30, row);
  tft.print(directionPrintout);   
  tft.setCursor(113, row);
  tft.print(minutes);  
}

void drawHeader(){
  tft.setCursor(1, 1);
  tft.print("Linie Richtung    Min");  
}


