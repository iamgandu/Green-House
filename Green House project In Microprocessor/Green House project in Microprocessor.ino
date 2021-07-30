//LCD SETUP
#include <LiquidCrystal.h>
//display
const short rs=5, en=4, d4=3, d5=2, d6=1, d7=0;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);

// 14 - 22 - heater on
// 22 - 32 - window on ac off heater off
// 32+ = ac on heater of


// 20 - 22 - heater on
// 22 - 27 - window on ac off heater off
// 28+ = ac on heater of


//Control Variables Threshhold
const short heater_on_temp = 20;
const short heater_off_temp = 22;
const short ac_activate = 28;


//Temperature sensor
const short room_temperature_sensor = A0;
float room_temperature_value = 0;
float room_temperature_sensor_value = 0;
const short outdoor_temperature_sensor = A1;
float outdoor_temperature_value = 0;
float outdoor_temperature_sensor_value = 0;


// SOIL HUMIDITY SENSOR
const short soil_humidity_sensor = A2;
float soil_humidity_sensor_value = 0;

// AIR HUMIDITY SENSOR
const short air_humidity_sensor = A3;
float air_humidity_sensor_value = 0;

//OUTDOOR LIGHT SENSOR
const short light_sensor = A4;
float light_sensor_value = 0;

// WIND SENSOR
const short wind_sensor = A5;
float wind_sensor_value = 0;



// Control pins (output)
const short heater_pin = 13;
const short airconditioner_pin = 12;
const short humidifier_pin= 11;
const short lights_pin= 10;
const short pump_pin= 9;
const short window_pin= 8;

//control state
short heater_state = 0;
short airconditioner_state = 0;
short humidifier_state= 0;
short lights_state= 0;
short pump_state= 0;
short window_state= 0;

// for lcd





void setup() {

  //inputs
  pinMode(soil_humidity_sensor, INPUT);
  pinMode(air_humidity_sensor, INPUT);
  pinMode(light_sensor, INPUT);
  pinMode(room_temperature_sensor, INPUT);
  pinMode(outdoor_temperature_sensor, INPUT);
  pinMode(wind_sensor, INPUT);

  //outputs
  lcd.begin(20,4);
  pinMode(heater_pin, OUTPUT);
  pinMode(airconditioner_pin, OUTPUT);
  pinMode(humidifier_pin, OUTPUT);
  pinMode(lights_pin, OUTPUT);
  pinMode(pump_pin, OUTPUT);
  pinMode(window_pin, OUTPUT);
  
}

void loop() {

  //get all sensor data
  getTemperatureSensorData();
  getSoilHumiditySensorData();
  getAirHumiditySensorData();
  getLightSensorData();
  getWindSensorDate();

  updateLogic();
  

  lcd.clear();
  lcdDisplay();
 
  delay(10); 

}


//tool functions

float normalizeAnalogInput(const int sensor){
  float value = 0;
   for (int i = 0; i <= 20; i++) 
 { 
   value = value + analogRead(sensor); 
   delay(1); 
 } 
   value = value/20.0; 
   return value;
}




// Sensor data

void getSoilHumiditySensorData(){
  soil_humidity_sensor_value  = normalizeAnalogInput(soil_humidity_sensor);
 
}

void getAirHumiditySensorData(){
    air_humidity_sensor_value  =  normalizeAnalogInput(air_humidity_sensor);
}

void getLightSensorData(){
    light_sensor_value  = normalizeAnalogInput(light_sensor);
}

void getTemperatureSensorData(){
  float temp_convert= ((5.0/1023.0)*100) ;
  //get sensor data
  room_temperature_sensor_value = normalizeAnalogInput(room_temperature_sensor);
  outdoor_temperature_sensor_value = normalizeAnalogInput(outdoor_temperature_sensor);
  //convert temp
  room_temperature_value = room_temperature_sensor_value * temp_convert;
  outdoor_temperature_value = outdoor_temperature_sensor_value * temp_convert;
}

void getWindSensorDate(){
  wind_sensor_value = normalizeAnalogInput(wind_sensor);
}

