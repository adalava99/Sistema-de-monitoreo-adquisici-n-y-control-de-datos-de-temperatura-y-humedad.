#include <Arduino_MKRIoTCarrier.h>
#include <SD.h>
#include "visuals.h"


MKRIoTCarrier carrier;
File dataFile;


//Defining variables
float minTemp;
float maxTemp;
float currentTemp;
float currentHum;
int count = 3;
unsigned long currentTime;
//Relay 1 for fan
//Relay 2 for ligth bulb


//Colors
uint32_t colorRed = carrier.leds.Color(127, 0, 0);
uint32_t colorGreen = carrier.leds.Color(0, 127, 0);
uint32_t colorBlue = carrier.leds.Color(0, 0, 127);
uint32_t noColor = carrier.leds.Color(0, 0, 0);

//Variables for temperature setting
float baseTemperature[] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
int i = 0;
float tempSet = 27;

//Declare pages
void welcome();
void settingTempPage();
void sensorsPage();
void manualHeat();
void manualCool();
void manualScreen();
void scanningManual();
void automaticScreen();
void registerSD();

//Declare their flags
bool button_0 = false;
bool manualHeatFlag = false;
bool manualCoolFlag = false;
bool welcomeFlag = false;


void setup() {
  CARRIER_CASE = true;
  Serial.begin(9600);
  
  
  //See if MKR IoT Carrier be initialized  
  Serial.println("Inicializando Carrier...");
  
  if (!carrier.begin()) {
    Serial.println("Carrier not connected, check connections");
    while (1);
  }
  
  Serial.println("Carrier inicializado.");

  //See if SD can be initialized  
  Serial.println("Inicializando tarjeta SD...");
  
  if (!SD.begin(SD_CS)) {
    Serial.println("Card failed, or not present");
    //don't do anything more:
    while (1);
  }
  
  Serial.println("Tarjeta inicializada.");
  
}

void loop() {
  if (!welcomeFlag) {
    welcome();
    welcomeFlag = true;
  }

  carrier.Buttons.update();
  minTemp = tempSet - 2;
  maxTemp = tempSet + 2;

  if (carrier.Buttons.getTouch(TOUCH3)) {
    manualHeatFlag = true;
    manualScreen();
    delay(4000);
    while (manualHeatFlag) {
      manualHeat();
    }
  }

  if (carrier.Buttons.getTouch(TOUCH4)) {
    manualCoolFlag = true;
    manualScreen();
    delay(4000);
    while (manualCoolFlag) {
      manualCool();
    }
  }

  sensorsPage();

}


//Funtions
// Welcome
void welcome() {
  carrier.display.fillScreen(0x0000);
  //Basic configuration for the text
  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.drawBitmap(50, 40, arduino_logo, 150, 106, 0x253);  //0x00979D);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(35, 160);
  carrier.display.print("Loading");
  //Printing a three dots animation
  for (int i = 0; i < 3; i++) {
    carrier.display.print(".");
    delay(1000);
  }

  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(65, 30);
  carrier.display.print("Arduino ");
  carrier.display.setTextSize(2);
  carrier.display.setCursor(30, 60);
  carrier.display.print("MKR IoT Carrier");

  carrier.display.setTextSize(2);
  carrier.display.setCursor(24, 120);
  carrier.display.println("Practicas");
  carrier.display.setCursor(25, 140);
  carrier.display.println("Comunitarias");
  carrier.display.setCursor(75, 160);
  carrier.display.println("ESPOL");
  delay(4500);
}


//Set base temperature
void settingTempPage() {

  carrier.Buttons.update();
  int arrayLen = sizeof(baseTemperature) / sizeof(baseTemperature[0]);
  if (carrier.Buttons.onTouchDown(TOUCH1) && i < arrayLen) {
    i = i + 1;
    tempSet = baseTemperature[i];
    carrier.display.fillRect(45, 140, 100, 60, 0x00);
    carrier.display.setCursor(50, 150);
    carrier.display.print(tempSet);

    if (i == arrayLen) {
      i = 0;
      tempSet = baseTemperature[i];
      carrier.display.fillRect(45, 140, 100, 60, 0x00);
      carrier.display.setCursor(50, 150);
      carrier.display.print(tempSet);
    }
    delay(1000);
  }

  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    button_0 = false;
  }
}




