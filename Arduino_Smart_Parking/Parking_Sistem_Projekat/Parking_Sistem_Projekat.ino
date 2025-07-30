#include <DHT.h>
#include <DHT_U.h>
#include "DFRobotDFPlayerMini.h"

#include <DHT11.h>

#include <SoftwareSerial.h> // SoftwareSerial library


#include <LiquidCrystal_I2C.h>

#define servoPin 44

#include <U8g2lib.h>
#include <Servo.h> 



#define trigPin_3 43
#define echoPin_3 42








Servo ServoRampa;


DFRobotDFPlayerMini myDFPlayer;



#define DHTPIN 42     // Pin na koji je povezan DHT senzor
#define DHTTYPE DHT11   // Tip senzora (DHT11 ili DHT22)

DHT dht(DHTPIN, DHTTYPE);  // Kreiramo instancu DHT senzora

int rxPin = 17; // RX2 pin na Arduino Mega
int txPin = 16; // TX2 pin na Arduino Mega

bool alarmAktiviran = false;



int IR_Ulaz = 6;
int IR_Izlaz = 7;

int ParkingMesta = 3; 
int ZauzetaMesta = 0;          

int Prvo_Stanje = 0;
int Drugo_Stanje = 0;

int piezoPin = 26; 

long distance;





long distance_2;

long distance_3;


int buttonUlaz = 4;

int buttonState = 0;  // Trenutno stanje tastera

int lastButtonState = 0;  // Prethodno stanje tastera

bool isOpen = false; 

bool carDetected = false;  
bool carDetected2 = false;  

const int AnalogWaterPin = A5;

const int SemaforCrvno_1 = 22;
const int SemaforZuto_1 = 23;
const int SemaforZeleno_1 = 24;


const int SemaforCrvno_2 = 38;
const int SemaforZuto_2 = 39;
const int SemaforZeleno_2 = 40;

const int SenzorPlamena = A0;
const int pragAktivacije = 60; // Prag za aktivaciju alarma

const int PumpPin = A2; 
const int relayPin = A4;


const int VentilatorPin = A8;
const int ReleyPinFan = A9;

bool espConnected = false;  // Promenljiva koja prati status konekcije ESP8266



enum StanjeSemafora {
  CRVENO,
  ZELENO,
  BLINK_ZELENO,
  ZUTO_1,
  ZUTO_2
};

StanjeSemafora trenutnoStanje = CRVENO;

StanjeSemafora trenutnoStanje2 = ZELENO;

unsigned long prethodniMillis = 0;

unsigned long prethodniMillis2 = 0;

const long trajanjeCrvenog = 7000;  
const long trajanjeZelenog = 7000;  
const long trajanjeBlinkanja = 4000; 
const long trajanjeZutog = 2000;   

unsigned long blinkMillis = 0;  
bool zelenoBlinkStanje = false; 
//----------------------------------------------------------------------------
bool zelenoBlinkStanje2 = false; /

unsigned long blinkMillis2 = 0;


// Definišemo minimalnu i maksimalnu udaljenost (u centimetrima)
const int minDistance = 5;    
const int maxDistance = 10;  

// Ako koristite ST7920 sa SPI interfejsom
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

void setup()
 {

  dht.begin();  
  Serial.begin(9600);    
  Serial2.begin(115200); 
  Serial.println("Arduino Mega ready");

  Serial2.println("Are you there, ESP8266?"); 

  pinMode(PumpPin, INPUT_PULLUP);
  pinMode(relayPin, OUTPUT);   
  digitalWrite(relayPin, LOW);       

  

  pinMode(VentilatorPin,INPUT_PULLUP);

  pinMode(ReleyPinFan, OUTPUT);
  digitalWrite(ReleyPinFan, LOW); 

 pinMode(buttonUlaz, INPUT_PULLUP);   


  pinMode(31, OUTPUT); 
  pinMode(30, INPUT);  

  pinMode(50, OUTPUT); 
  pinMode(51, INPUT);   


  pinMode(45, OUTPUT);  
  pinMode(44, INPUT);   
  u8g2.begin(); 
  u8g2.setContrast(255); 
  pinMode(IR_Ulaz, INPUT);
  pinMode(IR_Izlaz, INPUT);





  pinMode(SemaforCrvno_1,OUTPUT);
  pinMode(SemaforZuto_1,OUTPUT);
  pinMode(SemaforZeleno_1,OUTPUT);

  digitalWrite(SemaforCrvno_1, LOW);
  digitalWrite(SemaforZuto_1, LOW);
  digitalWrite(SemaforZeleno_1, LOW);

  pinMode(SemaforCrvno_2, OUTPUT);
  pinMode(SemaforZuto_2, OUTPUT);
  pinMode(SemaforZeleno_2, OUTPUT);

  digitalWrite(SemaforCrvno_2, LOW);
  digitalWrite(SemaforZuto_2, LOW);
  digitalWrite(SemaforZeleno_2, LOW);

  pinMode(SenzorPlamena,INPUT);

   

  trenutnoStanje = CRVENO;
  prethodniMillis = millis();

  trenutnoStanje2 = ZELENO;
  prethodniMillis2 = millis();
    
  ServoRampa.attach(servoPin);
  ServoRampa.write(0);





}