//LCD display function
void lcdDisplay(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Room Temp: "+ String(room_temperature_value,1) + "C" );
  lcd.setCursor(0, 1);
  lcd.print("OutsideTemp: "+ String(outdoor_temperature_value,1 )+ "C");

  // HUMIDITY
  lcd.setCursor(0, 2);                 
  if (air_humidity_sensor_value<(1044/2)){
    lcd.print("Humid: Low");     
  }
  else{
    lcd.print("Humid: High");   
  }
  
  //SOIL HUMIDITY
  lcd.setCursor(0, 3);                 
  if(pump_state == 0){
    lcd.print("S.H.: Dry");   
  }
  else if (pump_state == 1){
    lcd.print("S.H.: Moist");   
  }
  else{
    lcd.print("S.H.: Wet");   
  }

  delay(200);
  lcd.clear();
  //delay(700);
  
  lcd.setCursor(0, 0);
  if(airconditioner_state== 1 && humidifier_state ==1)  lcd.print("AC:ON  HUMIDF:ON");
  else if(airconditioner_state == 0 && humidifier_state ==0)  lcd.print("AC:OFF  HUMIDF:OFF");
  else if(airconditioner_state == 0 && humidifier_state ==1)  lcd.print("AC:OFF  HUMIDF:ON");
  else if(airconditioner_state == 1 && humidifier_state ==0)  lcd.print("AC:ON  HUMIDF:OFF");


  //PUMP AND HEATER
  lcd.setCursor(0, 1);
  if(heater_state== 1 && (pump_state ==0|| pump_state==1))  lcd.print("HEATER:ON  PUMP:ON");
  else if(heater_state== 0 && (pump_state ==0||pump_state==1))  lcd.print("HEATER:OFF  PUMP:ON");
  else if(heater_state== 1 && (pump_state ==2))  lcd.print("HEATER:ON  PUMP:OFF");
  else if(heater_state== 0 && (pump_state ==2))  lcd.print("HEATER:OFF  PUMP:OFF");

  //DAYTIME AND LIGHT
  lcd.setCursor(0, 2);                 
  if(lights_state== 1)   lcd.print("LS: "+ String(light_sensor_value) +" Light:ON" );
  else lcd.print("LS:"+ String(light_sensor_value) +" Light:OFF " );
  
  //wind and window
  lcd.setCursor(0, 3); 
  if(window_state==0){
    lcd.print("WIND:LOW WINDOW:OPEN");
  }
  else if(window_state == 1){
    lcd.print("WIND:MID WINDOW:OPEN");
  }
  else if(window_state == 2){
    lcd.print("WIND:STORM WINDOW:CLOSED");
  }
  else{
    lcd.print("WIND:COLD WINDOW:CLOSED");
  }


  delay(500) ;
}

void updateLogic(){
  //air humidity and humidifier
  if(air_humidity_sensor_value<(1044/2)){
    digitalWrite(humidifier_pin,HIGH);
    humidifier_state = 1;
  }
  else{
    digitalWrite(humidifier_pin,LOW);
    humidifier_state = 0;
  }
  
  // lights
  if(light_sensor_value<100){
    digitalWrite(lights_pin,HIGH);
    lights_state = 1;
  }
  else{
    digitalWrite(lights_pin,LOW);
    lights_state = 0;
  }

  //airconditioner and heater
  if(room_temperature_value<heater_on_temp){
    //turn on heater
    digitalWrite(heater_pin,HIGH);
    digitalWrite(airconditioner_pin,LOW);
    heater_state = 1;
    airconditioner_state = 0;

  }
  else if(room_temperature_value>heater_off_temp && room_temperature_value<ac_activate){
    //BOTH OFF normal temp
    digitalWrite(heater_pin,LOW);
    digitalWrite(airconditioner_pin,LOW);
    heater_state = 0;
    airconditioner_state = 0;
  }
  else{
    digitalWrite(heater_pin,LOW);
    digitalWrite(airconditioner_pin,HIGH);
    heater_state = 0;
    airconditioner_state = 1;
  }

  //WINDOW CONTROL
  if(heater_state ==0 && airconditioner_state ==0)
  {
      if(((wind_sensor_value/1024)*100) <25 ){
      // low wind, open window
        digitalWrite(window_pin,HIGH);
        window_state = 0;
      }
      else if(((wind_sensor_value/1024)*100) >25 && ((wind_sensor_value/1024)*100) <75){
      // normal wind, open window
        digitalWrite(window_pin,HIGH);
        window_state = 1;
      }
      else{
        // high wind, close window
        digitalWrite(window_pin,LOW);
        window_state = 2;
      }
  }
  else{
    //bad temperature
    digitalWrite(window_pin,LOW);
    window_state = 3;
  }
  
  // pump control
  if(((soil_humidity_sensor_value/1024)*100) <25){
      //dry
      digitalWrite(pump_pin,HIGH);
      pump_state=0;
  }
  else if(((soil_humidity_sensor_value/1024)*100) <75){
      //moist
      digitalWrite(pump_pin,HIGH);
      pump_state=1;
  }
  else{
      //TURN OFF PUMP and wet
      digitalWrite(pump_pin,LOW);
      pump_state=2;
  }



}
