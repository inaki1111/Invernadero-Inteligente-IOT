 #include <LiquidCrystal.h>

#include <common.h>
#include <Firebase.h>
#include <FirebaseESP8266.h>
#include <FirebaseFS.h>
#include <Utils.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
    

const int DHTPin = D3;  //Conectar sensor DHT a pin D3 = GPIO_0
#define DHTTYPE DHT11 //Definimos el tipo de sensor de humedad (DHT11 o DHT22)
const int relay = 4;   //D4  CHECAR QUE LA CONEXIÓN ESTE EN NORMALMENTE CERRADO

int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

String ssid           = "Tec-IoT";
String password       = "spotless.magnetic.bridge";

byte cont = 0;
byte max_intentos = 50;
DHT dht(DHTPin, DHTTYPE);


//Conexión a Firebase
#define FIREBASE_HOST "sistema-de-riego-agrotec-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "lTNnDTdMelDX8IhdyJigAXXFWRXxM5WopR4woNx1"
String path = "/FuncionaPls";

FirebaseData firebaseData;

void printResult(FirebaseData &data);
void causaError(void);
void inforsetluzsensor(void);
void inforgetluzsensor(void);

void setup()
{
  Serial.begin(115200);
  dht.begin();
  Serial.println("\n");

  //inicicalizar relevador lampara

  pinMode(relay, OUTPUT);

  
   // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  //Conexion Wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and cont < max_intentos){
    cont++;
    delay(500);
    Serial.print(".");
    }
  
  delay(2000);
  Serial.print("\n");
  if (cont < max_intentos){
    Serial.println("****************************************");
    Serial.print("Conectado a la red WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("macAddress: ");
    Serial.println(WiFi.macAddress());
    Serial.println("****************************************");
    }
    else { // No se conecto
      Serial.println("--------------------------------------");
      Serial.println("Error de conexion");
      Serial.println("--------------------------------------");
    }
     
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    Firebase.reconnectWiFi(true);

    //Establezca el tiempo de espera de lectura de la base de datos en 1 minuto (máximo 15 minutos)
    Firebase.setReadTimeout(firebaseData, 1000 * 60);

    //Tamaño y  tiempo de espera de escritura, tiny (1s), small (10s), medium (30s) and large (60s).
    //tiny, small, medium, large and unlimited.
    Firebase.setwriteSizeLimit(firebaseData, "medium");
    
    Serial.println("------------------------------------");
    Serial.println("ACTUALIZAR EL ESTADO DEL INVERNADERO");
    //Also can use Firebase.set instead of Firebase.setDouble
    if (Firebase.setInt(firebaseData, path + "/Control/Humedad_1", 0)){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setInt(firebaseData, path + "/Control/Temperatura_2", 1)){InforSetLuzSensor();}else{CausaError();}

delay(2000);
    Serial.println("------------------------------------");
    Serial.println("ACTUALIZAR EL ESTADO DE LOS SENSORES");
    
    float humedad = dht.readHumidity();  //Lectura de Humedad
    float temperatura = dht.readTemperature(); // Lectura de Temperatura
    Serial.println(temperatura);
    Serial.println(humedad);

    if (Firebase.pushFloat(firebaseData, path + "/Sensores/Humedad", humedad)){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.pushFloat(firebaseData, path + "/Sensores/Temperatura", temperatura)){InforSetLuzSensor();}else{CausaError();}

delay(2000);
    Serial.println("------------------------------------");
    Serial.println("ACTUALIZAR LOS MENSAJE DE ADVERTENCIA");
    if (Firebase.setString(firebaseData, path + "/Mensajes/humedadP_1", "HUMEDAD DEPLORABLE")){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setString(firebaseData, path + "/Mensajes/humedadP_2", "HUMEDAD ACEPTABLE")){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setString(firebaseData, path + "/Mensajes/humedadP_3", "HUMEDAD OPTIMA")){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setString(firebaseData, path + "/Mensajes/temperaturaP_1", "TEMPERATURA DEPLORABLE")){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setString(firebaseData, path + "/Mensajes/temperaturaP_2", "TEMPERATURA ACEPTABLE")){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.setString(firebaseData, path + "/Mensajes/temperaturaP_3", "TEMPERATURA OPTIMA")){InforSetLuzSensor();}else{CausaError();}

delay(2000);
    Serial.println("------------------------------------");
    Serial.println("  LEER  EL  ESTADO  DE  LAS  LUCES  ");
    if (Firebase.getInt(firebaseData, path + "/Control/Humedad_1" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getInt(firebaseData, path + "/Control/Temperatura_2" )){InforGetLuzSensor(); }else{CausaError(); }
delay(2000);
    Serial.println("------------------------------------");
    Serial.println("   LEER EL ESTADO DE LOS SENSORES   ");
    if (Firebase.setFloat(firebaseData, path + "/Sensores/Humedad",humedad)){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.setFloat(firebaseData, path + "/Sensores/Temperatura",temperatura)){InforGetLuzSensor(); }else{CausaError(); }
delay(2000);
    Serial.println("------------------------------------");
    Serial.println("      LEER    LOS     MENSAJES      ");
    if (Firebase.getString(firebaseData, path + "/Mensajes/humedadP_1" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getString(firebaseData, path + "/Mensajes/humedadP_2" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getString(firebaseData, path + "/Mensajes/humedadP_3" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getString(firebaseData, path + "/Mensajes/temperaturaP_1" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getString(firebaseData, path + "/Mensajes/temperaturaP_2" )){InforGetLuzSensor(); }else{CausaError(); }
    if (Firebase.getString(firebaseData, path + "/Mensajes/temperaturaP_3" )){InforGetLuzSensor(); }else{CausaError(); }

  //get, getInt, getFloat, getDouble, getBool, getString, getJSON, getArray, getBlob, getFile.
  //set, setInt, setFloat, setDouble, setBool, setString, setJSON, setArray, setBlob and setFile.
    
}//llave setup   
void loop() 
{   
  delay(2000);

  digitalWrite(relay,LOW);  //poner la lampara en low mediante el relevador
  delay(5000);
  digitalWrite(relay,HIGH);
  

  float humedad = dht.readHumidity();  //Lectura de Humedad
  float temperatura = dht.readTemperature(); //Lectura de Temperatura

 
  String temp = "Temperatura: " + String(temperatura) + "grados";

  String hum =  "Humedad: " + String(humedad) + "%";
   
  lcd.setCursor(0,0);
  lcd.print(temp);
  lcd.setCursor(0,1);
  lcd.print(hum);
  lcd.clear();
  
   
   Serial.print("Humedad: ");
   Serial.print(humedad);
   Serial.print(" %");
   Serial.print("\nTemperatura: ");
   Serial.print(temperatura);
   Serial.print(" *C");
   Serial.println("\n********************************");

    if (Firebase.pushFloat(firebaseData, path + "/Sensores/Humedad", humedad)){InforSetLuzSensor();}else{CausaError();}
    if (Firebase.pushFloat(firebaseData, path + "/Sensores/Temperatura", temperatura)){InforSetLuzSensor();}else{CausaError();}

}


void InforGetLuzSensor(void)
{
  Serial.println("Aprobado");
  Serial.println("Ruta: " + firebaseData.dataPath());
  Serial.println("Tipo: " + firebaseData.dataType());
  Serial.println("ETag: " + firebaseData.ETag());
  Serial.print("Valor: ");
  printResult(firebaseData);
  Serial.println("------------------------------------");
  Serial.println(); 
}

void InforSetLuzSensor(void)
{
  Serial.println("Aprobado");
  Serial.println("Ruta: " + firebaseData.dataPath());
  Serial.println("Tipo: " + firebaseData.dataType());
  Serial.println("ETag: " + firebaseData.ETag());
  Serial.print("Valor: ");
  printResult(firebaseData);
  Serial.println("------------------------------------");
  Serial.println(); 
}

void CausaError(void)
{
  Serial.println("ERROR");
  Serial.println("RAZON: " + firebaseData.errorReason());
  Serial.println("------------------------------------");
  Serial.println();
}

void printResult(FirebaseData &data)
{
    if (data.dataType() == "int")
        Serial.println(data.intData());
    else if (data.dataType() == "float")
        Serial.println(data.floatData(), 2);
    else if (data.dataType() == "double")
        printf("%.9lf\n", data.doubleData());
    else if (data.dataType() == "boolean")
        Serial.println(data.boolData() == 1 ? "true" : "false");
    else if (data.dataType() == "string")
        Serial.println(data.stringData());
}
