#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP32Servo.h>
#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
HX711 scale;
Servo s1, s2, s3;
LiquidCrystal_I2C lcd(0x3F, 16, 4);
FirebaseData fbdb;
const int sensor1 = 23;
const int sensor2 = 18;
const int sensor3 = 19;
const int pinservo1 = 25; 
const int pinservo2 = 26;
const int pinservo3 = 27;
const int DOUT = 32;
const int CLK = 33;
int gtcb1,gtcb2,gtcb3;
int demsp1=0,demsp2=0,demsp3=0;
int weight,temp=0;
int pre1=HIGH,pre2=HIGH,pre3=HIGH;
float calibration_factor = 1067.204;
#define WIFI_SSID     "Pepsiii 2Ghz"
#define WIFI_PASSWORD "nguyenlieukhai"
#define FIREBASE_HOST "https://fir-70cb1-default-rtdb.firebaseio.com//" //Thay bằng địa chỉ firebase của bạn
#define FIREBASE_AUTH "MSNahdKuX5262vBcGBKh2Nv89EBElgVHDIY1bDs1"   //Không dùng xác thực nên không đổi

void setup() {
  Serial.begin(115200);
  //set up servo
  s1.attach(pinservo1);  
  s1.write(180);
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
}

void sensor(){
  gtcb1 = digitalRead(sensor1);
  gtcb2 = digitalRead(sensor2);
  gtcb3 = digitalRead(sensor3);
}

void phanloaisanpham()
{
  if  (weight>0)
  { 
    s1.write(45);
    temp = weight;
    Serial.println("Can nang tạm: ");
    Serial.print(temp);
    if (temp <= 20)
    {
      s2.write(60);
      s3.write(0);
      delay(4000);
      s2.write(0);
    }
    else if (temp <=50)
    {
      s3.write(180);
      s2.write(0);
      temp = 0;
    }
    else                
    {
      s2.write(0);
      s3.write(0);
      temp = 0;
    }
  }
  else
  {
    s1.write(180);  
  }
}

void demsp()
{
  if (gtcb1==LOW && pre1==HIGH)
  {
    demsp1++;
    s2.write(0);
    delay(20);
  }
  pre1=gtcb1;
  
  if (gtcb2==LOW && pre2==HIGH)
  {
    demsp2++;
    s3.write(0);
    delay(20);
  }
  pre2=gtcb2;
  
  if (gtcb3==LOW && pre3==HIGH)
  {
    demsp3++;
    delay(20);
  }
  pre3=gtcb3;
  
}

void lcd_display()
{
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("ESP8266");
  lcd.setCursor(0,1);
  lcd.print("Weight: ");
  lcd.print(weight);
  lcd.print(" G ");
  lcd.setCursor(0,2);
  Serial.print("Can nang: ");
  Serial.print(weight);
  Serial.println(" G ");
  delay(100);
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
  sensor();
  phanloaisanpham();
  demsp();
  lcd_display();
  capnhatfb();
}
