// for display
#include <Elegoo_GFX.h>
#include <Elegoo_TFTLCD.h>
// for touch
#include <TouchScreen.h>
// for DHT22 Sensors
#include <DHT.h>

// define TFT pinout
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

// define touch pinout and stuff
#define YP A3
#define XM A2
#define YM 9
#define XP 8
#define TS_MINX 120
#define TS_MAXX 900
#define TS_MINY 70
#define TS_MAXY 920
#define MINPRESSURE 10
#define MAXPRESSURE 1000

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// define some colors
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define BACKG_Color BLACK      // Change Background Color here
#define EDITABLE_Color MAGENTA // Change Color for User editable Values here
#define NONEDIT_Color CYAN     // Change Color for Non editable Values here
#define DECORATON_Color WHITE  // Change Color for Decorations here

// define sensors ------------------
#define DHT1_PIN 31 // Top
#define DHT2_PIN 33 // Bottom
#define DHTTYPE DHT22
DHT dht_top(DHT1_PIN, DHTTYPE);
DHT dht_bot(DHT2_PIN, DHTTYPE);
float hum1, hum2, temp1, temp2;

// define relais --------------------
class Relais{
  private:
    byte _pin;
   public:
    Relais(){
      // some Error handling here
      }
    Relais(byte pin){
      _pin = pin;}
    void init(){
      pinMode(_pin, OUTPUT);
      off();}
    void off(){
      digitalWrite(_pin, LOW);}
    void on() {
      digitalWrite(_pin, HIGH);}
  };
// PinNumber by consturct should be better for understand what pins for what
Relais Fan(49);
Relais Heat(51);
Relais Move(53);

// some variables
bool started = false;


// ------------------------------------------------------------------
void setup() {
  // init the Relais // default = LOW
  Fan.init();
  Heat.init();
  Move.init();
  
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
  update_humd(100.00); // DUMMY provide value for update it on screen
  update_target(32.0); // DUMMY provide value for update it on screen
  check_started(started);
  
} // END VOID SETUP --------------------------------------------------

void build_gui(){
  // Display Size 320 widht in Rotation 3
  // Display Size 240 hight in Rotation 3
  tft.setRotation(3);  // 0-3
  tft.fillScreen(BLACK); // Background Color for Display

  // Top Section
  tft.drawLine(0, 50, 320, 50, DECORATON_Color);
  tft.drawLine(100, 0, 100, 50, DECORATON_Color);
  tft.drawLine(220, 0, 220, 50, DECORATON_Color);
  // Static Content Top Section
  tft.setCursor(5, 0);
  tft.setTextColor(DECORATON_Color);
  tft.setTextSize(2);
  tft.print("TEMP:");
  tft.setCursor(105, 0);
  tft.setTextColor(DECORATON_Color);
  tft.setTextSize(2);
  tft.print("HUMIDITY:");

  // Mid Section
  

  // Bottom Section
  tft.drawLine(0, 190, 320, 190, DECORATON_Color);
  tft.drawLine(60, 190, 60, 240, DECORATON_Color);
  tft.drawLine(260, 190, 260, 240, DECORATON_Color);
  // Static Content Bot Section
  tft.setCursor(11, 192); tft.setTextColor(EDITABLE_Color); tft.setTextSize(7);
  tft.print("-");
  tft.setCursor(275, 192);
  tft.print("+");
  tft.setCursor(65, 192); tft.setTextColor(DECORATON_Color); tft.setTextSize(2);
  tft.print("TARGET:");

  
  
} // END BUILD_GUI SETUP ---------------------------------------------

// would it be more performant to save the old Value and write it in Black before write the new Value?
void update_temp(float prov_temperature){
  tft.fillRect(15,20,69,21, BACKG_Color);
  tft.setCursor(15, 20); tft.setTextColor(NONEDIT_Color); tft.setTextSize(3);
  tft.print(prov_temperature,1);
}
void update_humd(int prov_humidity){
  tft.fillRect(135,20,51,21, BACKG_Color);
  tft.setCursor(135, 20); tft.setTextColor(NONEDIT_Color); tft.setTextSize(3);
  tft.print(prov_humidity);
}
void update_target(float prov_target){
  tft.fillRect(118,212,69,21, BACKG_Color);
  tft.setCursor(118, 212); tft.setTextColor(EDITABLE_Color); tft.setTextSize(3);
  tft.print(prov_target,1);
}
void check_started(bool is_started){
  tft.fillRect(227,13,87,21, BACKG_Color);
  tft.setCursor(227, 13); tft.setTextColor(EDITABLE_Color); tft.setTextSize(3);
  if(is_started == true){
    tft.print("Stop");
  }
  else{
    tft.print("Start");
  }
}

void loop() {
  // check temp. sensors, print Failure if one or both can't be read. Maybe print in Mid Section?
  // isnan  
  /*hum1 = dht_top.readHumidity();
  temp1 = dht_top.readTemperature();
  hum2 = dht_bot.readHumidity();
  temp2= dht_bot.readTemperature();*/

  // Control Relais by name with [Name].on() // [Name].off()
  //Fan.on();
  //Heat.on();
  //Move.on();
  
  // get touch info and scale it
  TSPoint p = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    // Zero is Top-Left Corner like the coordinates for drawing
    p.x = map(p.x, TS_MAXX, TS_MINX, tft.width(), 0); //cause of rotation 3 swapped MAXX and MINX
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    
    // just for check touch >> WORKS
    tft.fillRect(5,120,34,29, BLACK); // Fill Black then write otherwise Values never disapear
    tft.setCursor(5, 120); tft.setTextColor(GREEN); tft.setTextSize(2);
    tft.print(p.x);
    tft.setCursor(5, 135);
    tft.print(p.y);
  }
  delay(10); // just for testing, use millies() later?
  
  // if the values are too far apart = activate fan // if they are close enought stop fan
  // display mean temp. -- just update this Section of the Screen
  // display mean humd. -- just update this Section of the Screen
  // buttons for "Start" - "Stop" and temperature Correction Act Val, Arrow up and Down for 0,1°C
  // stirrer is continuously on
  // heating algorithm based on the set value
  
}