void loop()
{
  Semafori();
  ProveraKonekcije();
  KontrolaVodenePumpe();
  KontrolaVentilacije();
  Vodostaj();
  kontrolaUlazaIzlaza();
  DHTSenzor();
  proveriSenzorPlamena();

//----------------------------------NIVO VODE-------------------------

//-----------------------------------------PARKING SENZOR 1-----------
long distance = readUltrasonic(31, 30);
long distance_2 = readUltrasonic(50, 51);
  controlBuzzer(distance);
  delay(100); 
  
  controlBuzzer(distance_2);  /
    delay(100); 

  delay(100); 

  if (distance < 5) 
  {
    if (!carDetected) 
    {
      carDetected = true; 
      if (ParkingMesta > 0) 
      {
        ParkingMesta--;  
        ZauzetaMesta++;
      }
    }
  }
  else 
  {
    if (carDetected)
    {
      carDetected = false;
      ParkingMesta++;
      ZauzetaMesta--;
    }
  }

  
  if (distance_2 < 5) 
  {
    if (!carDetected2) 
    {
      carDetected2 = true; 
      if (ParkingMesta > 0) 
      {
        ParkingMesta--; 
        ZauzetaMesta++;
      }
    }
  }
  else 
  {
    if (carDetected2)
    {
      carDetected2 = false; 
      ParkingMesta++;
      ZauzetaMesta--;
    }
  }
//-----------------------------------------------
//---------------SPUŠTANJE RAMPE-----------------
//-----------------------------------------------
if(Prvo_Stanje==1 && Drugo_Stanje==1)
{
    ZatvoriRampu();
    Prvo_Stanje=0, Drugo_Stanje=0;
}
//----------------------------------------------
  u8g2.clearBuffer();         
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.setCursor(0, 10);
  u8g2.print("Slob. mesta: ");
  u8g2.print(ParkingMesta);
  u8g2.setCursor(0, 24);
  u8g2.print("Zauz. mesta: ");
  u8g2.print(ZauzetaMesta);
  u8g2.setCursor(0, 35);
  u8g2.print("P1: ");
  u8g2.print(distance);
  
  u8g2.setCursor(55, 35);
  u8g2.print("P2: ");
  u8g2.print(distance_2);

  u8g2.setCursor(55, 45);
  u8g2.print("P3: ");
  u8g2.print(distance_3);
//------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------
}


//----------------------------------------------------------------------VOIDI 
void proveriSenzorPlamena() {
  int stanjeVatre = analogRead(SenzorPlamena);
  Serial.println(stanjeVatre);

  if (stanjeVatre < pragAktivacije) {
    tone(piezoPin, 1500); 
  } else {
    noTone(piezoPin); 
  }
}
void DHTSenzor()
{
 // Očitavanje temperature i vlažnosti
  float vlaznost = dht.readHumidity();
  float temperatura = dht.readTemperature();

  // Provera da li su očitavanja uspešna
  if (isnan(vlaznost) || isnan(temperatura)) {
    Serial.println("Greska pri ocitavanju DHT senzora!");
    return;
  }

  // Ispisivanje očitanih vrednosti na serijski monitor
  Serial.print("Vlaznost: ");
  Serial.print(vlaznost);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" *C");

  delay(500); 
}
void prikaziTekst(int tekstIndex) 
{
  u8g2.clearBuffer();

  switch (tekstIndex) {
    case 0:
      u8g2.drawStr(0, 24, "Dobrodosli!");
      break;
  }

  u8g2.sendBuffer(); 
}
unsigned long prethodnoVremeUlaz = 0;
const unsigned long vremeCekanja = 100; 
void kontrolaUlazaIzlaza() 
{
  unsigned long trenutnoVreme = millis();
  // Provera stanja tastera za otvaranje rampe
  buttonState = digitalRead(buttonUlaz);
  if (buttonState != lastButtonState && (trenutnoVreme - prethodnoVremeUlaz >= vremeCekanja)) {
    if (buttonState == LOW) {
      OtvoriRampu();
      isOpen = true;
    }
    isOpen = !isOpen; 
    prethodnoVremeUlaz = trenutnoVreme;
  }
  
  if (digitalRead(IR_Ulaz) == LOW && Prvo_Stanje == 0) 
  {
    Prvo_Stanje = 1;
  }

  if (digitalRead(IR_Izlaz) == LOW && Drugo_Stanje == 0 && (trenutnoVreme - prethodnoVremeUlaz >= vremeCekanja)) 
  {
    Drugo_Stanje = 1;
    if (Prvo_Stanje == 0) 
    {
      OtvoriRampu();
    }
    prethodnoVremeUlaz = trenutnoVreme; 
  }



  lastButtonState = buttonState; 
}
void Vodostaj() {
  int NivoVode = analogRead(AnalogWaterPin); 

  u8g2.setCursor(0, 55); 
  u8g2.print("Voda:  ");
  u8g2.print(NivoVode); 
  u8g2.sendBuffer();  

  delay(100); 
}
void KontrolaVentilacije() 
{ 
  if(digitalRead(VentilatorPin) == HIGH)
  {
      digitalWrite(ReleyPinFan,HIGH);
  }
  else
  {
     digitalWrite(ReleyPinFan,LOW);
  }
}
  bool pumpState = false; 
