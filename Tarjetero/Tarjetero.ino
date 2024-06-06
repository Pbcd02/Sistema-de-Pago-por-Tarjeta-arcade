#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN  D0
#define SS_PIN  D8

String URL = "http://192.168.43.171/Rfid_database/Rfid_database.php";
String URL2 = "http://192.168.43.171/Rfid_database/Rfid_search.php";
String URL3 = "http://192.168.43.171/Rfid_database/Rfid_mod.php";

int saldo = 0;
int nuevoSaldo = 0;
String saldoRecibido;
String Tarjeta = "";
const char* ssid = "Virgin";
const char* password = "holaquehace.";
bool Bandera = true;
bool Bandera2 = true;

WiFiClient client;    
HTTPClient http;

typedef enum{
  Espera,
  Lectura,
  Verificacion,
  Creacion,
  Recarga
}tarjetero;
tarjetero Paso = Espera;

MFRC522 mfrc522(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("--------------------------------------------------");
  connectWiFi();
  SPI.begin();	
  mfrc522.PCD_Init();		
}

void loop() {
  
  switch(Paso){
    case Espera:
    if(Bandera2){
      Serial.println("    Ingrese Una Tarjeta");
      Bandera2 = false;
    }
      if ( ! mfrc522.PICC_IsNewCardPresent())	
        return;	
      if ( ! mfrc522.PICC_ReadCardSerial()) 	
        return;
      Bandera = true;
      Paso = Lectura;  
    break;
    case Lectura:
      lecturaTarjeta();
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
  Serial.println(payload);
  if(payload == " ,"){
    Serial.println("Tarjeta No Registrada");
    Paso = Creacion;
  }else{
    int comaIndex = payload.indexOf(",");
    saldoRecibido = payload.substring(0, comaIndex);
    String idRecibido = payload.substring(comaIndex + 1);
    Serial.println("    Tarjeta Registrada");
    Serial.println("ID de la tarjeta: " + idRecibido);
    Serial.println("Saldo Disponible: " + saldoRecibido);
    Paso = Recarga;
  }
//  Paso = Espera;

}

void recarga_SQL(){
  if(Bandera){
    Serial.print("    Ingrese el saldo: ");
    Bandera = false;
  }
  if(Serial.available()>0){
    nuevoSaldo = Serial.parseInt();
    if(nuevoSaldo > 0){
      Serial.println(nuevoSaldo);
      delay(500);
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
      Tarjeta="";
      Bandera2 = true;
      Paso = Espera;
    }
  } 
}