//Scan current values of temperature and humidity
void scanning() {
  //Reading temperature
  carrier.display.fillScreen(0x0000);
  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(25, 60);
  carrier.display.print("Temperatura");
  currentTemp = carrier.Env.readTemperature();                             //storing temperature value in a variable
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9);  //0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  carrier.display.setCursor(60, 180);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.print(currentTemp);  // display the temperature on the screen
  carrier.display.setTextColor(0xFFFF);
  carrier.display.print(" C");
  delay(5000);

  // Reading humidity
  carrier.display.fillScreen(0x0000);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.print("Humedad");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14);  //0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  currentHum = carrier.Env.readHumidity();                              //storing humidity value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.print(currentHum);  // display the humidity on the screen
  carrier.display.setTextColor(0xFFFF);
  carrier.display.print(" %");
  delay(5000);
}

void scanningManual() {
  //Reading temperature
  carrier.display.fillScreen(0xFFFF);
  carrier.display.setRotation(0);
  carrier.display.setTextWrap(true);
  carrier.display.setTextColor(0x0000);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(25, 60);
  carrier.display.print("Temperatura");
  currentTemp = carrier.Env.readTemperature();                             //storing temperature value in a variable
  carrier.display.drawBitmap(80, 80, temperature_logo, 100, 100, 0xDAC9);  //0xDA5B4A); //draw a thermometer on the MKR IoT carrier's display
  carrier.display.setCursor(60, 180);
  carrier.display.setTextColor(0x0000);
  carrier.display.print(currentTemp);  // display the temperature on the screen
  carrier.display.setTextColor(0x0000);
  carrier.display.print(" C");
  delay(5000);

  // Reading humidity
  carrier.display.fillScreen(0xFFFF);
  carrier.display.setCursor(54, 40);
  carrier.display.setTextSize(3);
  carrier.display.setTextColor(0x0000);
  carrier.display.print("Humedad");
  carrier.display.drawBitmap(70, 70, humidity_logo, 100, 100, 0x0D14);  //0x0CA1A6); //draw a humidity figure on the MKR IoT carrier's display
  currentHum = carrier.Env.readHumidity();                              //storing humidity value in a variable
  carrier.display.setCursor(60, 180);
  carrier.display.setTextColor(0x0000);
  carrier.display.print(currentHum);  // display the humidity on the screen
  carrier.display.setTextColor(0x0000);
  carrier.display.print(" %");
  delay(5000);
}

//Compare current temperature with base temperature
void sensorsPage() {
  scanning();
  registerSD();
  carrier.display.fillScreen(0x0000);

  if (currentTemp < minTemp) {
    carrier.Relay1.close();
    carrier.Relay2.open();
    carrier.leds.fill(colorBlue, 0, 5);
    carrier.leds.setBrightness(100);
    carrier.leds.show();
    

    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(25, 100);
    carrier.display.setTextSize(3);
    carrier.display.print("Temperatura");
    carrier.display.setCursor(50, 150);
    carrier.display.setTextSize(3);
    carrier.display.print("baja");
    carrier.Buttons.update();
    delay(1000);
    carrier.display.fillScreen(0x0000);
    carrier.leds.fill(noColor, 0, 5);
    carrier.leds.show();
  }
  if (minTemp <= currentTemp && currentTemp <= maxTemp) {
    carrier.Relay1.close();
    carrier.Relay2.close();
    carrier.leds.fill(colorGreen, 0, 5);
    carrier.leds.setBrightness(100);
    carrier.leds.show();

    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(25, 95);
    carrier.display.setTextSize(3);
    carrier.display.print("Temperatura");
    carrier.display.setCursor(50, 150);
    carrier.display.setTextSize(3);
    carrier.display.print("estable");
    carrier.Buttons.update();
    delay(1000);
    carrier.display.fillScreen(0x0000);
    carrier.leds.fill(noColor, 0, 5);
    carrier.leds.show();
  }
  if (currentTemp > maxTemp) {
    carrier.leds.fill(colorRed, 0, 5);
    carrier.leds.setBrightness(100);
    carrier.Relay1.open();
    carrier.Relay2.close();
    carrier.leds.show();

    carrier.display.fillScreen(0x0000);
    carrier.display.setCursor(25, 95);
    carrier.display.setTextSize(3);
    carrier.display.print("Temperatura");
    carrier.display.setCursor(50, 150);
    carrier.display.setTextSize(3);
    carrier.display.print("alta");
    carrier.Buttons.update();
    delay(1000);
    carrier.display.fillScreen(0x0000);
    carrier.leds.fill(noColor, 0, 5);
    carrier.leds.show();
  }
  carrier.Buttons.update();

  if (carrier.Buttons.getTouch(TOUCH0)) {
    button_0 = true;
    carrier.display.fillScreen(0x0000);
    carrier.display.setTextColor(0xFFFF);
    carrier.display.setTextSize(3);
    carrier.display.setCursor(37, 40);
    carrier.display.print("Selecciona");
    carrier.display.setCursor(24, 70);
    carrier.display.print("temperatura");
    carrier.display.setCursor(90, 100);
    carrier.display.setTextSize(3);
    carrier.display.print("base:");
    carrier.display.setTextSize(5);
    carrier.display.setCursor(50, 150);
    carrier.display.print(tempSet);

    carrier.display.fillRect(45, 140, 100, 60, 0x00);
    carrier.display.setCursor(50, 150);
    carrier.display.print(tempSet);

    while (button_0) {
      settingTempPage();
    }
  }
}