void KontrolaVodenePumpe() {

  int StanjePumpe = digitalRead(PumpPin);
  digitalWrite(relayPin, (StanjePumpe == LOW) ? HIGH : LOW);
}
void ProveraKonekcije()
{
  float temperature = dht.readTemperature();
  if (Serial.available()) 
  {
    String command = Serial.readStringUntil('\n');
    command.trim(); 
    if (command == "OPEN") 
    {
        OtvoriRampu();
    }
    if (command == "CLOSE") 
    {
        ZatvoriRampu();
    }

    if (command == "RESERVE1") 
    {
      if (ParkingMesta > 0) {
        ParkingMesta--;  // Smanjuje broj slobodnih parking mesta
        ZauzetaMesta++;  // Povećava broj zauzetih parking mesta
        Serial.println("Parking mesto je rezervisano.");
      }
       else 
      {
         tone(piezoPin, 255); 
      }
    }
    if (command == "PLAY") 
    {
        Serial.println("Pustanje prve pesme...");
        myDFPlayer.setTimeOut(500);  // Serijski timeout 500ms
        myDFPlayer.volume(25);       // Podešavanje jačine zvuka (opseg od 0 do 30)
        myDFPlayer.EQ(0);            // Normalna ekvilizacija
        myDFPlayer.play(1);  // Pusti prvu pesmu (0001.mp3) sa SD kartice
    }
    if(command == "STOP")
    {
        myDFPlayer.stop();
    }
    if (command == "GET_STATUS") {
      int SlobodnaMesta = ParkingMesta - ZauzetaMesta;
      String statusMessage = String(SlobodnaMesta) + "," + String(ZauzetaMesta);
      Serial.println(statusMessage);  // Šaljemo ESP8266 broj slobodnih i zauzetih mesta
      Serial.println("Poslat status: " + statusMessage);  // Za debagovanje
    }
  }
}

