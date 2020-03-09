#include <Wire.h>
#include <DallasTemperature.h> 
// Install lib: DallasTemperature + OneWire

// Data wire is plugged into digital pin D6 on the Arduino
#define ONE_WIRE_BUS 6
#define CONSTANT_CHARGING_VOLTAGE 14.2
#define CONSTANT_CHARGING_CURRENT 2
#define FLOATING_BATT_VOLTAGE 13.5

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature temperatureSensor(&oneWire);

// Address of temp sensor
// Place holder address NOW!
uint8_t temperatureSensor1[8] = { 0x28, 0x6A, 0x24, 0xFC, 0x0B, 0x00, 0x00, 0x57 };  //Internal
uint8_t temperatureSensor2[8] = { 0x28, 0x33, 0x09, 0x79, 0x97, 0x15, 0x03, 0xC1 }; //LeadAcid
uint8_t temperatureSensor3[8] = {0x28, 0x00, 0x6B, 0xFD, 0x0B, 0x00, 0x00, 0x52 }; //Lipo

const int solarEnPin      = 2;
const int enLiIonPin      = 3;        // LOW = battery charging is working, HIGH = Stop charging
const int solarShortPin   = 5;
const int systemTempLEDPin = 8;
const int liIonTempLEDPin  = 9;
const int currentInPin    = A0;
const int voltageInPin    = A1;
const int currentOutPin   = A2;
const int voltageBattPin  = A3;
const int currentLoadPin  = A6;


// Global vars

//Open Current
float vOpen;

//Solar panel short current
float iShort; 

//Current voltage from panel
float voltageFromPanel;

//Immediate current from panel
float currentFromPanel;

//
float currentToSystem;
//hai

float internalTemp;
float leadacidTemp;
float liIonTemp;

float voltageToBatt;
float currentToBatt;         // = CurrentToSystem - CurrentToLoad
float currentToLoad;         //This is the current of load

int currentPanelTemporary;
int voltageFromPanelTemporary;  
int currentOutTemporary; 
int voltageToBatteryTemporary;  
int currentLoadTemporary;       

byte wiper;

typedef enum BattStates {BULK,TRICKLE, FlOATING} BattStates;


void setup() {
  pinMode(solarShortPin, OUTPUT);
  digitalWrite(solarShortPin, LOW); //Default OFF, there is pull down resistor
  
  pinMode(enLiIonPin, OUTPUT);
  pinMode(solarEnPin, OUTPUT);      //Default ON, there is pull up resistor
  digitalWrite(solarEnPin, HIGH);
  pinMode(currentInPin, INPUT);
  pinMode(currentOutPin, INPUT);
  pinMode(voltageBattPin, INPUT);
  pinMode(currentLoadPin, INPUT);
//  pinMode(dataLine, OUTPUT);    //Shouldn't need to declare for I2C
//  pinMode(clockLine, OUTPUT);   //Shouldn't need to declare for I2C
  
  // turn on LiIon charger
  digitalWrite(liIonTempLEDPin, LOW);
  digitalWrite(enLiIonPin, LOW);
  
  // declare global variable
  wiper = 155;   //init

  //Initialize Communication - Only need to perform once
  Serial.begin(9600);     
  Wire.begin();
  temperatureSensor.begin();
}


/*
 * name: changePot()
 * return: void
 * Pretty code - I2C communication to change feedback voltage
 */
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
  delay(50);

}

/*
 * bulkChangingBangBang() 
 * return: void
 * Description: Bumps current until hits 2 amps
 */
void bulkChangingBangBang(){
  //do I need to shut off the power to the battery to take an accurate reading of its resting voltage. 
  if(currentToBatt > 2.2){
    wiper+= 1;              //CHECK if this decrease charging voltage;
    changePot(wiper);
  }

  else if (currentToBatt < 1.8){
    wiper -= 1;
    changePot(wiper);
  }
}


/*
 * name: readValues()
 * return: void
 * description: Read in analog values and update to global vars
 */
void determineBattValues(){//returns the state of the circuit and how the battery should be charged. 
   currentPanelTemporary      = analogRead(currentInPin);   //from solar
   voltageFromPanelTemporary  = analogRead(voltageInPin);   //from solar
   currentOutTemporary        = analogRead(currentOutPin);  // Ibatt + Iload
   voltageToBatteryTemporary  = analogRead(voltageBattPin); // Vbatt
   currentLoadTemporary       = analogRead(currentLoadPin); // Iload
    //Current to battery = SolarOutput - Iload

    //scale all of our values back
    // Update Global Vars
    currentFromPanel  = -0.0277*currentPanelTemporary+15.052;
    voltageFromPanel  = 0.981*(voltageFromPanelTemporary * .0526577) + 0.0069-.17;
    currentToSystem   = -0.0284*currentOutTemporary+15.15;
    currentToLoad     = -0.029*currentLoadTemporary+15.38;
    currentToBatt     = currentToSystem - currentToLoad;
    voltageToBatt     = voltageToBatteryTemporary *.01541436 - 0.4;
   
}

