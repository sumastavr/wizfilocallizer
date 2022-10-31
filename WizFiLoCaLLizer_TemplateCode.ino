#define BLYNK_TEMPLATE_ID           "Copy your template ID Here"
#define BLYNK_DEVICE_NAME           "Copy your Device name ID Here"
#define BLYNK_AUTH_TOKEN            "Copy your Auth Token Here"

/* Wi-Fi info */
char ssid[] = "Copy your WiFi SSID Here";       // your network SSID (name)
char pass[] = "Copy your WiFi Password Here";   // your network password

#include "WizFi360.h" // Wiz360 driver Library
#include <ESP8266_Lib.h> 
#include <BlynkSimpleShieldEsp8266.h> // Blynk Serial AT interface: Simple shield which is compatible with Wiz360

#include <MD_Parola.h> // Library for MAX7219 matrix display
#include <SPI.h>

// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
// #define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // must select proper hardware type! see library for options

#define MAX_DEVICES 4   // number of connected device, 4 in my case
#define CLK_PIN   2     // connect to GP2 of RP2040 for clock signal
#define DATA_PIN  3     // connecto to GP3 of RP2040 for data signal
#define CS_PIN    6     // connect to GP6 of RP2040 for Chip Select signal

// HARDWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // for better flexibility SW SPI is used, alternatively HW serial is an option uncomment above line

uint8_t scrollSpeed = 25;                   // default frame delay value, the higher the slower it moves
textEffect_t scrollEffect = PA_SCROLL_LEFT; // Moving type
textPosition_t scrollAlign = PA_LEFT;       // Alignment scrolling
uint16_t scrollPause = 0;                   // in milliseconds

// Global message buffers shared by Serial and Scrolling functions
#define	BUF_SIZE	150
char curMessage[BUF_SIZE] = { "" }; // variable current message which will be copied to if new message is received
char newMessage[BUF_SIZE] = { "" }; // current default new message, will be shown during power on
bool newMessageAvailable = true; // trigger new message flag such that above new message will be copied during boot-up

// String buffers that holds the text to show on the scrolling display
// It gets updated as soon as new text is received from the blynk server
// Consists of 2 const string in the beginning and the end, while the middle part is variable text
const String preText="       Sumasta is currently ";
String place="in the office and should be nearby ";
const String callInfo="          Press RED button below to call                     ";

const String noLocalizationText= "Localization disabled";

// Total Text that is displayed on the scrolling matrix
String totalText=preText+place+callInfo;

// setup according to the device you use
#define WIZFI360_EVB_PICO

int status = WL_IDLE_STATUS;  // the Wifi radio's status

char auth[] = BLYNK_AUTH_TOKEN;

#define UsbSerial Serial    // USB Serial for debbugging through the PC via native USB interface of RP2040
#define SimSerial Serial1   // UART Serial connected to SIM800L
#define WizSerial Serial2   // UART Serial connected to the WIZ Module, internally connected inside the WIZ360-pico=module

// All Serial interface with same speed, however can be adjusted if needed
#define SERIAL_BAUDRATE   115200
#define ESP8266_BAUD      115200
#define SIM_BAUD          115200

// Assign ESP8266 to wifi and WizSerial interface
ESP8266 wifi(&WizSerial);

// Define and initiate widget terminals for interfacing with the blynk smartphone UI and Server
WidgetTerminal terminal(V3);
WidgetTerminal terminal2(V4);
WidgetTerminal terminalAT(V5);

#define BUTTON_PIN 10

String temp;

boolean isCalling=false;

long timerError=millis();
long timerCallEnded=millis();
long timerStartCalling=millis();

boolean splashBanner=false;

void setup() {

  UsbSerial.begin(SERIAL_BAUDRATE);
  WizSerial.begin(ESP8266_BAUD);
  SimSerial.begin(SIM_BAUD);

  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  totalText.toCharArray(newMessage,BUF_SIZE);

  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);

  initSIM();

  pinMode(BUTTON_PIN,INPUT_PULLUP);

}

