#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
//Definimos los pines a usar del Sensor RFID
#define RST_PIN  D0
#define SS_PIN  D8
#define boton 10
#define Periodo 200
//Definimos las 3 urls que se usaran para almacenar datos en la base de datos
String URL = "http://192.168.43.23/Rfid_database.php";
String URL2 = "http://192.168.43.23/Rfid_search.php";
String URL3 = "http://192.168.43.23/Rfid_mod.php";
//Se define las variables necesarias para el programa
int saldo = 0;
int nuevoSaldo = 0;
bool Bandera = true;
bool Bandera2 = true;
bool modo = true;
String saldoRecibido;
String Tarjeta = "";
String payload = "";
String idRecibido2 = "";
//Se define el Wifi al que se conectara la esp
const char* ssid = "fadroga";
const char* password = "holaquehace.";
volatile unsigned long ultimoTiempo = 0;
bool opcion = true;
//Iniciamos los objetos de wifi, php, RFID y de la LCD
WiFiClient client;    
HTTPClient http;
MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27,16,2);
//Se define la estructura principal del programa
typedef enum{
  Espera,
  Lectura,
  Verificacion,
  Creacion,
  Recarga,
  Juego
}tarjetero;
tarjetero Paso = Espera;
//Se define la estructura para mostrar mensajes en la LCD
int mostrar = 0;


void Est(){
  if(digitalRead(boton) && modo == false){
    unsigned long tiempoActual = millis();
    if ((tiempoActual - ultimoTiempo) > Periodo) {
        modo = true;
        Serial.println("Modo Recarga");
        espera(10);
        espera(1);
    }
      ultimoTiempo = tiempoActual;
    }
  if(!digitalRead(boton) && modo == true){
    unsigned long tiempoActual = millis();
    if ((tiempoActual - ultimoTiempo) > Periodo) {
          modo = false;
          Serial.println("Modo Juego");
          espera(11);
          espera(1);
    }
    ultimoTiempo = tiempoActual;
  } 
}


void setup() {
  Serial.begin(9600);
  Serial.println("--------------------------------------------------");
  connectWiFi();
  SPI.begin();	
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  pinMode(boton, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(boton), Est, CHANGE);
}

void loop() {
  
  switch(Paso){
    case Espera:
    
    if(Bandera2){
      Serial.println("    Ingrese Una Tarjeta");
      Bandera2 = false;
      espera(1);
    }
    Est();
      if ( ! mfrc522.PICC_IsNewCardPresent())	
        return;	
      if ( ! mfrc522.PICC_ReadCardSerial()) 	
        return;
      Bandera = true;
      Paso = Lectura;  
    break;
    case Lectura:
      
      lecturaTarjeta();
      espera(2);
      Bandera2 = true;
      Paso = Verificacion;
    break;
    case Verificacion:
      buscar_SQL();
//      Paso = Espera;
    break;
    case Creacion:
      agregar_SQL();
      Paso = Espera;
    break;
    case Recarga:
      
      recarga_SQL();
      
    break;
    case Juego:
      
      descuento_SQL();
      
    break;
  }
         
}

void connectWiFi(){
  WiFi.mode(WIFI_OFF);
  delay(1000);
  //This line hides the viewing of ESP as wifi hotspot
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.print("Connectado a: "); Serial.println(ssid);
  Serial.print("Direccion IP: "); Serial.println(WiFi.localIP());
  Serial.println("--------------------------------------------------");
}

void lecturaTarjeta(){

  Serial.print("UID:");				
  for (byte i = 0; i < mfrc522.uid.size; i++) {	
    if (mfrc522.uid.uidByte[i] < 0x10){		
      Serial.print(" 0");			
      }
      else{
      Serial.print(" ");
      Tarjeta += " ";		
      }
    Tarjeta += String(mfrc522.uid.uidByte[i], HEX);
  }
  Tarjeta.toUpperCase();                                     
  Serial.println(Tarjeta);				
  mfrc522.PICC_HaltA();
  
}

