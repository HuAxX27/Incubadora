//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// PROGRAMA DE SENSADO ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// HUGO AXEL LOZA MACIAS///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////LIBRERIAS PARA INSTALAR//////////////////////////////////////
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <SD.h>
#include <SPI.h>
#include <DHT.h>
//////////////////////////////////////////////////////////////////////////////////////


#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif 

// Definimos el pin digital donde se conecta el sensor
#define HT1 36
#define HT2 37

// Dependiendo del tipo de sensor
#define DHTTYPE DHT22

uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
//uint8_t second, minute, hour, wday, day, month, year; 

#define I2C_ADDR 0x27

RTC_DS1307 RTC;
LiquidCrystal_I2C lcd(I2C_ADDR,16,2);  

// Inicializamos el sensor DHT11
DHT dht1 (HT1, DHTTYPE);  //Indica el pin con el que trabajamos y el tipo de sensor
DHT dht2 (HT2, DHTTYPE);

File myFile;
int pinCS = 53; // Pin 10 on Arduino Uno 

//Aqui va declarado el relee del motor
int MotorRel = 2;

void setup() {
  pinMode(pinCS, OUTPUT);
  pinMode(MotorRel, OUTPUT);   
  
  
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.print("Incubadora ON");
    lcd.createChar(2, clock);
  Wire.begin();
   RTC.begin();
   SD.begin();
  delay(2000);
  
  // SD Card Initialization
  if (SD.begin())
  {
    lcd.setCursor(0, 1);
    lcd.print("MOD SD CORRECTO");
  } else
  {
     lcd.setCursor(0, 1);
     lcd.clear();
     lcd.print(" MOD SD INCORRECTO");
    return;
  }
 dht1.begin();  //Iniciamos los sensores
 //Inicializamos el serial
 Serial.begin(9600);
}
void loop() {
  //Lee el sensor 
 float h1 = dht1.readHumidity();
 float t1 = dht1.readTemperature();
 float h2 = dht2.readHumidity();
 float t2 = dht2.readTemperature();

//Promedia los 2 sensores
float h = (h1 + h2)/2;
 float t= (t1 + t2)/2;
 delay(50);
 if(t > 10){
Serial.flush();
Serial.println(t);
delay(30);
 }
  myFile = SD.open("test.txt", FILE_WRITE);
  delay(500);
  if (myFile) { 
    //myFile.print(day + "/" + month + "/" + year + " " + hour + ":" + minute);
    //myFile.print(",");  
    
///////////////////////////////////////////////////////
/////////Se guarda hora fecha y sensado en SD//////////
///////////////////////////////////////////////////////     

    DateTime now = RTC.now();
    myFile.print(h);
    myFile.print(",");    
    myFile.print(t);
    myFile.print(",");
    myFile.print(now.day(), DEC);
    myFile.print("/");    
    myFile.print(now.month(), DEC);
    myFile.print("/");
    myFile.print(now.year(), DEC);
    myFile.print(" ");    
    myFile.print(now.hour(), DEC);
    myFile.print(":");
    myFile.println(now.minute(), DEC);
    
///////////////////////////////////////////////////////
/////////Se muestra hora fecha y sensado en LCD////////
///////////////////////////////////////////////////////    

    lcd.clear();
    lcd.printByte(2);
  lcd.print(now.hour(), DEC);
    lcd.print(':');
    lcd.print(now.minute(), DEC);
    lcd.print(':');
    lcd.print(now.second(), DEC);
    lcd.print(" ");
    lcd.print("T:");
    lcd.print(t);
    lcd.setCursor(0, 1);
   lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print(now.month(), DEC);
    lcd.print('/');
    lcd.print(now.year(), DEC);
    lcd.print(" ");
    lcd.print("H:");
    lcd.print(h);

///////////////////////////////////////////////////////////////////
//Ciclo IF para controlar el giro del motor cada hora medio minuto//
////////////////////////////////////////////////////////////////////
    if(now.minute()== 30 && now.second() <= 29 ){
      digitalWrite(MotorRel, LOW);   // Se activa el relee
      
    }
    else if(now.minute()== 30 && now.second()>= 30){
     digitalWrite(MotorRel, HIGH);  //Se apaga el relee
    }
    else{
      digitalWrite(MotorRel, HIGH); 
    }

///////////////se cierra el grabado en SD////////////////////////       
    myFile.close(); // close the file
  }
  // if the file didn't open, print an error:
  else {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Error Documento");
  }

}
