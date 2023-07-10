#include "Particle.h"
#include "utility.h"
#include "relaycontrol.h"

#define RELAY1PIN 4 //veggie
#define RELAY2PIN 5 //tomatoes
#define RELAY3PIN 6 //front yard
#define RELAY4PIN 7 //potted flowers



//Constructor
Relay_Control::Relay_Control(const int8_t &time_zone)
    : TIME_ZONE(time_zone)
{
    setup_relays();
    
    relay1_LastState = false;
    relay2_LastState = false;
    relay3_LastState = false;
    relay4_LastState = false;
    
    
    relay1 = false;
    relay2 = false;
    relay3 = false;
    relay4 = false;


    timerState = false;
    
    lastUpdateTime = 0;
}

void Relay_Control::turn_off_relays() {
    delay(1000);
    DEBUG_PRINT("Turning off relays");
    control_relay( 0, false);

}



void Relay_Control::control_relay(int relay_num, bool state){
    
    switch (relay_num)
    {
        case 0:
            relay1 = state;
            relay2 = state;
            relay3 = state;
            relay4 = state;
        case 1:
            relay1 = state;
            break;
        case 2:
            relay2 = state;
            break;
        case 3:
            relay3 = state;
            break;
        case 4:
            relay4 = state;
            break;
        default:
            break;
    }   
    relay_loop();

}

void Relay_Control::setup_relays()  {

 
     // It's important you do this here, inside the setup() function rather than outside it or in the loop function.
    pinMode(RELAY1PIN, OUTPUT);
    pinMode(RELAY2PIN, OUTPUT);
    pinMode(RELAY3PIN, OUTPUT);
    pinMode(RELAY4PIN, OUTPUT);
    
    digitalWrite(RELAY1PIN, HIGH);
    digitalWrite(RELAY2PIN, HIGH);
    digitalWrite(RELAY3PIN, HIGH); 
    digitalWrite(RELAY4PIN, HIGH);
   
    DEBUG_PRINT("Relay Setup Complete")

}
void Relay_Control::process_event(String event_title)
{



    if (event_title.indexOf("Veg") > -1) {
        relay1 = true;
    } else {
        relay1 = false;
    }
    
    if (event_title.indexOf("Tom") > -1) {
        relay2 = true;
    } else {
        relay2 = false;
    }
    
    if (event_title.indexOf("Fro") > -1) {
        relay3 = true;
    } else {
        relay3 = false;
    }

    if (event_title.indexOf("Pot") > -1) {
        relay4 = true;
    } else {
        relay4 = false;
    }
    
}

bool Relay_Control::relay1TimerActive() {
    bool isActive = false;
    if (
        relay1
    ){
        isActive = true;
    } else { 
        isActive = false;
    }
    return isActive;
}

bool Relay_Control::relay2TimerActive() {
    bool isActive = false;
    if (
        relay2 
    ){
        isActive = true;
    } else { 
        isActive = false;
    }
    return isActive;
}

bool Relay_Control::relay3TimerActive() {
    bool isActive = false;
    if (
        relay3
    ){
        isActive = true;
    } else { 
        isActive = false;
    }
    return isActive;
    
}

bool Relay_Control::relay4TimerActive() {
    bool isActive = false;
    if (
        relay4
    ){
        isActive = true;
    } else { 
        isActive = false;
    }
    return isActive;
    
}


void Relay_Control::relay_loop()
{


    timerState = relay1TimerActive();  //State Change method this block
    if(timerState != relay1_LastState)  {
        delay(1000);
        if(timerState)  {
            digitalWrite(RELAY1PIN, LOW);
            DEBUG_PRINT("relay1 = ON");
        } else  {
            digitalWrite(RELAY1PIN, HIGH);
            DEBUG_PRINT("relay1 = OFF");
        }
        relay1_LastState = timerState;
    }
    
    timerState = relay2TimerActive();  //State Change method this block
    if(timerState != relay2_LastState) {
        delay(1000);
        if(timerState) {
            digitalWrite(RELAY2PIN, LOW);
            DEBUG_PRINT("relay2 = ON");
        } else {
            digitalWrite(RELAY2PIN, HIGH);
            DEBUG_PRINT("relay2 = OFF");
        }
        relay2_LastState = timerState;
    }
    
    timerState = relay3TimerActive();  //State Change method this block
    if(timerState != relay3_LastState) {
        delay(1000);
        if(timerState) {
            digitalWrite(RELAY3PIN, LOW);
            DEBUG_PRINT("relay3 = ON");
        } else {
            digitalWrite(RELAY3PIN, HIGH);
            DEBUG_PRINT("relay3 = OFF");
        }
        relay3_LastState = timerState;
    }

    timerState = relay4TimerActive();  //State Change method this block
    if(timerState != relay4_LastState) {
        delay(1000);
        if(timerState) {
            digitalWrite(RELAY4PIN, LOW);
            DEBUG_PRINT("relay4 = ON");
        } else {
            digitalWrite(RELAY4PIN, HIGH);
            DEBUG_PRINT("relay4 = OFF");
        }
        relay4_LastState = timerState;
    } 

}