BattStates BattState = BULK;
//Handles logic for chargin battery
//TODO: Handle moving backward in states
void chargeBatt(){
//  static BattStates BattState = BULK;

  //If under chanrged
  switch(BattState){
      case BULK: 
//        Serial.println("In bulk!");
        bulkChangingBangBang();
       if(voltageToBatt > CONSTANT_CHARGING_VOLTAGE){
         BattState = TRICKLE;
       }
      break;
      case TRICKLE:
//        Serial.println("In trickle!");
        if(currentToBatt <= 0.2 * CONSTANT_CHARGING_CURRENT){
          BattState = FlOATING;
        }
        if(currentToBatt>0.5){
          BattState = BULK;
          wiper = 155;
        }
        
      break;  
      case FlOATING:
//        Serial.println("In floating!");
        setBattVoltageBangBang(FLOATING_BATT_VOLTAGE);
        if(currentToBatt>0.5){
          BattState = BULK;
        }
  
      break;
  }
}


/*
 * tempSensingAndShutoff()
 * return: void
 * Read in Temp for all temp sensor and decide if the system need to be shutdown or not
 * CHECK: Do the system need to recover?
 */
void tempSensingAndShutoff(){
   temperatureSensor.requestTemperatures();
   
   internalTemp = temperatureSensor.getTempC(temperatureSensor1);
   leadacidTemp = temperatureSensor.getTempC(temperatureSensor2);
   liIonTemp = temperatureSensor.getTempC(temperatureSensor3);
   // overTempMain include internal temperature sensors and lead acid battery 
   // these device provide/sink large amount of current
   bool overTempMain = (internalTemp > 40) || (leadacidTemp > 40);
   bool overTempLiIon = liIonTemp > 40;
  
    if(overTempMain == 1){ 
    //Over temp LED signal
      digitalWrite(systemTempLEDPin, HIGH);
    //Shut down voltage regulator
      digitalWrite(solarEnPin, LOW);
      Serial.print("over 40C");  
   }
  
    else{
      //Turn off LED iftemp nothing is wrong
      digitalWrite(systemTempLEDPin, LOW);
      //Serial.print("skip broken led");
   }
   
    if(overTempLiIon == 1){
      Serial.println(liIonTemp);
      //turn off charging until reset
      digitalWrite(enLiIonPin, HIGH);
      digitalWrite(liIonTempLEDPin, HIGH);
   }   

}

/*
 * shortCurrentOpenVoltage()
 * return: void
 * Turn of regulator to measure Vopen, then short circuit to measure Ishort
 * CHECK: Do the system need to recover?
 */
void shortCurrentOpenVoltage(){ //sends the status of the panel and how much power it is producing 
   //Perform shutdown voltage regulator
    digitalWrite(solarEnPin, LOW);
    delay(1000);
    
    vOpen = 0.981*(analogRead(voltageInPin) * .0526577) + 0.0069-.17;
    //Turn on short MOSFET

    delay(100);
    digitalWrite(solarShortPin, HIGH);
    delay(100);
    iShort = -0.0277*analogRead(currentInPin)+15.052;
       delay(100);

    //Turn off short MOSFET
    //Turn voltage regulator back on
    digitalWrite(solarShortPin, LOW);
    digitalWrite(solarEnPin, HIGH);
   
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
//  Serial.println(voltageFromPanel);
//  Serial.print("Current from panel voltage:  ");
//  Serial.println(currentFromPanel);
  
//  Serial.println(voltageFromPanel*currentFromPanel); // power from panel
//  Serial.println(internalTemp);
//  Serial.println(leadacidTemp);
//  Serial.println(liIonTemp);
  Serial.print("Batt voltage:  ");
  Serial.println(voltageToBatt);
  Serial.print("Current   ");
  Serial.println(currentToBatt);
  Serial.print("state: ");
  Serial.println(BattState);
//  Serial.print("System current:  ");
//  Serial.println(currentFromPanel);
//  Serial.print("Load current:  ");
//  Serial.println(currentToLoad);

//  Serial.print("Curren val:  ");
//  Serial.println(currentOutTemporary);
  Serial.print("Wiper: ");
  Serial.println(wiper);

  Serial.print(internalTemp);
    Serial.print(" " );
    Serial.print(leadacidTemp);
    Serial.print(" " );
    Serial.println(liIonTemp);



}



/*
 * name: sendSystemInfo()
 * return: void
 * print out to serial requested data Vcurr, Icurr, Pcurr, Vopen, Ishort
*/
void sendSystemInfo(){ //send the voltage and current the battery sees. 
  int requestCode = 0;
  if(Serial.available() > 0){
    requestCode = (Serial.read() - '0');
    delay(50);

    if(requestCode == 0){   //Code 0 mean regular charge data Vcurr, Icurr, Pcurr
      sendChargingData();
    }

    else if(requestCode == 1){  //Code 1 mean perfrom Open-Short test for solar panel
      shortCurrentOpenVoltage();
      sendShortOpen();
    }

    else{ //something was received through serial that wasn't one of those 2 strings. 
      //  Serial.print(requestCode);
    } 
  }
}

/*
 * name: sendSystemInfo()
 * return: void
 * print out to serial requested data Vcurr, Icurr, Pcurr, Vopen, Ishort
*/
void setBattVoltageBangBang(float target){
  //As wiper increases votage decreases
  if(voltageToBatt > target){
    wiper++;              //CHECK if this decrease charging voltage;
    changePot(wiper);
  }

  else if (voltageToBatt < target){
    wiper--;
    changePot(wiper);
  }

}


void loop() {
  
   tempSensingAndShutoff();
  determineBattValues();
  sendSystemInfo();
  chargeBatt();
}
 
