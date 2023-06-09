#include <IRremote.h>

#define sensorPin A0
const int pingPin = 12; 
const int echoPin = 11;
byte nomorPinLED[] = {2,3,4,5,6,7};

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(10,false);
  for(int x=0; x<7; x++){
  pinMode(nomorPinLED[x], OUTPUT);
  }
}



void loop() {
  int input;
  if (IrReceiver.decode()) {
    input = IrReceiver.decodedIRData.command;
    switch(input){
      case 12:
        Temperature();
        break;
      
      case 24:
        Distance();
        break;

      case 94:
        LED();
        Serial.println("LED is ON");
        break;

      default:
        Serial.println("Error! operator is not correct");
    }
//    Serial.println(input);
    IrReceiver.resume();
//  Serial.println(IrReceiver.decodedIRData.command);
  }
  delay(500);

}


void Temperature(){
  int reading = analogRead(sensorPin);
  
  // Convert the reading into voltage:
  //Convert digital data into analog by multiplying by 5000 and dividing by 1024
  float voltage = reading * (5000 / 1024.0);
  // Convert the voltage into the temperature in degree Celsius:
  float temperatureC = voltage / 10;
  float temperatureF=(temperatureC*1.8)+32; // Converting to Fahrenheit 
  
  // Print the temperature in Celsius into the Serial Monitor:
  Serial.print("Temperature in Celsius    = ");
  Serial.print(temperatureC);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.println("C");
  
  // Print the temperature in Celsius into the Serial Monitor:
  Serial.print("Temperature in Fahrenheit = ");
  Serial.print(temperatureF);
  Serial.print(" \xC2\xB0"); // shows degree symbol
  Serial.println("F");
  
  Serial.print("\n");
}

void LED(){
  for (int x=0 ; x<7 ; x++){
    digitalWrite(nomorPinLED[x], HIGH);
    delay(50);
    digitalWrite(nomorPinLED[x], LOW);
    delay(50);
  }
}

void Distance(){
  long duration, inches, cm;
   pinMode(pingPin, OUTPUT);
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(pingPin, LOW);
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);
   inches = microsecondsToInches(duration);
   cm = microsecondsToCentimeters(duration);
   Serial.print(inches);
   Serial.print(" in || ");
   Serial.print(cm);
   Serial.print(" cm");
   Serial.println();
}

long microsecondsToInches(long microseconds) {
   return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
   return microseconds / 29 / 2;
}
