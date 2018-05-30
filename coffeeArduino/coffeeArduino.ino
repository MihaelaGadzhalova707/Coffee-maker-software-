#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "Mosquitto.h"
#include "Networking.h"

#define __APP_VERSION__  "1.0.1 build 2018-05-20"

//=========================================================================================
const int DEBUG_LEVEL         = 3;
const int send_requests_count = 10;
const int local_id            = 1;

const int CHECKER             = D8;
const int SWITCH_ON_OFF       = D5;
const int SWITCH_ONE_CUP      = D6;
const int SWITCH_TWO_CUPS     = D7;
const int INPUT_ALARM1        = D0;
const int INPUT_ALARM2        = D1;
const int SETTINGS_UD         = D2; // Settings Up/Down pulses
const int SETTINGS_CS_A1      = D3; // CS - actuator 1
const int SETTINGS_CS_A2      = D4; // CS - actuator 2

const int ACTUATOR_MAX_STEPS  = 64; // MCP4011 ofers 64 steps
const int ACTUATOR_STEP_WIDTH = 50; // 50 ms pulse width

const int pulse_width         = 1000; //1 second
unsigned long previousMillis  = 0;//will store last time CHECKER updated
const long interval           = 2000;
int ledState                  = LOW;
const String wifi_ssid        = "nsn_cisco";
const String wifi_password    = "passenable";
//const String wifi_ssid        = "Mi Phone";
//const String wifi_password    = "coffeemaker";
/*const String http_host        = "rety.dynu.net";*/
const int http_port           = 80;
/*
const String mqtt_host        = "10.96.252.90";
const int mqtt_port           = 1883;
const String mqtt_id          = "Coffee_Machine:" + String(local_id);
const String mqtt_user        = "rety";
const String mqtt_password    = "retypass";
const String mqtt_topic_in    = "machine";
const String mqtt_topic_out   = "server";
*/

const String mqtt_host        = "rety.dynu.net";
const int mqtt_port           = 1883;
const String mqtt_id          = "Coffee_Machine:" + String(local_id);
const String mqtt_user        = "rety";
const String mqtt_password    = "retypass";
const String mqtt_topic_in    = "machine";
const String mqtt_topic_out   = "server";

static int settings_pos_a1     = -1;
static int settings_pos_a2     = -1;

Networking  network;
Mosquitto mosquitto;

//=========================================================================================
void setup() {
  Serial.begin(9600);
  Serial.println("");
  Serial.println("Version: " + String(__APP_VERSION__));

  pinMode(CHECKER,OUTPUT);
  pinMode(SWITCH_ON_OFF, OUTPUT);
  pinMode(SWITCH_ONE_CUP, OUTPUT);
  pinMode(SWITCH_TWO_CUPS, OUTPUT);
  pinMode(INPUT_ALARM1, INPUT);
  pinMode(INPUT_ALARM2, INPUT);
  pinMode(SETTINGS_UD, OUTPUT);
  pinMode(SETTINGS_CS_A1, OUTPUT);
  pinMode(SETTINGS_CS_A2, OUTPUT);

  digitalWrite(CHECKER, LOW);
  digitalWrite(SWITCH_ON_OFF, LOW);
  digitalWrite(SWITCH_ONE_CUP, LOW);
  digitalWrite(SWITCH_TWO_CUPS, LOW);
  digitalWrite(SETTINGS_UD, LOW);
  digitalWrite(SETTINGS_CS_A1, HIGH);
  digitalWrite(SETTINGS_CS_A2, HIGH);

  Serial.println();
  Serial.println("First stop AP");

  network.setDebugLevel(DEBUG_LEVEL);
  network.stopWifiAp();

  Serial.println("Init environment");
  if(DEBUG_LEVEL > 2) Serial.setDebugOutput(true);

  Serial.println("Init network");
  network.setLocalId(local_id);
  network.setWifiSsid(wifi_ssid);
  network.setWifiPassword(wifi_password);
 /* network.setHost(http_host);*/
  network.setPort(http_port);
  network.connectWifi();


  Serial.println("Connecting mosquitto broker");

  mosquitto.setMqttReconnectCount(send_requests_count);
  mosquitto.setDebugLevel(DEBUG_LEVEL);
  mosquitto.setCallback(mqttCallback);

  mosquitto.setup(mqtt_host, mqtt_id, mqtt_user, mqtt_password, mqtt_port, mqtt_topic_in, mqtt_topic_out);

}

//=========================================================================================
void loop() {
  unsigned long currentMillis = millis();
  delay(200);
   
   if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
     digitalWrite(CHECKER, ledState);
  }
  
 if(! mosquitto.read()) Serial.println("Failed to read !!!");
  
}


