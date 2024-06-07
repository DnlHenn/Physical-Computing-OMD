//---------------------------------- Wichtige Parameter-------------------------------------------//

int maxPeople = 1;             // Maximale Anzahl an Personen in einem Raum; bei Überschreitung ertönt der Buzzer
int sensitivity = 1;          // Höhere Werte für geringere Sensitivität; geringe Werte für höhere Sensitivität
int buz_buf = 0;               // Variable, deren Wert einmalig erhöht wird, sobald die Raumkapazität überschrritten wurde

//------------------------------------------------------------------------------------------------//

#include "rgb_lcd.h"

rgb_lcd lcd;

// Hintergrundfarbe des LCD Bildschirms
const int colorR_default = 0;
const int colorG_default = 255;
const int colorB_default = 0;
const int colorR_alert = 255;
const int colorG_alert = 0;
const int colorB_alert = 0;

// Startwert der Anzahl an Personen im Raum
int currentPeople = 0;

// Buzzer-Pin
int buzzer = 8;

// Button-Pins
const int buttonPin_Reset = 2;  // Pin, an dem der Knopf angeschlossen ist
int buttonState_Reset = 0;     // Variable zum Speichern des Knopfstatus

const int buttonPin_Plus = 3;  // Pin, an dem der Knopf angeschlossen ist
int buttonState_Plus = 0;     // Variable zum Speichern des Knopfstatus

const int buttonPin_Minus = 12;  // Pin, an dem der Knopf angeschlossen ist
int buttonState_Minus = 0;     // Variable zum Speichern des Knopfstatus

// Sensoren Pins
int sensor1[] = {4,5};
int sensor2[] = {6,7};
int sensor1Initial;
int sensor2Initial;

// Instanz der String-Klasse; hier (vorerst) eine leere Zeichenfolge, wird später anhand der Messung gefüllt
String sequence = "";

// wird mit jedem Loop erhöht, in dem die Daten inkonsistent sind (zzt. bis 150)
int timeoutCounter = 0;

void setup() {
  // Setup code
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  pinMode(buttonPin_Reset, INPUT_PULLUP);

  // Setup vom LCD Bildschirm (16 Zeichenm zwei Zeilen)
  lcd.begin(16, 2); 
  lcd.setRGB(colorR_default, colorG_default, colorB_default);

  // Zeige eine Nachricht an
  lcd.print("ROOM OCCUPANCY:");

  delay(1000);

  // Initiale Messung der Distanz vom Sensor zum nächsten Objekt 
  sensor1Initial = measureDistance(sensor1);
  sensor2Initial = measureDistance(sensor2);

}

void loop() {
  // Fortlaufende Messung beider Sensoren vom Sensor zum nächsten Objekt
  int sensor1Val = measureDistance(sensor1);
  int sensor2Val = measureDistance(sensor2);

  // Wenn in der fortlaufenden Messung die Distanz von Sensor 1 kleiner ist als die Distanz der initialen Messung von Sensor 1, speichere in sequence eine 1 
  if (sensor1Val < sensor1Initial - sensitivity && sequence.charAt(0) != '1'){
    sequence += "1";
  }
  // Wenn in der fortlaufenden Messung die Distanz von Sensor 2 kleiner ist als die Distanz der initialen Messung von Sensor 2, speichere in sequence eine 2
  else if (sensor2Val < sensor2Initial - sensitivity && sequence.charAt(0) != '2'){
    sequence += "2";
  }

  // Wenn Fall 1 zuerst eingetroffen ist und danach Fall 2 (erster Sensor zuerst, danach zweiter), dann erhöhe die aktuelle Personenanzahl um 1 und leere sequence
  if (sequence.equals("12")) {
    currentPeople++;  
    sequence="";
    delay(550);
  }
  // Wenn Fall 2 zuerst eingetroffen ist und danach Fall 1 (zweiter Sensor zuerst, danach erster), dann reduziere die aktuelle Personenanzahl um 1 und leere sequence
  else if (sequence.equals("21") && currentPeople > 0) {
    currentPeople--;  
    sequence="";
    delay(550);
  }

  // Leere sequence (reset), wenn nicht valide Daten zusammenkommen oder timeOut überschritten wird
  if (sequence.length() > 2 || sequence.equals("11") || sequence.equals("22") || timeoutCounter > 150){
    sequence="";  
  }

  // Sobald sequence gefüttert wurde, erhöhe den Counter mit jedem Loop um eins, ist die Länge 0 oder höher, setze den Counter wieder auf null
  if (sequence.length() == 1) { 
    timeoutCounter++;
  } 
  else {
    timeoutCounter = 0;
  }
  buttonPush_Reset();
  buttonPush_Plus();
  buttonPush_Minus();

  // Zeige die derzeitige Anzahl an Personen an
  lcd.setCursor(0, 1);
  lcd.print(currentPeople);

  
  updateDisplay();

  checkBuzzer();
}