//Manual heat mode
void manualHeat() {
  carrier.Buttons.update();
  carrier.Relay1.close();
  carrier.Relay2.open();

  carrier.display.fillScreen(0xFFFF);
  carrier.display.setTextColor(0x0000);
  carrier.display.setCursor(70, 70);
  carrier.display.setTextSize(4);
  carrier.display.print("Foco");
  carrier.display.setCursor(95, 145);
  carrier.display.setTextSize(4);
  carrier.display.print("ON");
  delay(2000);
  scanningManual();
  registerSD();

  if (carrier.Buttons.getTouch(TOUCH3)) {
    manualHeatFlag = false;
    carrier.Relay2.close();
    carrier.Relay1.close();
    automaticScreen();
    delay(4000);
  }
}

//Manual cooling mode
void manualCool() {
  carrier.Buttons.update();
  carrier.Relay2.close();
  carrier.Relay1.open();

  carrier.display.fillScreen(0xFFFF);
  carrier.display.setTextColor(0x0000);
  carrier.display.setCursor(35, 70);
  carrier.display.setTextSize(3);
  carrier.display.print("Ventilador");
  carrier.display.setCursor(95, 120);
  carrier.display.setTextSize(4);
  carrier.display.print("ON");
  delay(2000);
  scanningManual();
  registerSD();  

  if (carrier.Buttons.getTouch(TOUCH4)) {
    manualCoolFlag = false;
    carrier.Relay2.close();
    carrier.Relay1.close();
    automaticScreen();
    delay(4000);
  }
}

//Manual Screens
void manualScreen() {
  carrier.display.fillScreen(0xFFFF);
  carrier.display.setTextColor(0x0000);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(80, 60);
  carrier.display.print("Modo");
  carrier.display.setTextSize(3);
  carrier.display.setCursor(70, 100);
  carrier.display.print("manual");
  carrier.display.setTextSize(3);
  carrier.display.setCursor(45, 150);
  carrier.display.print("activado");
}

void automaticScreen() {
  carrier.display.fillScreen(0x0000);
  carrier.display.setTextColor(0xFFFF);
  carrier.display.setTextSize(3);
  carrier.display.setCursor(80, 65);
  carrier.display.print("Modo");
  carrier.display.setTextSize(3);
  carrier.display.setCursor(25, 110);
  carrier.display.print("automatico");
  carrier.display.setTextSize(3);
  carrier.display.setCursor(60, 160);
  carrier.display.print("activado");
}

//Temp and hum register in SD car
void registerSD(){  
  
  if (millis()>currentTime+60000){
    currentTime=millis();
    
    
  // abre el archivo de datos en modo escritura al final del archivo
  dataFile = SD.open("archive.txt", FILE_WRITE);
  
  // si no se puede abrir el archivo, se muestra un mensaje en el monitor serial
  if (!dataFile) {
    Serial.println("Error al abrir el archivo de datos");
    return;
  }

  // escribe los datos en el archivo CSV
  dataFile.print(currentTemp);
  dataFile.print(",");
  dataFile.print(currentHum);
  dataFile.println();

  // cierra el archivo de datos
  dataFile.close();

  // muestra los datos en el monitor serial
  Serial.print("Temperatura: ");
  Serial.print(currentTemp);
  Serial.print(" C  Humedad: ");
  Serial.print(currentHum);
  Serial.println(" %");
  }

}
