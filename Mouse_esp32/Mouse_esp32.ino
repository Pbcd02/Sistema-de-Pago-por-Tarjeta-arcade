#include <BleMouse.h>


#define Button 33
#define Button2 22
#define Periodo 150
volatile bool Bandera= false;
volatile bool Bandera2= false;
volatile bool Bandera3= true;

volatile unsigned long ultimoTiempo = 0;
volatile unsigned long ultimoTiempo2 = 0;

volatile unsigned long uTiempo = 0;
volatile unsigned long iTiempo = 0;
volatile unsigned int inter = 1500;


BleMouse bleMouse;


void IRAM_ATTR presionado() {
  unsigned long tiempoActual = millis();
  if ((tiempoActual - ultimoTiempo) > Periodo) {
    Bandera = !bleMouse.isPressed(MOUSE_LEFT);
    if(Bandera){
      bleMouse.press(MOUSE_LEFT);
    }else{
      bleMouse.release(MOUSE_LEFT);
      Bandera3= true;
      
    }
    ultimoTiempo = tiempoActual;
  }
  
}

void IRAM_ATTR presionado2() {
  unsigned long tiempoActual2 = millis();
  if ((tiempoActual2 - ultimoTiempo2) > Periodo) {
    Bandera2 = !bleMouse.isPressed(MOUSE_RIGHT);
    if(Bandera2){
      bleMouse.press(MOUSE_RIGHT);
    }else{
      bleMouse.release(MOUSE_RIGHT);
      Bandera3= true;      
    }
    ultimoTiempo2 = tiempoActual2;
  }
  
}

void setup() {
  Serial.begin(115200);
  bleMouse.begin();
  pinMode(Button, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Button), presionado, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Button2), presionado2, CHANGE);
}

void loop() {
  reinicio();
}

void reinicio(){
  if(digitalRead(Button) || digitalRead(Button2)){
    if(bleMouse.isPressed(MOUSE_RIGHT) || bleMouse.isPressed(MOUSE_LEFT)){
      uTiempo = millis();
      if(uTiempo-iTiempo>inter){
        bleMouse.release(MOUSE_RIGHT);
        bleMouse.release(MOUSE_LEFT);
        iTiempo = uTiempo;
      }
    }
  }
}
void macro1(){
  if(Bandera && Bandera2){
    bleMouse.press(MOUSE_BACK);
    Bandera3= false;
  }else{
  bleMouse.release(MOUSE_BACK);
  }
}