//=========================================================================================
void mqttCallback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String msg;
  for (int i = 0; i < length; i++)
  {
    Serial.print((char) payload[i]);
    msg += (char) payload[i];
  }

  Serial.println();

  if(msg.equals("on_off")) doOnOff();
  else if(msg.equals("make_one_cup")) doStartOneCup();
  else if(msg.equals("make_two_cups")) doStartTwoCups();
  else if(msg.equals("get_version"))
  {
    mosquitto.publish("version:" + String(__APP_VERSION__));
    return;
  }else if(msg.startsWith("set:")){
    String tmp = msg.substring(4);

    if(tmp.indexOf(":") <= 0)
    {
      mosquitto.publish("error");
      return;
    }

    int actuator = (tmp.substring(0, tmp.indexOf(":"))).toInt();
    int pos = (tmp.substring(tmp.indexOf(":") + 1)).toInt();
    int rety = 3;
    Serial.println(msg);
    Serial.println(tmp);
    Serial.println(actuator);
    Serial.println(pos);
    
    if(! setActuator(actuator, pos))
    {
    mosquitto.publish("error");
    return;
    }
  }else{
    mosquitto.publish("error");
    return;
  }
  
  mosquitto.publish("success");

}

//=========================================================================================
void mqttSend(String &contents)
{
  int count = 0;

  while(contents.length())
  {
    if((count++) > send_requests_count) break;
    if( mosquitto.publish(contents)) break;
  }
}

//=========================================================================================
bool setActuator(int actuator, int pos)
{
  int settings_cs;
  int settings_pos = -1;

  switch(actuator)
  {
    case 1:
      settings_cs = SETTINGS_CS_A1;
      settings_pos = settings_pos_a1;
      break;
    case 2:
      settings_cs = SETTINGS_CS_A2;
      settings_pos = settings_pos_a2;
      break;
    default: return false;
  }

// Deactivate all actuators:
  digitalWrite(SETTINGS_CS_A1, HIGH);
  digitalWrite(SETTINGS_CS_A2, HIGH);

// Uninitialized! Move wiper position to zero:
  if(settings_pos == -1)
  {
    digitalWrite(SETTINGS_UD, LOW); // Decrementing
    delay(200);

    digitalWrite(settings_cs, LOW); // Select actiator
    delay(200);

    for(int i = 0; i < ACTUATOR_MAX_STEPS; i++)
    {
      digitalWrite(settings_cs, HIGH);
      delay(ACTUATOR_STEP_WIDTH);
      digitalWrite(settings_cs, LOW);
      delay(ACTUATOR_STEP_WIDTH);
    }

    delay(200);
    digitalWrite(settings_cs, HIGH); // Deselect actiator
    delay(200);
  }

// Put wiper positon on place:
  delay(50);

// Decrementing:
  int steps = settings_pos - pos;

  if(steps > 0)
  {
    digitalWrite(SETTINGS_UD, LOW); // Decrementing
    delay(200);
  }else{
    digitalWrite(SETTINGS_UD, HIGH); // Incrementing
    delay(200);
  }

// Move position:
  digitalWrite(settings_cs, LOW); // Select actiator
  delay(200);
  digitalWrite(SETTINGS_UD, LOW); // Set ready
  delay(100);

  for(int i = 0; i < abs(steps); i++)
  {
    digitalWrite(SETTINGS_UD, HIGH);
    delay(ACTUATOR_STEP_WIDTH);
    digitalWrite(SETTINGS_UD, LOW);
    delay(ACTUATOR_STEP_WIDTH);
  }

  delay(100);
  digitalWrite(settings_cs, HIGH); // Deselect actiator
  delay(50);

// Set current actuator position:
  switch(actuator)
  {
    case 1: settings_pos_a1 = pos; break;
    case 2: settings_pos_a2 = pos; break;
    default: return false;
  }
  return true;
}

//=========================================================================================
void doOnOff() {digitalWrite(SWITCH_ON_OFF, HIGH); delay(pulse_width); digitalWrite(SWITCH_ON_OFF, LOW); }
void doStartOneCup() { digitalWrite(SWITCH_ONE_CUP, HIGH); delay(pulse_width); digitalWrite(SWITCH_ONE_CUP, LOW); }
void doStartTwoCups() { digitalWrite(SWITCH_TWO_CUPS, HIGH); delay(pulse_width); digitalWrite(SWITCH_TWO_CUPS, LOW); }

//=========================================================================================
