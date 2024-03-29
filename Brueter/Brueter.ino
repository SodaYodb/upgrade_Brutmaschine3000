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

// ------------------------------------------------------------------------- SENSOR
#define DHT1_PIN 31 // Top
#define DHT2_PIN 33 // Bottom
#define DHTTYPE DHT22
DHT dht_top(DHT1_PIN, DHTTYPE);
DHT dht_bot(DHT2_PIN, DHTTYPE);
float hum1, hum2, temp1, temp2;

// ------------------------------------------------------------------------- COLOR
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
#define WARNING_Color RED      // Change Color for Error Msg here

// ------------------------------------------------------------------------- CLASS RELAIS
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

// ------------------------------------------------------------------------- CLASS TOUCH BUTTON
class TouchButton {
  private:
    int _x;
    int _y;
    int _width;
    int _height;
  public:
    TouchButton(){
      // some Error handling here
      }
    TouchButton(int x, int y, int width, int height) {
      _x = x;
      _y = y;
      _width = width;
      _height = height;}
  
    bool isPressed(int px, int py) {
      return (px >= _x && px <= _x + _width && py >= _y && py <= _y + _height);}
};

TouchButton btn_toggle(220, 0, 100, 50); // Size of top left Box
TouchButton btn_minus(0, 190, 60, 50); // Size of bottom left Box
TouchButton btn_plus(260, 190, 60, 50); // Size of bottom right Box

// some variables
static unsigned long sensor_p_millis = 0; // don't edit
static unsigned long touch_p_millis = 0; // don't edit
char* last_error = "";

float last_prov_temperature = 0.0; // don't delete
float last_prov_target = 0.0; // don't delete
int last_prov_humidity = 0; // don't delete

bool started = false;

// -------------------------------------------------------------------------
float temp_delta = 1.0; // delta for turn on or off the Fan
// -------------------------------------------------------------------------
float target_temp = 31.0; // Setup Target tempe you most use
// -------------------------------------------------------------------------
// var for PID
float kp = 0.5;  // Proportional gain
float ki = 0.2;  // Integral gain
float kd = 0.1;  // Derivative gain
float integral = 0.0;
float prev_error = 0.0;


// ------------------------------------------------------------------------- VOID SETUP
void setup() {
  // init the Relais // default = LOW
  Fan.init();
  Heat.init();
  Move.init();
  
  tft.reset(); // Reset Screen

  uint16_t identifier = tft.readID();
  if(identifier==0x0101)
  {identifier=0x9341;}
  else {identifier=0x9328;}
  tft.begin(identifier);
  build_gui(); // generate GUI
  // Prefill some values to have something to show
  update_temp(30.0); // DUMMY provide value for update it on screen
  update_hum(100.00); // DUMMY provide value for update it on screen
  update_target(target_temp);
  check_started(started);
} // ------------------------------------------------------------------------- END VOID SETUP

void build_gui(){
  tft.setRotation(3);  // 0-3
  tft.fillScreen(BACKG_Color); // Background Color for Display

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
} // ------------------------------------------------------------------------- END BUILD_GUI SETUP

void update_temp(float prov_temperature){
  tft.setCursor(15, 20); tft.setTextColor(BACKG_Color); tft.setTextSize(3);
  tft.print(last_prov_temperature,1);
  tft.setCursor(15, 20); tft.setTextColor(NONEDIT_Color);
  tft.print(prov_temperature,1);
  last_prov_temperature = prov_temperature;
  }

void update_hum(int prov_humidity){
  tft.setCursor(135, 20); tft.setTextColor(BACKG_Color); tft.setTextSize(3);
  tft.print(last_prov_humidity);
  tft.setCursor(135, 20); tft.setTextColor(NONEDIT_Color);
  tft.print(prov_humidity);
  last_prov_humidity = prov_humidity;
  }

void update_target(float prov_target){
  tft.setCursor(118, 212); tft.setTextColor(BACKG_Color); tft.setTextSize(3);
  tft.print(last_prov_target,1);
  tft.setCursor(118, 212); tft.setTextColor(EDITABLE_Color); tft.setTextSize(3);
  tft.print(prov_target,1);
  last_prov_target = prov_target;
  }
  
void check_started(bool is_started){
  tft.fillRect(227,13,87,21, BACKG_Color);
  tft.setCursor(227, 13); tft.setTextColor(EDITABLE_Color); tft.setTextSize(3);
  if(is_started){
    tft.print("Stop");}
  else {
    tft.print("Start");}}

void show_error(char* error_string){
   if (strcmp(last_error, error_string) == 0) {
    return;}
  tft.setCursor(5, 52); tft.setTextColor(BACKG_Color); tft.setTextSize(2);
  tft.print(last_error);
  tft.setCursor(5, 52); tft.setTextColor(WARNING_Color);
  tft.print(error_string);
  last_error = error_string;}

void loop() {
  unsigned long i_touch = 40; // interval for Touchscreen
  unsigned long i_sensor = 500; // interval Sensors

  // ------------------------------------------------------------------------- SENSORS
  if (millis() - sensor_p_millis >= i_sensor) {
    // get Sensor Values and Check if there is a NaN
    hum1 = dht_top.readHumidity();
    temp1 = dht_top.readTemperature();
    hum2 = dht_bot.readHumidity();
    temp2= dht_bot.readTemperature();
    bool read_error = false;
    if (isnan(hum1) || isnan(temp1)) {
      show_error("Top Sensor Failed");
      read_error = true;}
    if (isnan(hum2) || isnan(temp2)) {
      show_error("Bottom Sensor Failed");
      read_error = true;}
    // ------------------------------------------------------------------------- HEAT & FAN
    if (!read_error){
      float diff1 = temp1 - temp2;
      float diff2 = temp2 - temp1;
        if (diff1 > temp_delta || diff2 > temp_delta) {
          Fan.on();}
        else{
          Fan.off();}
    // update temp and hum with mean values
      update_temp((temp1 + temp2) / 2.0);
      update_hum((hum1 + hum2) / 2.0);}
    // simple PID 
      float error = target_temp - (temp1 + temp2) / 2.0;
      integral += error;
      float derivative = error - prev_error;
      float heat_value = kp * error + ki * integral + kd * derivative;
      if (heat_value > temp_delta || diff1 > temp_delta || diff2 > temp_delta) {
        Heat.on();} 
      else {
        Heat.off();}
      prev_error = error;
      
      sensor_p_millis = millis();
  }
  
  // ------------------------------------------------------------------------- TOUCH
  if (millis() - touch_p_millis >= i_touch) {
    TSPoint p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
      if (btn_plus.isPressed(p.y, p.x)) {
        target_temp = target_temp + 0.1;
        update_target(target_temp);}
      if (btn_minus.isPressed(p.y, p.x)) {
        target_temp = target_temp - 0.1;
        update_target(target_temp);}
      if (btn_toggle.isPressed(p.y, p.x)) {
        started = !started;
        check_started(started);
        // delay to prevent a fast toggle while just press Start-Stop
        delay(500);}
    };
    touch_p_millis = millis();
  }
  // Move on or off
  if (started == true){
    Move.on();}
  else {Move.off();}
} // ------------------------------------------------------------------------- END VOID LOOP