void agregar_SQL(){
  String postData = "idCard=" + String(Tarjeta) + "&saldo=" + String(saldo);   

  
  http.begin(client, URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int httpCode = http.POST(postData);
  String payload = http.getString();
  Serial.println(payload); 
  Serial.println("--------------------------------------------------");

  Tarjeta = "";
}

void buscar_SQL(){
  String postData = "idCard=" + String(Tarjeta);
  http.begin(client, URL2);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.POST(postData);
  String payload = http.getString();
  //Serial.println(payload);
  if(payload == " ,"){
    Serial.println("Tarjeta No Registrada");
    espera(3);
    espera(4);
    Paso = Creacion;
  }else{
    int comaIndex = payload.indexOf(",");
    saldoRecibido = payload.substring(0, comaIndex);
    String idRecibido = payload.substring(comaIndex + 1);
    idRecibido2 = idRecibido;
    Serial.println("    Tarjeta Registrada");
    Serial.println("ID de la tarjeta: " + idRecibido);
    Serial.println("Saldo Disponible: ");
    Serial.println(saldoRecibido);
    espera(5);
    if(modo){
      Paso = Recarga;
    }else{
      Paso = Juego;
    }
  }
//  Paso = Espera;

}
void descuento_SQL(){
  if(saldoRecibido.toInt() >= 5){
    nuevoSaldo = saldoRecibido.toInt() -5;
    String postData = "idCard=" + String(Tarjeta) + "&nuevoSaldo=" + String(nuevoSaldo);
    http.begin(client, URL3);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.println("  Moneda Insertada");
    Serial.print("Saldo disponible: ");
    Serial.println(nuevoSaldo);
    Serial.println("--------------------------------------------------");
    Tarjeta="";
    Bandera2 = true;
    espera(9);
    espera(6);
    Paso = Espera;
  }else{
        Serial.println("Saldo insuficiente");
        Serial.println("Porfavor realice una recarga");
        Serial.println("--------------------------------------------------");
        espera(7);
        espera(12);
        Tarjeta="";
        Bandera2 = true;
        Paso = Espera;
      }
}
void recarga_SQL(){
  if(Bandera){
    Serial.println("    Ingrese Saldo:");
    espera(5);
    Bandera = false;
  }
  if(Serial.available()>1){
    nuevoSaldo = Serial.parseInt();
    //Serial.println(nuevoSaldo);
    if(nuevoSaldo > 0){
      //Serial.println(nuevoSaldo);
      //delay(500);
      nuevoSaldo = nuevoSaldo + saldoRecibido.toInt();
      String postData = "idCard=" + String(Tarjeta) + "&nuevoSaldo=" + String(nuevoSaldo);
      http.begin(client, URL3);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      int httpCode = http.POST(postData);
      String payload = http.getString();
      Serial.print("Saldo actualizado: ");
      Serial.println(nuevoSaldo);
      Serial.println(payload);
      Serial.println("--------------------------------------------------");
      espera(8);1
      espera(6);
      Tarjeta="";
      Bandera2 = true;
      Paso = Espera;
    }if(nuevoSaldo == 0){
      //Serial.println("Monto Invalido");
      Bandera = true;
    }
  } 
}

void mostrar_lcd(){
  switch(mostrar){
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Ingrese Una"); 
      lcd.setCursor (0,1);
      lcd.print("   Tarjeta");
    break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UID: "); 
      lcd.setCursor (0,1);
      lcd.print(Tarjeta);
    break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tarjeta No"); 
      lcd.setCursor (0,1);
      lcd.print("   Registrada");
    break;
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Registro Exitoso"); 
      lcd.setCursor (0,1);
      lcd.print("!!!");
    break;
    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ID: ");
      lcd.setCursor(4, 0);
      lcd.print(idRecibido2);
      lcd.setCursor(0, 1);
      lcd.print("Saldo: ");
      lcd.setCursor(7, 1);
      lcd.print(saldoRecibido);
    break;
    case 6:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Saldo");
      lcd.setCursor(0,1);
      lcd.print("Disponible:");
      lcd.setCursor(12,1);
      lcd.print(nuevoSaldo);
    break;
    case 7:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Saldo");
      lcd.setCursor(0,1);
      lcd.print("Insuficiente");
    break;
    case 8:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Recarga Exitosa!");
    break;
    case 9:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Monedada");
      lcd.setCursor(0,1);
      lcd.print("    Insertada");
    break;
    case 10:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Modo Recarga");
    break;
    case 11:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Modo Juego");
    break;
    case 12:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Realice una");
      lcd.setCursor(0,1);
      lcd.print("Recarga");
    break;
  }
}

void espera(int pos){
  mostrar = pos;
  mostrar_lcd();
  delay(2000);
}
