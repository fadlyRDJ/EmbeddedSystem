#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

#define BLYNK_TEMPLATE_ID "TMPL6kDUVReng"
#define BLYNK_TEMPLATE_NAME "PROJECT IOT HOME CONTROLING"
#define BLYNK_AUTH_TOKEN "kBKhk7dY7vF1WLbWwokq1DtAvJG2m6Ss"

BlynkTimer timer;
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "";
char pass[] = "";

#define trig 23
#define echo 22
#define buzzer 15

BLYNK_WRITE(V4){
  int ledState = param.asInt();
  if(param.asInt() == 1)
  {
    digitalWrite(18,HIGH);
  }
  else
  {
    digitalWrite(18,LOW);
  }
}

BLYNK_WRITE(V5){
  int ledState = param.asInt();
  if(param.asInt() == 1)
  {
    digitalWrite(19,HIGH);
  }
  else
  {
    digitalWrite(19,LOW);
  }
}

BLYNK_WRITE(V6){
  int ledState = param.asInt();
  if(param.asInt() == 1)
  {
    digitalWrite(21,HIGH);
  }
  else
  {
    digitalWrite(21,LOW);
  }
}

void sendSensor(){
  //TMP
  double voltage = analogRead(2) /1223; // Read the voltage from the sensor
  double temperature = (voltage - 0.5) * 100; // Convert the voltage to temperature in degrees Celsius
  double TempC = temperature;
  double TempF = (temperature*1.8)+32; 
  Blynk.virtualWrite(V1, TempF);
  Blynk.virtualWrite(V2, TempC);
  Serial.println(voltage);
  Serial.println(TempF);

  // UltraSonic
  digitalWrite(trig, LOW);   // Makes trigPin low
  delayMicroseconds(2);       // 2 micro second delay

  digitalWrite(trig, HIGH);  // tigPin high
  delayMicroseconds(10);      // trigPin high for 10 micro seconds
  digitalWrite(trig, LOW);   // trigPin low

  long duration = pulseIn(echo, HIGH);   //Read echo pin, time in microseconds
  float distance = duration * 0.034 / 2;   //Calculating actual/real distance

  Serial.print("Distance = ");        //Output distance on arduino serial monitor
  Serial.println(distance);
  Blynk.virtualWrite(V3, distance);
}

BLYNK_WRITE(V0){
  int ledState = param.asInt();
  if(param.asInt() == 1)
  {
    digitalWrite(buzzer, HIGH);
  }
  else
  {
    digitalWrite(buzzer,LOW);
  }
}

void setup(){
  Serial.begin(9600);
  pinMode(buzzer,OUTPUT);
  pinMode(18,OUTPUT);
  pinMode(19,OUTPUT);
  pinMode(21,OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, sendSensor);
}

void loop(){
  Blynk.run();
  timer.run();
}
