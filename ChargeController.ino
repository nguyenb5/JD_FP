#include <Wire.h>
#include "DHT.h"


#define DHTPIN 2    // Digital pin connected to the DHT sensor
#define DHTPIN2 3
//#define DHTPIN3 4

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);
//DHT dht3(DHTPIN3, DHTTYPE);

int tempSense = 9;
int solarShort = 8;
int enLiIon = 6;
int solarEn = 5;
int currentIn = A0;
int voltageIn = A1;
int currentBatt = A2;
int voltageBatt = A3;
int dataLine = A4;  //SDA
int clockLine = A5;  //SCL
int currentLoad = A6;

void setup() {
  Wire.begin();
  pinMode(tempSense, OUTPUT);
  pinMode(solarShort, OUTPUT);
  pinMode(enLiIon, OUTPUT);
  pinMode(solarEn, OUTPUT);
  pinMode(currentIn, INPUT);
  pinMode(currentBatt, INPUT);
  pinMode(voltageBatt, INPUT);
  pinMode(currentBatt, INPUT);
  pinMode(dataLine, OUTPUT);
  pinMode(clockLine, OUTPUT);
  pinMode(currentLoad, INPUT);

  dht.begin();
  dht2.begin();
//  dht3.begin();  

  //pinMode( , OUTPUT);
  
  Serial.begin(9600);
}

void changePot(byte newWiperValue){
  //wiper value should be a number between 0 and 256, because there are 255 positions on the pot. The value of the pot can be measured from the wiper to point A or B.
  //Measuring from point A to the wiper will cause the resistance of the pot to decrease as the decimal value sent to it increases. Measuring from B to the wiper will cause the opposite to happen, but in this code we will 
  //be measuring from A to the wiper. 
  // In this case, 255 correlates to 119.5 ohms while 0 correlates to 5100. Each step of the decimal value increases the resistance by roughly 19.5 ohms. 
    //((value/256)*5000) + 100 will calculate the value of the pot based on the decimal value sent to it. 
  Wire.beginTransmission(44); //44 corresponds to the binary value that will begin the transmission to the pot. If AD0 is connected to 5V then 45 would be used. 
  Wire.write(byte(0x00));//the next byte sent doesn't matter, but the first bit determines whether to permanetly burn the resistance value onto the pot. We want a 0 here so we can change it again. 
  Wire.write(newWiperValue);//the third byte is the decimal value used to determine the resistance. This value should be between 0 and 255
  Wire.endTransmission();

  delay(500);
}

int readValues(){//returns the state of the circuit and how the battery should be charged. 
   int currentPanelTemporary = analogRead(currentIn);
   int voltageFromPanel = analogRead(voltageIn);
   int loadCurrent = analogRead(currentLoad);
   int currentBatteryTemporary = analogRead(currentBatt);
   int voltageToBattery = analogRead(voltageBatt);
   int currentUSBTemp = analogRead(currentLoad);

   //scale all of our values back
   float currentFromPanel = (currentPanelTemporary/2)/.110;
   float currentToBattery = (currentBatteryTemporary/2)/.110;
   float currentToUSB = (currentUSBTemp/2)/.110;
   float totalCurrentRequired = currentToBattery + currentToUSB;
   float batteryVoltage = voltageToBattery * .01543;

   Serial.print(currentFromPanel);
   Serial.print(currentToBattery);
   Serial.print(currentToUSB);
   Serial.print(totalCurrentRequired);
   Serial.print(batteryVoltage);
   
//   if(batteryVoltage <13.4){
//    //bulk charge
//    return 1;
//   }
//
//   else if(batteryVoltage > 14.2){
//    //rest
//    return 2;
//   }
//
//   else if (batteryVoltage > 13.2){
//    return 3; 
//   }
//   
//   else {
//    return 4;
//   }
//   
   
}

void bulkChangingBangBang(){

  byte wiper = 128;
  float batteryVoltage = 0;
  int currentBatteryTemporary = 0;
  int voltageToBattery = 0;
  float currentToBattery = 0;
  do{
    currentBatteryTemporary = analogRead(currentBatt);
    voltageToBattery = analogRead(voltageBatt);
    currentToBattery = (currentBatteryTemporary/2)/.110;
    batteryVoltage = voltageToBattery * .01543;
    
    if(currentToBattery > 1.5){
      wiper++;
      changePot(wiper);
    }

    else{
      wiper--;
      changePot(wiper);
    }
    delay(500);
    //do I need to shut off the power to the battery to take an accurate reading of its resting voltage. 
  }while(batteryVoltage < 13.2);

}

void tempSensingAndShutoff(){
  
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  float h2 = dht2.readHumidity();
  float t2 = dht2.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f2 = dht2.readTemperature(true);

  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  float hif2 = dht.computeHeatIndex(f2, h2);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic2 = dht.computeHeatIndex(t2, h2, false);

  if(t > 25){ 
    digitalWrite(11, HIGH);
        //Serial.print("over 25");  
  }
  
  else{
    digitalWrite(11, LOW);
        //Serial.print("skip broken led");
  }

}

void shortCurrentOpenVoltage(){ //sends the status of the panel and how much power it is producing. tc
   //digitalWrite(inputNmos, high/low);
   int current = analogRead(currentBatt);
   //digitalWrite(inputNmos, high/low);
   int voltage = analogRead(voltageBatt);
  
   Serial.begin(9600);
   Serial.write(current);
   Serial.write(voltage);
   Serial.end();
   
   //digitalWrite( )short the input to get current
   //delay(100) //wait for sensor to reach a value
   //value = analogRead();      
   //digitalWrite() return the input to the normal state
   //Serial.write(value);
   //digitalWrite() open the input
   //value = analogRead();
   //Serial.write(value);
}

void sendChargingData(){
   int value = 0;
  
   value = analogRead(currentBatt);
   Serial.write(value);
   value = analogRead(voltageBatt);
   Serial.write(value); 
}

void battFullWait10Mins(){
  uint32_t period = 10 * 60000L;       // 10 minutes

  for( uint32_t tStart = millis();  (millis()-tStart) < period;  ){ //hang out in this loop for 10 minutes and just send the Pi the charging and panel data if it asks for it.
    sendSystemInfo();
  }
}

void sendSystemInfo(){ //send the voltage and current the battery sees. 
  //Serial.begin(9600);
  int value = 0;
  if(Serial.available()){
    String incoming = Serial.readString();

    if(incoming == "sendMeData"){
       sendChargingData();
    }

    else if(incoming == "shortAndOpen"){
       shortCurrentOpenVoltage();
    }

    else{ //something was received through serial that wasn't one of those 2 strings. 
       Serial.print(incoming);
    } 
  }

  else{
    Serial.print("nothing was available");    
  }
}

byte wiper = 0;


void loop() {
  sendSystemInfo();
  int state = readValues();
  if(state == 1){
 
  }

  else{
       /*
    
  
    switch(state)
      case 1:
       //blah
       //change pot value
       break;
  
      case 2:
        break;
  
      default:
        //minimum charging voltage
        break;
      }
      */
  }
}