void Semafori()
{
  unsigned long trenutniMillis = millis();
unsigned long trenutniMillis2 = millis();

  switch (trenutnoStanje)
  {
   case CRVENO:
      digitalWrite(SemaforCrvno_1, HIGH);
      digitalWrite(SemaforZuto_1, LOW);
      digitalWrite(SemaforZeleno_1, LOW);
      if (trenutniMillis - prethodniMillis >= trajanjeCrvenog) {
        trenutnoStanje = ZUTO_1;
        prethodniMillis = trenutniMillis;
      }
      break;
    case ZUTO_1:
      digitalWrite(SemaforCrvno_1, LOW);
      digitalWrite(SemaforZuto_1, HIGH);
      digitalWrite(SemaforZeleno_1, LOW);
      if (trenutniMillis - prethodniMillis >= trajanjeZutog) {
        trenutnoStanje = ZELENO;
        prethodniMillis = trenutniMillis;
      }
      break;
    case ZELENO:
      digitalWrite(SemaforCrvno_1, LOW);
      digitalWrite(SemaforZuto_1, LOW);
      digitalWrite(SemaforZeleno_1, HIGH);
      if (trenutniMillis - prethodniMillis >= trajanjeZelenog) {
        trenutnoStanje = BLINK_ZELENO;
        prethodniMillis = trenutniMillis;
        blinkMillis = trenutniMillis;
      }
      break;

    case BLINK_ZELENO:
      if (trenutniMillis - blinkMillis >= 500) {
        blinkMillis = trenutniMillis;
        zelenoBlinkStanje = !zelenoBlinkStanje;
        digitalWrite(SemaforZeleno_1, zelenoBlinkStanje ? HIGH : LOW);
      }
      if (trenutniMillis - prethodniMillis >= trajanjeBlinkanja) {
        trenutnoStanje = ZUTO_2;
        prethodniMillis = trenutniMillis;
      }
      break;

    case ZUTO_2:
      digitalWrite(SemaforCrvno_1, LOW);
      digitalWrite(SemaforZuto_1, HIGH);
      digitalWrite(SemaforZeleno_1, LOW);
      if (trenutniMillis - prethodniMillis >= trajanjeZutog) {
        trenutnoStanje = CRVENO;
        prethodniMillis = trenutniMillis;
      }
      break;
  }

  switch (trenutnoStanje2) {
    case ZELENO:
      digitalWrite(SemaforCrvno_2, LOW);
      digitalWrite(SemaforZuto_2, LOW);
      digitalWrite(SemaforZeleno_2, HIGH);
      if (trenutniMillis2 - prethodniMillis2 >= trajanjeZelenog) {
        trenutnoStanje2 = BLINK_ZELENO;
        prethodniMillis2 = trenutniMillis2;
        blinkMillis2 = trenutniMillis2;
      }
      break;

    case BLINK_ZELENO:
      if (trenutniMillis2 - blinkMillis2 >= 500) {
        blinkMillis2 = trenutniMillis2;
        zelenoBlinkStanje2 = !zelenoBlinkStanje2;
        digitalWrite(SemaforZeleno_2, zelenoBlinkStanje2 ? HIGH : LOW);
      }
      if (trenutniMillis2 - prethodniMillis2 >= trajanjeBlinkanja) {
        trenutnoStanje2 = ZUTO_1;
        prethodniMillis2 = trenutniMillis2;
      }
      break;

    case ZUTO_1:
      digitalWrite(SemaforCrvno_2, LOW);
      digitalWrite(SemaforZuto_2, HIGH);
      digitalWrite(SemaforZeleno_2, LOW);
      if (trenutniMillis2 - prethodniMillis2 >= trajanjeZutog) {
        trenutnoStanje2 = CRVENO;
        prethodniMillis2 = trenutniMillis2;
      }
      break;

    case CRVENO:
      digitalWrite(SemaforCrvno_2, HIGH);
      digitalWrite(SemaforZuto_2, LOW);
      digitalWrite(SemaforZeleno_2, LOW);
      if (trenutniMillis2 - prethodniMillis2 >= trajanjeCrvenog) {
        trenutnoStanje2 = ZUTO_2;
        prethodniMillis2 = trenutniMillis2;
      }
      break;
    case ZUTO_2:
      digitalWrite(SemaforCrvno_2, LOW);
      digitalWrite(SemaforZuto_2, HIGH);
      digitalWrite(SemaforZeleno_2, LOW);
      if (trenutniMillis2 - prethodniMillis2 >= trajanjeZutog) {
        trenutnoStanje2 = ZELENO;
        trenutniMillis2 = trenutniMillis2;
      }
      break;
  }
}

// Funkcija za spuštanje rampe
void OtvoriRampu()
{
  slowMove(ServoRampa, 0, 90, 10); 

}
void ZatvoriRampu() 
{
  slowMove(ServoRampa, 90, 0, 10); 
}
void PunParking()
{
  u8g2.setFont(u8g2_font_ncenB08_tr); // Postavljanje fonta
  u8g2.setCursor(0, 50);
  u8g2.print("Parking je pun!");
  u8g2.sendBuffer();           // Slanje bafera na displej
  tone(piezoPin, 255); 
  delay(1000);  // Pauza od 1 sekunde
  noTone(piezoPin);
}
//----------------------------------
//---------------------------------------

// Funkcija za sporo pomeranje servo motora
void slowMove(Servo &servo, int start, int end, int delayTime) 
{
  if (start < end) {
    for (int pos = start; pos <= end; pos++) {
      servo.write(pos);  // Postavljanje pozicije servo motora
      delay(delayTime);  // Pauza za usporavanje kretanja
    }
  } else {
    for (int pos = start; pos >= end; pos--) {
      servo.write(pos);  // Postavljanje pozicije servo motora
      delay(delayTime);  // Pauza za usporavanje kretanja
    }
  }
}
long readUltrasonic(int trigPin, int echoPin) 
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = (duration * 0.034) / 2;

  return distance;
}
void controlBuzzer(long distance) {
  if (distance > 0 && distance < 5) { // Podesite maksimalnu udaljenost za detekciju
    int frequency = map(distance, 0, 5, 2000, 100); // Invertovana logika: bliže objekat -> viša frekvencija
    tone(piezoPin, frequency, 100); // Pišti 100 ms
  } else {
    noTone(piezoPin); // Zaustavi pištanje ako nema detekcije ili je objekat predaleko
  }
}
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
void updateLCD() {
  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.setCursor(0, 10);
  u8g2.print("Slob. mesta: ");
  u8g2.print(ParkingMesta);
  u8g2.setCursor(0, 24);
  u8g2.print("Zauz. mesta: ");
  u8g2.print(ZauzetaMesta);
  u8g2.sendBuffer();
}
