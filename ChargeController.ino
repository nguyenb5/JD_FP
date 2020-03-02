  #include <Wire.h>
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
uint8_t temperatureSensor1[8] = { 0x28, 0x6A, 0x24, 0xFC, 0x0B, 0x00, 0x00, 0x57 };	//Internal
uint8_t temperatureSensor2[8] = { 0x28, 0x33, 0x09, 0x79, 0x97, 0x15, 0x03, 0xC1 };	//LeadAcid
uint8_t temperatureSensor3[8] = { 0x28, 0x61, 0x64, 0x12, 0x3F, 0xFD, 0x80, 0xC6 };	//Lipo

const int solarEnPin 		  = 2;
const int enLiIonPin 		  = 3;				// LOW = battery charging is working, HIGH = Stop charging
const int solarShortPin 	= 5;
const int systemTempLEDPin = 8;
const int liIonTempLEDPin  = 9;
const int currentInPin 	  = A0;
const int voltageInPin 	  = A1;
const int currentOutPin 	= A2;
const int voltageBattPin 	= A3;
const int currentLoadPin 	= A6;


// Global vars

float vOpen;
float iShort; 
float votlageFromPanel;
float currentFromPanel;
float Pcurr;
float currentToSystem;
float voltageToBatt;
float currentToBatt;         // = CurrentToSystem - CurrentToLoad
float currentToLoad;         //This is the current of load

byte wiper;

void setup() {
  pinMode(solarShortPin, INPUT);		//Default OFF, there is pull down resistor
  pinMode(enLiIonPin, OUTPUT);
  pinMode(solarEnPin, INPUT);			//Default ON, there is pull up resistor
  pinMode(currentInPin, INPUT);
  pinMode(currentOutPin, INPUT);
  pinMode(voltageBattPin, INPUT);
  pinMode(currentLoadPin, INPUT);
//  pinMode(dataLine, OUTPUT);		//Shouldn't need to declare for I2C
//  pinMode(clockLine, OUTPUT);		//Shouldn't need to declare for I2C
  pinMode(currentLoadPin, INPUT);
  
  // turn on LiIon charger
  digitalWrite(liIonTempLEDPin, LOW);
  
  // declare global variable
  wiper = 128;   //init

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
   int currentPanelTemporary      = analogRead(currentInPin);   //from solar
   int voltageFromPanelTemporary  = analogRead(voltageInPin);   //from solar
   int currentOutTemporary        = analogRead(currentOutPin);  // Ibatt + Iload
   int voltageToBatteryTemporary  = analogRead(voltageBattPin); // Vbatt
   int currentLoadTemporary       = analogRead(currentLoadPin); // Iload
    //Current to battery = SolarOutput - Iload

    //scale all of our values back
    // Update Global Vars
    currentFromPanel = (currentPanelTemporary*5/1024)/.110;
    currentToSystem = (currentOutTemporary*5/1024)/.110;
    currentToLoad = (currentLoadTemporary*5/1024)/.110;
    currentToBatt = currentToSystem - currentToLoad;
    voltageToBatt = voltageToBatteryTemporary *.01543;

  //  currentFromPanel = (currentPanelTemporary/2)/.110;
  //  float currentToSystem = (currentOutTemporary/2)/.110;
  //  float currentToLoad = (currentLoadTemporary/2)/.110;
  //  float totalCurrentRequired = currentToBattery + currentToLoad; //CHECK
  //  float batteryVoltage = voltageToBattery * .01543;


   //for debug
   Serial.print(currentFromPanel);
   Serial.print(currentToSystem);
   Serial.print(currentToLoad);
   Serial.print(currentToBatt);
   Serial.print(voltageToBatt);
   
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
  do{    
    if(currentToBatt > 1.5){
      wiper++;              //CHECK if this decrease charging voltage;
      changePot(wiper);
    }

    else{
      wiper--;
      changePot(wiper);
    }
    delay(500);
    //do I need to shut off the power to the battery to take an accurate reading of its resting voltage. 
  } while(voltageToBatt < 14.2);

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
	
    if(overTempMain == 1){ 
	  //Over temp LED signal
      digitalWrite(systemTempLEDPin, HIGH);
	  //Shut down voltage regulator
	    pinMode(solarEnPin, OUTPUT);
      digitalWrite(solarEnPin, LOW);
      //Serial.print("over 40C");  
   }
  
    else{
	  //Turn off LED if nothing is wrong
      digitalWrite(systemTempLEDPin, LOW);
      //Serial.print("skip broken led");
   }
   
    if(overTempLiIon = 1){
	   //turn off charging until reset
	    digitalWrite(enLiIonPin, HIGH);
	    digitalWrite(liIonTempLEDPin, HIGH);
   }   

}


void shortCurrentOpenVoltage(){ //sends the status of the panel and how much power it is producing
   //Perform shutdown voltage regulator
    pinMode(solarEnPin, OUTPUT);
    digitalWrite(solarEnPin, LOW);
    delay(1000);
    vOpen = analogRead(voltageInPin)*(5/1024)*((5.1+49.9)/5.1);
    //Turn on short MOSFET
    pinMode(solarShortPin, OUTPUT);
    digitalWrite(solarShortPin, HIGH);
    delay(100);
    iShort = (analogRead(currentInPin)/2)/.110;
   
    //Turn off short MOSFET
    //Turn voltage regulator back on
    pinMode(solarShortPin, INPUT);
    pinMode(solarEnPin, INPUT);
   
}

void sendShortOpen(){
  Serial.println(vOpen);
  Serial.println(iShort);
}

/*
 * sendChargingData()
 * return: void
 * print out to serial battery voltage and current
 */
void sendChargingData(){
  //  int value = 0;
  Serial.println(votlageFromPanel);
  Serial.println(currentFromPanel);
  Serial.println(votlageFromPanel*currentFromPanel); // power from panel

  //Serial.println(batteryVoltage);
  //Serial.println(currentToBatt);
  //Serial.println(currentToLoad);

  
  //  value = analogRead(currentOut);
  //  Serial.write(value);
  //  value = analogRead(voltageBattPin);
  //  Serial.write(value); 
}

/*
 * Need to rewrite to async code CHECK
 */
void battFullWait10Mins(){
  uint32_t period = 10 * 60000L;       // 10 minutes

  for( uint32_t tStart = millis();  (millis()-tStart) < period;){ //hang out in this loop for 10 minutes and just send the Pi the charging and panel data if it asks for it.
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
       sendShortOpen();
    }

    else{ //something was received through serial that wasn't one of those 2 strings. 
       Serial.print(requestCode);
    } 
  }
}

void loop() {
  sendSystemInfo();
  int state = readValues();
  //Gather data

  //Execute based on state

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
 