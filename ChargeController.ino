#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h> 
// Install lib: DallasTemperature + OneWire

// Data wire is plugged into digital pin D6 on the Arduino
#define ONE_WIRE_BUS 6

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);	

// Pass oneWire reference to DallasTemperature library
DallasTemperature temperatureSensor(&oneWire);

// Address of temp sensor
// Place holder address NOW!
uint8_t temperatureSensor1[8] = { 0x28, 0xEE, 0xD5, 0x64, 0x1A, 0x16, 0x02, 0xEC };	//Internal
uint8_t temperatureSensor2[8] = { 0x28, 0x61, 0x64, 0x12, 0x3C, 0x7C, 0x2F, 0x27 };	//LeadAcid
uint8_t temperatureSensor3[8] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };	//Lipo

const int solarEn 		= 2;
const int enLiIon 		= 3;				// LOW = battery charging is working, HIGH = Stop charging
const int solarShort 	= 5;
const int systemTempLED = 8;
const int liIonTempLED  = 9;
const int currentIn 	= A0;
const int voltageIn 	= A1;
const int currentBatt 	= A2;
const int voltageBatt 	= A3;
//int dataLine = A4;  //SDA			//No need
//int clockLine = A5;  //SCL		//No need
const int currentLoad 	= A6;

void setup() {
  pinMode(solarShort, INPUT);		//Default OFF, there is pull down resistor
  pinMode(enLiIon, OUTPUT);
  pinMode(solarEn, INPUT);			//Default ON, there is pull up resistor
  pinMode(currentIn, INPUT);
  pinMode(currentBatt, INPUT);
  pinMode(voltageBatt, INPUT);
  pinMode(currentLoad, INPUT);
//  pinMode(dataLine, OUTPUT);		//Shouldn't need to declare for I2C
//  pinMode(clockLine, OUTPUT);		//Shouldn't need to declare for I2C
  pinMode(currentLoad, INPUT);
  
  // turn on LiIon charger
  digitalWrite(liIonTempLED, LOW);
  
  //Initialize Communication - Only need to perform once
  Serial.begin(9600);			
  Wire.begin();
  temperatureSensor.begin();
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


   //for debug
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

/*
 * tempSensingAndShutoff()
 * return: void
 * Read in Temp for all temp sensor and decide if the system need to be shutdown or not
 * CHECK: Do the system need to recover?
 */
void tempSensingAndShutoff(){
   temperatureSensor.requestTemperatures();
   
   float internalTemp = temperatureSensor.getTempC(temperatureSensor1);
   float leadacidTemp = temperatureSensor.getTempC(temperatureSensor2);
   float liIonTemp 	  = temperatureSensor.getTempC(temperatureSensor3);
   
   // overTempMain include internal temperature sensors and lead acid battery 
   // these device provide/sink large amount of current
   bool overTempMain = (internalTemp > 40) || (leadacidTemp > 40);
   bool overTempLiIon = liIonTemp > 40;
	
   if(overTemp == 1){ 
	  //Over temp LED signal
      digitalWrite(systemTempLED, HIGH);
	  //Shut down voltage regulator
	  pinMode(solarEn, OUTPUT);
      digitalWrite(solarEn, LOW);
      //Serial.print("over 40C");  
   }
  
   else{
	  //Turn off LED if nothing is wrong
      digitalWrite(systemTempLED, LOW);
      //Serial.print("skip broken led");
   }
   
   if(overTempLiIon = 1){
	   //turn off charging until reset
	   digitalWrite(enLiIon, HIGH);
	   digitalWrite(liIonTempLED, HIGH);
   }   

}


void shortCurrentOpenVoltage(){ //sends the status of the panel and how much power it is producing
   //Perform shutdown voltage regulator
   pinMode(solarEn, OUTPUT);
   digitalWrite(solarEn, LOW);
   delay(1000);
   int vOpen = analogRead(voltageIn)*(5/1024)*((5.1+49.9)/5.1);
   //Turn on short MOSFET
   pinMode(solarShort, OUTPUT);
   digitalWrite(solarShort, HIGH);
   delay(100);
   float iShort = (analogRead(currentIn)/2)/.110;
   
   //Turn off short MOSFET
   //Turn voltage regulator back on
   pinMode(solarShort, INPUT);
   pinMode(solarEn, INPUT);
   
   
   //CHECK
   //digitalWrite(inputNmos, high/low);
   int current = analogRead(currentBatt);
   //digitalWrite(inputNmos, high/low);
   int voltage = analogRead(voltageBatt);
  
   //CHECK
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

/*
 * sendChargingData()
 * return: void
 * print out to serial battery voltage and current
 */
void sendChargingData(){
   int value = 0;
  
   value = analogRead(currentBatt);
   Serial.write(value);
   value = analogRead(voltageBatt);
   Serial.write(value); 
}

/*
 * Need to rewrite to async code
 */
void battFullWait10Mins(){
  uint32_t period = 10 * 60000L;       // 10 minutes

  for( uint32_t tStart = millis();  (millis()-tStart) < period;  ){ //hang out in this loop for 10 minutes and just send the Pi the charging and panel data if it asks for it.
    sendSystemInfo(); 					// Sending too much data!
  }
}

/*
 * sendSystemInfo()
 * return: void
 * print out to serial requested data Vcurr, Icurr, Pcurr, Vopen, Ishort
*/
void sendSystemInfo(){ //send the voltage and current the battery sees. 
  //Serial.begin(9600);
  int requestCode = 0;
  if(Serial.available() > 0){
	requestCode = (Serial.read() - '0');
	delay(50);

    if(requestCode == 0){		//Code 0 mean regular charge data Vcurr, Icurr, Pcurr
       sendChargingData();
    }

    else if(requestCode == 1){	//Code 1 mean perfrom Open-Short test for solar panel
       shortCurrentOpenVoltage();
    }

    else{ //something was received through serial that wasn't one of those 2 strings. 
       Serial.print(incoming);
    } 
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
