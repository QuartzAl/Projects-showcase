#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DS3231.h>
#include <SPI.h>

#define HM_MODE 0
#define MS_MODE 1
#define STOPWATCH_MODE 2
#define TIMER_MODE 3
#define POMODORO_MODE 4

void sendMessage(int address, int message);
void updateDisplay(int value);
int numPlaces (int n);
void offDisplay();
int addresses[] = {13, 12, 11, 10};
const char* ssid     = "BIZNET";
const char* password = "manyar33";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DS3231 myRTC;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp; 


int lastValue = 0;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  
  pinMode(15, INPUT_PULLUP);
  pinMode(17, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
  // connect to ntp
  // get the current time
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (counter++ > 10){
      break;
    }
  }
  // Print local IP address and start web server
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

  // Initialize a NTPClient to get time
    timeClient.begin();
    // GMT +7 = 25200
    timeClient.setTimeOffset(25200);

  } else {
    myRTC.setClockMode(false);

  }
}

int mode = 0;
unsigned long previousMilis = 0;
unsigned long currentMillis;
unsigned long previousMilis2 = 0;
unsigned long stopWatchStartTime;
int previousPin15State = HIGH;
int previousPin17State = HIGH;
int previousPin4State = HIGH;
int previousPin16State = HIGH;
void loop(){
  // each address above is the address for an i2c slave device containing a single digit
  // addresses 10 and 11 are for the first two digits
  // addresses 12 and 13 are for the last two digits
  
  // set up NTP client with updates
  int hour, minute, second;
  if (WiFi.status() == WL_CONNECTED){
    while (!timeClient.update()){
      timeClient.forceUpdate();
    }
    hour = timeClient.getHours();
    minute = timeClient.getMinutes();
    second = timeClient.getSeconds();
    if (millis() - previousMilis2 > 1000){
      previousMilis2 = millis();
      myRTC.setEpoch(timeClient.getEpochTime());
    }
  }else {
    bool h12, hPM;
    hour = myRTC.getHour(h12, hPM);
    minute = myRTC.getMinute();
    second = myRTC.getSecond();

    if (h12 == true){
      if (hPM == true){
        hour += 12;
      }
    } else {
      hour = hour;
    }

  }

    if (digitalRead(15) == LOW && previousPin15State == HIGH){
    // switch between HM and MS mode
    mode = (mode + 1) % 2;
    previousPin15State = LOW;
  } 
  if (digitalRead(17) == LOW && previousPin17State == HIGH){
    // if in any other mode go in stopwatch mode
    // if in stopwatch mode display the stopwatch time with delay and return to the previous mode
    previousPin17State = LOW;
    if (mode != STOPWATCH_MODE){
      mode = STOPWATCH_MODE;
      stopWatchStartTime = millis();
      offDisplay();
    } else {
      mode = HM_MODE;
      unsigned long stopWatchEndTime = (millis() - stopWatchStartTime)/ 10;
      
      updateDisplay(stopWatchEndTime);
      delay(3000);

    }
  }

  if (digitalRead(4) == LOW && previousPin4State == HIGH){
    // if in any other mode go in timer mode
    // if in timer mode display the timer time with delay and return to the previous mode
    previousPin4State = LOW;
    if (mode != TIMER_MODE){
      mode = TIMER_MODE;
      stopWatchStartTime = millis();
      offDisplay();
    } else {
      mode = HM_MODE;
      
      int timerEndTime = (millis() - stopWatchStartTime)/ 1000;
      updateDisplay(timerEndTime);
      delay(3000);
    }
  }

  if (digitalRead(16) == LOW && previousPin16State == HIGH){
    // if in any other mode go in pomodoro mode
    // if in pomodoro mode display the pomodoro time with delay and return to the previous mode
    previousPin16State = LOW;
    if (mode != POMODORO_MODE){
      mode = POMODORO_MODE;
      stopWatchStartTime = millis();
      offDisplay();
    } else {
      mode = HM_MODE;
      // int pomodoroValue = (millis() - stopWatchStartTime)/ 1000;
      // updateDisplay(pomodoroValue);
      delay(3000);
    }
  }


  previousPin15State = digitalRead(15);
  previousPin17State = digitalRead(17);
  previousPin4State = digitalRead(4);
  previousPin16State = digitalRead(16);
  switch(mode){
    case HM_MODE:{
      updateDisplay(hour * 100 + minute);
      break;
    }
    case MS_MODE: {
      updateDisplay(minute * 100 + second);
      break;
    }
    case STOPWATCH_MODE: {
      currentMillis = millis();
      if (currentMillis - previousMilis > 1){
        previousMilis = currentMillis;
        updateDisplay((previousMilis - stopWatchStartTime)/ 10);
      }
      break;
    }
    case TIMER_MODE:{
      int currentMilis = millis();
      if (currentMilis - previousMilis > 1000){
        previousMilis = currentMilis;
        int duration = (previousMilis - stopWatchStartTime)/1000;
        int minutes = duration / 60;
        int seconds = duration % 60;
        updateDisplay(minutes * 100 + seconds);
      }
      break;
    }
    case POMODORO_MODE:{
      // updateDisplay(pomodoroValue);
      break;
    }
  }
  Serial.println("Mode: " + String(mode));
}

void updateDisplay(int value){
  // check last value if it's different from the current value
  if (value != lastValue){
    // check which digit to update
    int tempValue = value;
    int digits = numPlaces(value);
    int lastDigits = numPlaces(lastValue);
    int iterations = max(digits, lastDigits);
    for (int i = 0; i < iterations; i++){
      int digit = tempValue % 10;
      int lastDigit = lastValue % 10;
      if (digit != lastDigit)
        sendMessage(addresses[i], digit + 1);
      tempValue = tempValue / 10;
      lastValue = lastValue / 10;

    }
    lastValue = value;

  }
}

void offDisplay(){
  lastValue = 0;
  for (int i = 0; i < 4; i++){
    sendMessage(addresses[i], 0);
  }
}

int numPlaces (int n) {
    if (n < 0) n = (n == INT_MIN) ? INT_MAX : -n;
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    /*      2147483647 is 2^31-1 - add more ifs as needed
       and adjust this final return as well. */
    return 10;
}

void sendMessage(int address, int message){
  Wire.beginTransmission(address);
  Wire.write(message);
  int errorCode = Wire.endTransmission();
  Serial.print("Error code: ");
  Serial.println(errorCode);
}