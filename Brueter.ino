// for display
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
// for DHT22 Sensors
#include <DHT.h>

// define TFT Pinout
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// define some Colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// define sensors
#define DHT1_PIN 31 // Top
#define DHT2_PIN 33 // Bottom
#define DHTTYPE DHT22

DHT dht_top(DHT1_PIN, DHTTYPE);
DHT dht_bot(DHT2_PIN, DHTTYPE);
float hum1, hum2, temp1, temp2;

// define relais
int relais[] = {49,51,53};

// some variables
bool started = false;

// ------------------------------------------------------------------
void setup() {
  // define pinMode for relais and turn all off at Start
  for (int i=0; i<3; i++){
    pinMode(relais[i], OUTPUT);
    digitalWrite(relais[i], LOW);
  }
  tft.reset(); // Clear Screen
  // find correct identifier
  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
  {
      identifier=0x9341;
  }
  else {
    identifier=0x9328;
  }
  tft.begin(identifier);
  build_gui(); // generate GUI
  update_temp(30.0); // DUMMY provide value for update it on screen
  update_humd(100); // DUMMY provide value for update it on screen
  update_target(32.0); // DUMMY provide value for update it on screen
  check_started(started);
  
} // END VOID SETUP --------------------------------------------------

void build_gui(){
  // Display Size 320 widht in Rotation 3
  // Display Size 240 hight in Rotation 3
  tft.setRotation(3);  // 0-3
  tft.fillScreen(BLACK); // Background Color for Display

  // Top Section
  tft.drawLine(0, 50, 320, 50, WHITE);
  tft.drawLine(100, 0, 100, 50, WHITE);
  tft.drawLine(220, 0, 220, 50, WHITE);
  // Static Content Top Section
  tft.setCursor(5, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("TEMP:");
  tft.setCursor(105, 0);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("HUMIDITY:");

  // Mid Section
  

  // Bottom Section
  tft.drawLine(0, 190, 320, 190, WHITE);
  tft.drawLine(60, 190, 60, 240, WHITE);
  tft.drawLine(260, 190, 260, 240, WHITE);
  // Static Content Bot Section
  tft.setCursor(11, 192); tft.setTextColor(MAGENTA); tft.setTextSize(7);
  tft.print("-");
  tft.setCursor(275, 192);
  tft.print("+");
  tft.setCursor(65, 192); tft.setTextColor(WHITE); tft.setTextSize(2);
  tft.print("TARGET:");

  
  
} // END BUILD_GUI SETUP ---------------------------------------------

void update_temp(float prov_temperature){
  tft.setCursor(5, 20);
  tft.setTextColor(CYAN);
  tft.setTextSize(3);
  tft.println(prov_temperature);
}

void update_humd(int prov_humidity){
  tft.setCursor(140, 20);
  tft.setTextColor(CYAN);
  tft.setTextSize(3);
  tft.print(prov_humidity);
}

void update_target(float prov_target){
  tft.setCursor(118, 212);
  tft.setTextColor(MAGENTA);
  tft.setTextSize(3);
  tft.print(prov_target);
}

void check_started(bool is_started){
  tft.setCursor(227, 13); tft.setTextColor(MAGENTA); tft.setTextSize(3);
  if(is_started == true){
    tft.print("Stop");
  }
  else{
    tft.print("Start");
  }
}

void loop() {
  // check temp. sensors, print Failure if one or both can't be read
  hum1 = dht_top.readHumidity();
  temp1 = dht_top.readTemperature();
  hum2 = dht_bot.readHumidity();
  temp2= dht_bot.readTemperature();
  
  // if the values are too far apart = activate fan // if they are close enought stop fan
  // display mean temp. -- just update this Section of the Screen
  // display mean humd. -- just update this Section of the Screen
  // buttons for "Auto" - "Stop" and temperature Correction Act Val, Arrow up and Down for 0,1°C
  // stirrer is continuously on
  // heating algorithm based on the set value
  
}
