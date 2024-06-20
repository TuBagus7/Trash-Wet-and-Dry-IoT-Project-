#define BLYNK_TEMPLATE_ID "TMPL65t8An6gw"
#define BLYNK_TEMPLATE_NAME "Smart Trash"
#define BLYNK_AUTH_TOKEN "5OpxjJg9CdAnpMhxe8OI3o7nBuTF4YGM"

#include <Blynk.h>
#include <Wire.h>
#include <WiFi.h>
#include <Preferences.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

char auth[] = BLYNK_AUTH_TOKEN;
BlynkTimer timer;

Servo servo_cover;
Servo servo_kering;
Servo servo_basah;

const int ir = 0; 
const int rain = 34; 
const int echoPin_cover = 14;
const int trigPin_cover = 12;
const int echoPin_basah = 5; 
const int trigPin_basah = 4; 
const int echoPin_kering = 16; 
const int trigPin_kering = 17; 
const int servo_cover_pin = 27;
const int servo_basah_pin = 32;
const int servo_kering_pin = 33;

#define VPIN_basah V0
#define VPIN_kering V1

long duration, duration_basah, duration_kering;
float distanceCm, cm_basah, cm_kering;
int state;
int kondisi;

//void checkBlynkStatus()
//{
//
//  bool isconnected = Blynk.connected();
//  if (isconnected == false)
//  {
//    Serial.println("Blynk Not Connected");
//  }
//  if (isconnected == true)
//  {
//    Serial.println(" Blynk IoT Connected ");
//  }
//}

void basah() {
  servo_basah.write(0);
  delay(1000);
  servo_basah.write(90);
  delay(500);
}
void kering() {
  servo_kering.write(90);
  delay(1000);
  servo_kering.write(0);
  delay(500);
}
void setup() {
  Serial.begin(9600);
  pinMode(ir, INPUT);
  pinMode(trigPin_cover, OUTPUT);
  pinMode(echoPin_cover, INPUT);
  pinMode(trigPin_basah, OUTPUT);
  pinMode(echoPin_basah, INPUT);
  pinMode(trigPin_kering, OUTPUT);
  pinMode(echoPin_kering, INPUT);
  servo_cover.attach(servo_cover_pin);
  servo_basah.attach(servo_basah_pin);
  servo_kering.attach(servo_kering_pin);
  servo_cover.write(180);//posisi awal setelah di di jalankan
  servo_basah.write(90);
  servo_kering.write(0);

  // Blynk.config(auth);
  const char *ssid = "Redmi Note 10 Pro";
  const char *password = "1sampai8";
  Blynk.begin(auth, ssid, password, "Blynk.cloud", 80);
}

void loop() {
  Blynk.run();
//  checkBlynkStatus();
  // Clears the trigPin_cover
  digitalWrite(trigPin_cover, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_cover, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_cover, LOW);
  duration = pulseIn(echoPin_cover, HIGH);
  distanceCm = duration / 29.1 / 2;
  //-------------------------------------------
  // Clears the trigPin_basah
  digitalWrite(trigPin_basah, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_basah, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_basah, LOW);
  duration_basah = pulseIn(echoPin_basah, HIGH);
  cm_basah = duration_basah / 29.1 / 2;
  //-------------------------------------------
  // Clears the trigPin_kering
  digitalWrite(trigPin_kering, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin_kering, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_kering, LOW);
  duration_kering = pulseIn(echoPin_kering, HIGH);
  cm_kering = duration_kering / 29.1 / 2;
  //-------------------------------------------

  // Infra Red
  state = digitalRead(ir);
  //-------------------------------------------

  // Rain Drops
  kondisi = analogRead(rain);

  Serial.print("Jarak Cover: "); Serial.println(distanceCm);
  Serial.println(state);
  Serial.println(kondisi);
  Serial.print("Jarak Basah: "); Serial.println(cm_basah);
  Blynk.virtualWrite(VPIN_basah, cm_basah);
  Serial.print("Jarak Keiring: "); Serial.println(cm_kering);
  Blynk.virtualWrite(VPIN_kering, cm_kering);
//----------------------------------------------------------------------
  if (distanceCm <= 10) {// Jarak kurang dari 10 cm, buka tutup
    servo_cover.write(0);
    Serial.println("Buka COVER");
  } else {
    servo_cover.write(180); // Menutup pintu
    Serial.println("Tutup COVER");
  }
  //---------------------------------------------------------------------
  if (state == 0  && kondisi >= 4095 ) {
    kering();
    Serial.println("Kering");
  } else if (state == 0 && kondisi < 4095) {
    basah();
    Serial.println("Basah");
  }else if (state == 1 && kondisi < 4095) {
    Serial.println("Masukkan Sampah satu per satu");
    basah(); 
  }else {
    Serial.println("Error");
    Blynk.logEvent("error", "Masukkan Sampah Satu per Satu");
  }
  //---------------------------------------------------------------------

  
  if (cm_basah < 10 ) {
    Blynk.logEvent("basah", "Sampah Basah Hampir Penuh");
    Serial.println("Sampah basah hampir Penuh");
  } else if (cm_kering <10) {
    Blynk.logEvent("kering", "Sampah Kering Hampir Penuh");
    Serial.println("Sampah kering hampir Penuh");
  }else{
    Serial.println("Aman");
  }

  Serial.println("---------------------------------------");

 //delay(1000); // Delay untuk menghindari pembacaan yang terlalu cepat
}
