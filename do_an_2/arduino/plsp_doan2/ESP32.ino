#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 16, 4);
HX711 scale;
Servo s1, s2, s3;
FirebaseData fbdb;

const int sensor1 = 23;
const int sensor2 = 18;
const int pinservo1 = 25; 
const int pinservo2 = 26;
const int pinservo3 = 27;
const int DOUT = 32;
const int CLK = 33;
int queue1 = 0, queue2 = 0;
int demsp1 = 0,demsp2 = 0,demsp3 = 0;
int weight;
float calibration_factor = 964.8564;

#define WIFI_SSID     "Pepsiii 2Ghz"
#define WIFI_PASSWORD "nguyenlieukhai"
#define FIREBASE_HOST "https://doan2-af66d-default-rtdb.firebaseio.com/" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "tvgCN2YAYfYlU7cXzaVDXw74PpWdaPktNyf7Nbm3"   //Không dùng xác thực nên không đổi

void IRAM_ATTR isr()
{
  s2.write(0);
  s3.write(0);
  queue1 = 0;
}
void setup() {
  Serial.begin(115200);
  //set up cam bien
  pinMode(sensor1,INPUT);
  pinMode(sensor2,INPUT);
  //khoi tao ngat
  attachInterrupt(sensor1, isr, FALLING);
  //set up servo
  s1.attach(pinservo1);  
  s1.write(110);
  s2.attach(pinservo2);  
  s2.write(0);
  s3.attach(pinservo3);  
  s3.write(0);
  //set up loadcell
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); 
  long zero_factor = scale.read_average(); 
  // set up lcd & wifi
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.begin(16,4);
  lcd.setCursor(6,0);
  lcd.print("ESP32");
  lcd.setCursor(0,1);
  //set up wifi
  lcd.print("Ket noi wifi");
  Serial.println("Bat dau ket noi wifi");
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  while(!(WiFi.status() == WL_CONNECTED)){
    delay(500);
    Serial.println("...");
  }
  Serial.println("");
  Serial.println("Ket noi thanh cong!");
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("ESP32");
  lcd.setCursor(0,1);
  lcd.print("Da ket noi wifi!");
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
}

void loadcell()
{
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  weight = scale.get_units(5);
  if (weight < 0) weight = 0; 
  delay(1000);
}

void phanloaisanpham()
{
      if      (weight >= 95) 
      {
        queue1 = 1;
        while(queue1 == 1){
          s2.write(52);
          s1.write(65);
        }
        delay(100);
        demsp1++;
      } 
      else if (weight >= 45)  
      {
        queue1 = 2;
        while(queue1 == 2){
          s3.write(54);
          s1.write(65);
        }
        delay(100);
        demsp2++;
      }
      else    
      {
        if(weight>0)
        {
          s1.write(65);
          delay(100);
          demsp3++;
        }
      }
  if      (weight == 0)  s1.write(110);
}

void lcd_display()
{
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("ESP32");
  lcd.setCursor(0,1);
  lcd.print("Weight: ");
  lcd.print(weight);
  lcd.print(" G ");
  lcd.setCursor(0,2);
}

void capnhatfb()
{
  Firebase.setInt(fbdb,"plsp/weight",weight);
  Firebase.setInt(fbdb,"plsp/soluongsp1",demsp1);
  Firebase.setInt(fbdb,"plsp/soluongsp2",demsp2);
  Firebase.setInt(fbdb,"plsp/soluongsp3",demsp3);
}

void loop() {
  loadcell();
  lcd_display();
  capnhatfb();
  phanloaisanpham();
}
