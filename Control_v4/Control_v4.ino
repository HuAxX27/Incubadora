 
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// Control Ventilador //////////////////////////////////////
//////////////////////////// Control Resistencia /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////// HUGO AXEL LOZA MACIAS///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#include <TimerOne.h>           // Descargar librerias de http://www.arduino.cc/playground/Code/Timer1

volatile int i=0;               // Variable que se usara como contadoe
int AC_pin1 = 4;                // Salida a Opto Triac Carga#1
int AC_pin2 = 5;    // Salida a Opto Triac Carga #2
int dim1 = 1;                     // Dimming level (0-128)  0 = on, 128 = 0ff
int dim2 = 1;                    // Dimming level (0-128)  0 = on, 128 = 0ff
int inc=20;

float temp_w = 40.00;     //TEMPERATURA DESEADA     
//Temperatura minima
float tempL_m_w = 10;    

//Rango de temperaturas
float tempL_A_w = temp_w - .5;     //39.5
float tempL_B_w = temp_w - .25;    //39.75
float tempL_C_w = temp_w;          //40.00
float tempL_D_w = temp_w + .25;    //39.25
float tempL_E_w = temp_w + .5;     //39.5



float temp_r; //TEMPERATURA LEIDA *Aqui va a guardar lo leido*    

int freqStep = 65;    // Delay-por-brillo en microseconds.
// Esta frecuencia se basa en la frecuencia de la fuente (50Hz or 60Hz)
// y el numero de pasos que queremos. 
// 
// 1000000 us / 120 Hz = 8333 uS, Longitud de media onda.
// 8333 uS / 128 Pasos = 65 uS / Pasos
//

//Aqui va declarado el relee del indicador
int Relee = 8;

void setup() {    
  
  Serial.begin(9600);
  
  pinMode(AC_pin1, OUTPUT);                          // asignar el Pin de Triac como salida (Resistencia)
  pinMode(AC_pin2, OUTPUT);                          // asignar el Pin de Triac como salida (Ventilador)
  pinMode(Relee, OUTPUT);
   //digitalWrite(Relee, LOW);
   
  attachInterrupt(0, zero_cross_detect, RISING);   // Cruce por cero al Pin 2 (interupt 0)
  Timer1.initialize(freqStep);                      // Inicializamos el TimerOne library para la frequencia que necesitamos
  Timer1.attachInterrupt(dim_check, freqStep);                  
  // Use the TimerOne Library to attach an interrupt
  // to the function we use to check to see if it is 
  // the right time to fire the triac.  This function 
  // will now run every freqStep (65 uS for a 60Hz sinewave).                                            
}

void zero_cross_detect() {    
  i=0;
  // since the control pin stays high, the TRIAC won't 'unlatch'
  // when zero-crossing, so I need to put the pins to LOW
  digitalWrite(AC_pin1, LOW);
  digitalWrite(AC_pin2, LOW);
  
  // writing pins is like 10 times faster if 
  // we write the register directly 
  // instead of using 'digitalWrite'
}                                 

// Se enciende el triac en el tiempo correcto
void dim_check() {               
    if(i>=dim1) {                     
     digitalWrite(AC_pin1, HIGH); // Enciende Ventilador al llegar al conteo de Dim1
    }
    if (i>=dim2) {
     digitalWrite(AC_pin2,HIGH); // Enciende Resitencia al llegar al conteo de Dim2 
    }                    
    i++; //incrementa el paso de conteo


}                                   



void loop() {     
    Serial.flush();    
    if (Serial.available() > 4){
    temp_r = Serial.parseFloat();
    delay(30);
  //Serial.println(temp_r);
  
    }
//////////////////////////////////////////////////////////              
//Proteger el rango de activacion de opto 1 
    if (dim2<= 0){
    dim2 = 0;
    }
  else if (dim2 >= 128){
    dim2 = 128;
  }
  
 //Proteger el rango de activacion de opto 1     
      if (dim1<= 0){
    dim1 = 0;
  }
  else if (dim1 >= 128){
    dim1 = 128;
  }

////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Si el valor leido esta por debajo de temperatura buscada -0.5 se baja el tiempo de activacion en valor de inc 
   if ( temp_r <= 0 && temp_r <= tempL_m_w){
    dim2 = 128;      //Ventilador
    dim1 = 128;       //Resistencia
    digitalWrite(8, HIGH); 
    delay(3);  
  }

   
//Si el valor leido esta por debajo de temperatura buscada -0.5 se baja el tiempo de activacion en valor de inc 
   if ( temp_r < tempL_A_w && temp_r > tempL_m_w){
    dim2 = 0;      //Ventilador
    dim1 = 10;       //Resistencia
    digitalWrite(8, HIGH); 
    delay(3);  
  }
 
///Si se esta en el rango de temperaturas FLANCO BAJO -.50       Potencia de subida ALTA
else if ( temp_r >= tempL_A_w && temp_r < tempL_B_w){
    dim2 = 0;  //Rango de corte minimo en estado optimo //Ventilador//
    dim1 = 50;  //Rango de corte minimo en estado optimo //Resistencia//
    digitalWrite(8, LOW);
    delay(3); 
  }
  
///Si se esta en el rango de temperaturas FLANCO MEDIO BAJO -.25   Potencia de subida BAJA
  else if ( temp_r < tempL_C_w && temp_r >= tempL_B_w){
    dim2 = 20;  //Rango de corte minimo en estado optimo //Ventilador//
    dim1 = 100;  //Rango de corte minimo en estado optimo //Resistencia//
    digitalWrite(8, LOW);
    delay(3); 
  }

///Si se esta en el rango de temperaturas CORRECTO     POTENCIA MINIMA
  else if ( temp_r == tempL_C_w){
    dim2 = 50;  //Rango de corte minimo en estado optimo //Ventilador//
    dim1 = 128;  //Rango de corte minimo en estado optimo //Resistencia//
    digitalWrite(8, LOW);
    delay(3);
  }
///Si se esta en el rango de temperaturas FLANCO MEDIO ALTO +0.25       Potencia de Reduccion BAJA
  else if ( temp_r > tempL_C_w && temp_r < tempL_D_w){
    dim2 = 30;  //Rango de corte minimo en estado optimo //Ventilador//
    dim1 = 128;  //Rango de corte minimo en estado optimo //Resistencia//
    digitalWrite(8, LOW);
    delay(3); 
  }

///Si se esta en el rango de temperaturas FLANCO ALTO + 0.50     Potencia de Reduccion ALTA 
  else if ( temp_r > tempL_D_w && temp_r <= tempL_E_w){
    dim2 = 10;  //Rango de corte minimo en estado optimo //Ventilador//
    dim1 = 128;  //Rango de corte minimo en estado optimo //Resistencia//
    digitalWrite(8, LOW);
    delay(3);
  }
  

//Si el valor leido esta por encima de temperatura buscada +0.5 se aumenta el tiempo de activacion en valor de inc 
  else if ( temp_r > tempL_E_w){
    dim2 = 0;         //Ventilador
    dim1 = 128;         //Resistencia
    digitalWrite(8, HIGH);
    delay(3);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
  }