// Funktion zum Ändern der Hintergrundfarbe des Bildschirms abhängig von der Anzahl der Personen im Raum
void updateDisplay() {
  lcd.setCursor(0, 1);
  lcd.print(currentPeople);
  lcd.print(" (max.: ");
  lcd.print(maxPeople);
  lcd.print(")");
  
  if (currentPeople >= maxPeople) {
    lcd.setRGB(colorR_alert, colorG_alert, colorB_alert);
  } 
  else {
    lcd.setRGB(colorR_default, colorG_default, colorB_default);
  }
}

// Funktion zum Abspielen eines Tons, sofern die angegebene maximale Personenanzahl überschritten wurde
void checkBuzzer() {
  if (currentPeople > maxPeople) {
      if (buz_buf == 0){
      tone(buzzer, 50, 100);
      delayMicroseconds(10);
      }
      buz_buf = 1;

  } 
  else {
    noTone(buzzer);
    buz_buf = 0;
  }
}

// Funktion zum Drücken des Reset-Buttons (currentPeople wird resetted)
void buttonPush_Reset () {
  buttonState_Reset = digitalRead(buttonPin_Reset);

  // Überprüfe, ob der Knopf gedrückt wurde
  if (buttonState_Reset == HIGH) {
    currentPeople = 0;
    maxPeople = 1;
  }
  // Kurze Verzögerung, um das Entprellen zu vermeiden
  delay(65);
}

// Funktion zum Drücken des Plus-Buttons (maxPeople wird erhöht)
void buttonPush_Plus () {
  buttonState_Plus = digitalRead(buttonPin_Plus);

  // Überprüfe, ob der Knopf gedrückt wurde
  if (buttonState_Plus == HIGH) {
    maxPeople++;
  }
  // Kurze Verzögerung, um das Entprellen zu vermeiden
  delay(65);
}

// Funktion zum Drücken des Minut-Buttons (maxPeople wird reduziert)
void buttonPush_Minus () {
  buttonState_Minus = digitalRead(buttonPin_Minus);

  // Überprüfe, ob der Knopf gedrückt wurde
  if (buttonState_Minus == HIGH && maxPeople > 0) {
    maxPeople--;
  }
  // Kurze Verzögerung, um das Entprellen zu vermeiden
  delay(65);
}


//------------------------- Funktion, welche die Distanzmessung des Sensors zurückgibt ----------------------//
// a[0] = echo, a[1] = trig
int measureDistance(int a[]) {
  pinMode(a[1], OUTPUT);
  digitalWrite(a[1], LOW);
  delayMicroseconds(2);
  digitalWrite(a[1], HIGH);
  delayMicroseconds(10);
  digitalWrite(a[1], LOW);
  pinMode(a[0], INPUT);
  long duration = pulseIn(a[0], HIGH, 100000); // long = Variablen mit erweiterter Speichergröße | pulseIn liest aus, ob der Pin a[0] von LOW zu HIGH gewechselt ist, woraufhin ein Timer startet, bis er wieder auf LOW wechselt - Rückgabewert: Länge des Impulses
  return duration / 29 / 2; // Rückgabewert der Funktion
}