void loop() {

  Blynk.run();

  if (P.displayAnimate()){
    if (newMessageAvailable){
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }

  if (newMessageAvailable){
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
  }

  if(digitalRead(BUTTON_PIN)==LOW && !isCalling){
    delay(100);
    while(digitalRead(BUTTON_PIN)==LOW);
    
    timerStartCalling=millis();
    UsbSerial.println("Initiate Call");
    SimSerial.print("ATD+31621321503;\r\n");
    
    if(SimSerial.find("OK")){
      isCalling=true;
      temp="       Calling Pamungkas...   Press RED button again to hang-up       ";
      temp.toCharArray(newMessage,BUF_SIZE);
      newMessageAvailable = true;
      P.displayReset();
      UsbSerial.println("Calling Success");
      SimSerial.setTimeout(10);
    }else{     
      temp="      ERROR cannot initiate the call at the moment         ";
      temp.toCharArray(newMessage,BUF_SIZE);
      newMessageAvailable = true;
      P.displayReset();
      splashBanner=true;
      timerError=millis();
      UsbSerial.println("ERROR, can't make a call");
    }   
  }

  if(isCalling && digitalRead(BUTTON_PIN)==LOW && millis()-timerStartCalling>7000){
    delay(100);
    while(digitalRead(BUTTON_PIN)==LOW);

    UsbSerial.println("Hanging-up");
    isCalling=false;
    SimSerial.setTimeout(1000);
    SimSerial.print("ATH\r\n");
    
    if(SimSerial.find("OK")){
      temp="      Call Ended... Thank you       ";
      temp.toCharArray(newMessage,75);
      newMessageAvailable = true;
      P.displayReset();
      splashBanner=true;
    }else{
      temp="      Error, cannot hang-up        ";
      temp.toCharArray(newMessage,75);
      newMessageAvailable = true;
      P.displayReset();
      splashBanner=true;
    }
    timerError=millis();
  }

  if(isCalling && SimSerial.find("NO CARRIER")){
    temp="       Call Ended... Thank you       ";
    temp.toCharArray(newMessage,75);
    newMessageAvailable = true;
    P.displayReset();
    splashBanner=true;
    timerError=millis();
    isCalling=false;
  }

  if(splashBanner && millis()-timerError>5000){
    newMessageAvailable=true;
    P.displayReset();
    totalText.toCharArray(newMessage,BUF_SIZE);
    splashBanner=false;
    timerError=millis();
  }

  readSerial();

  while (SimSerial.available()){
    byte in=SimSerial.read();
    UsbSerial.write(in);
    terminalAT.write(in);
  }

}

void multiSerialPass(){

  if (SimSerial.available()) {
    int inByte = SimSerial.read();
    UsbSerial.write(inByte);
  }

  if (UsbSerial.available()) {
    int inByte = UsbSerial.read();
    //UsbSerial.write(inByte);
    SimSerial.write(inByte);
  }

}

void initSIM(){
  SimSerial.print("AT\r\n");
  if (!SimSerial.find("OK")){
    UsbSerial.println("NO SIM800 Detected!");   
  }else{

    SimSerial.print("AT+CPIN=5124\r\n");
    delay(2000);

    // Swap the audio channels
    SimSerial.print("AT+CHFA=0\r\n");
    delay(50);

    //Set ringer sound level
    SimSerial.print("AT+CRSL=100\r\n");
    delay(50);

    //Set AUDIO sound level
    SimSerial.print("AT+CLVL=100\r\n");
    delay(50);

    // Calling line identification presentation
    SimSerial.print("AT+CLIP=1\r\n");
    delay(50);

    // Calling line identification presentation
    SimSerial.print("AT+CALS=10\r\n");
    delay(50);
  }  
}

void readSerial(void){

  static char *cp = newMessage;

  while (UsbSerial.available()){
    *cp = (char)UsbSerial.read();
    if ((*cp == '\n') || (cp - newMessage >= BUF_SIZE-2)) // end of message character or full buffer
    {
      *cp = '\0'; // end the string
      // restart the index for next filling spree and flag we have a message waiting
      cp = newMessage;
      newMessageAvailable = true;
    }
    else  // move char pointer to next position
      cp++;
  }

}

// Expect localization place from the V3 pins
// Mostly applicable for WiFi and GPS trigger from IFTTT
// Check IFTTT recipe for details
BLYNK_WRITE(V3){

  String inFromBlynk=param.asStr();

  UsbSerial.print("Received from Blynk: ");
  UsbSerial.println(inFromBlynk);

  place=inFromBlynk;

  totalText=preText+place+callInfo;
  totalText.toCharArray(newMessage,BUF_SIZE);
  newMessageAvailable=true;

  Blynk.virtualWrite(V6, place);

  // Ensure everything is sent
  terminal.flush();
}

// Expect localization details from Bluetooth connection trigger via IFTTT Apps
// Only BLE connection started with "." in the first char will be displayed
// otherwise ignore

BLYNK_WRITE(V4){

  String inFromBlynk=param.asStr();

  if(inFromBlynk.charAt(0)=='.' || inFromBlynk.charAt(1)=='.'){
    UsbSerial.print("VALID BLE CONNECTION: ");
    UsbSerial.println(inFromBlynk);   
  }else{
    UsbSerial.println("Not a Localization BLE Connection");
    UsbSerial.println(inFromBlynk);
  }

  terminal2.flush();
}

BLYNK_WRITE(V5){

  String inFromBlynk=param.asStr();

  UsbSerial.print("Received from Blynk: ");
  UsbSerial.println(inFromBlynk);
  SimSerial.print(inFromBlynk);
  SimSerial.print("\r\n");
  terminalAT.flush();

}

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0){
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  if (value==1){
    UsbSerial.println("Show Localization");
    totalText=preText+place+callInfo;
    totalText.toCharArray(newMessage,BUF_SIZE);
    newMessageAvailable=true;
    P.displayReset();
    Blynk.virtualWrite(V6, place);
  }else{  
    noLocalizationText.toCharArray(newMessage,BUF_SIZE);
    newMessageAvailable=true;
    P.displayReset();
    UsbSerial.println("Disable localization");
    Blynk.virtualWrite(V6, noLocalizationText);
  }

}

BLYNK_WRITE(V7){
  // Set incoming value from pin V7 to a variable
  int value = param.asInt();
  scrollSpeed = value; 
  P.displayReset();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()
{
  // Change Web Link Button message to "Congratulations!"
  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
  Blynk.virtualWrite(V0, true);
  Blynk.virtualWrite(V7, scrollSpeed);
  Blynk.virtualWrite(V6, place);
}