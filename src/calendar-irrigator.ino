

/*
https://www.hackster.io/rodruizronald/smart-calendar-powered-by-particle-and-google-98b71a
https://github.com/rodruizronald/Smart-Calendar
https://console.cloud.google.com/apis/api/calendar-json.googleapis.com/quotas?project=irrigation-schedule-319920
https://console.cloud.google.com/apis/api/calendar-json.googleapis.com/credentials?project=irrigation-schedule-319920

*/

#include "utility.h"

//*****************************************************************************
//Fse
//  The following are header files for the application.
//
//*****************************************************************************

#include "oauth2.h"
#include "calendar.h"
#include "relaycontrol.h"
#include "app.h"
#include "http_status.h"


#define DEBUG_PRINT(...) { Particle.publish( "DEBUG", String::format(__VA_ARGS__) ); }

STARTUP(WiFi.selectAntenna(ANT_EXTERNAL)); 

char timeRemaining[128];
char currentState[32];
uint32_t freemem;

unsigned long  polling_time;
unsigned long  polling_rate;


void setup()
{
    //Serial.begin();
    Particle.variable("Time Remaining", timeRemaining);
    Particle.variable("Current State", currentState);
    //using this particle variable to track the amount of free memory available to the system.
    Particle.variable("Memory", freemem);
 
    Particle.function("Read Calendar (1 read)", read_calendar);  
    Particle.function("Erase Token (1 erase)", force_erase_token); 


    Particle.function("Relay On (0 all)", relay_on);
    Particle.function("Relay Off (0 all)", relay_off);

    Particle.function("Relay 1 Min", relay_1_time);
    Particle.function("Relay 2 Min", relay_2_time); 
    Particle.function("Relay 3 Min", relay_3_time); 


   
    Time.zone(TIME_ZONE);
    Time.setFormat(TIME_FORMAT_ISO8601_FULL);
   
    //Time.beginDST();
    //time_t time_status = Time.now();
    //DEBUG_PRINT(Time.format(time_status,"%Y-%m-%d %H:%M:%S"));
    //DEBUG_PRINT(Time.format(time_status, TIME_FORMAT_ISO8601_FULL));
    DEBUG_PRINT("Erase Token");
    
    //OAuth2.erase_token();
    
    if (OAuth2.authenticated()) 
    //NJF is this right?
    {   
        //play_status_info(MP3_File::UPDATE_DEVICE
        //Serial.println("Oauth2 already authenicated");
        DEBUG_PRINT("Oauth2 already authenicated");
        change_app_stage_to(App_Stage::CALENDAR);
        //change_app_stage_to(App_Stage::OAUTH2);
    }
    else
    {
       change_app_stage_to(App_Stage::OAUTH2);
    }
    
    polling_time = millis();
 
}
 
    
int read_calendar(String cmd) {
    if (cmd.toInt() > 0) change_app_stage_to(App_Stage::CALENDAR);
    return 0;
}

int relay_on(String cmd) {
    Control.control_relay(cmd.toInt(), true);
    return 0;
}

int relay_off(String cmd) {
    Control.control_relay(cmd.toInt(), false);
    return 0;
}

int relay_1_time(String cmd) {
    manual_relay_mode(1, cmd.toInt());
    return 0;
}

int relay_2_time(String cmd) {
    manual_relay_mode(2, cmd.toInt());
    return 0;
}

int relay_3_time(String cmd) {
    manual_relay_mode(3, cmd.toInt());
    return 0;
}

int force_erase_token(String cmd) {
    if (cmd.toInt() > 0) {
        OAuth2.erase_token();
        change_app_stage_to(App_Stage::OAUTH2);
    }
    return 0;
}

void manual_relay_mode(int relay_num, int run_time){
    Control.turn_off_relays();
    sprintf(currentState, "Manual: Relay %d", relay_num);
    Control.control_relay(relay_num, true);
    polling_rate = (run_time *  60 * 1000);
    polling_time = millis();
    change_app_stage_to(App_Stage::MANUAL);
}

void update_time_remaining(unsigned long now) {
                                            
    unsigned long msecs_till_action = polling_rate - (now - polling_time) ;
    int hours = msecs_till_action / 1000 / 3600;
    int minutes = (msecs_till_action / 1000 - (hours * 3600) )/ 60;
    int seconds = (msecs_till_action / 1000 - (hours * 3600) - (minutes * 60));
    sprintf(timeRemaining, "%d:%02d:%02d", hours , minutes, seconds);
}


void loop()
{
    //updating the particle variable.
    freemem = System.freeMemory();
   
    unsigned long now = millis();
    update_time_remaining(now);
    
    if ((polling_rate > (60 * 60 * 1000)) && (app_stage == App_Stage::ACTIVE) ) {
        DEBUG_PRINT("Max Event Time Exceeded");
        change_app_stage_to(App_Stage::EVENT_TOO_LONG);
    }
    
    unsigned long elapsed = now - polling_time; //will always be positive
    
    if (elapsed <= polling_rate) {

        switch (app_stage)
        {
              
            
            case App_Stage::EVENT_TOO_LONG:
                Control.relay_loop();
                break;
                
            case App_Stage::WAITING:
                Control.relay_loop();
                break;
                
            case App_Stage::PENDING:
                Control.relay_loop();
                break;
                
            case App_Stage::ACTIVE:
                Control.relay_loop();
                break;
                
          case App_Stage::MANUAL:
                Control.relay_loop();
                break;
                
           case App_Stage::FAILED:
                Control.turn_off_relays();
                delay(1000);
                DEBUG_PRINT("Press Reset Button");
                delay(15 * 60 * 1000);
                print_app_error();
                Control.turn_off_relays();
                Control.relay_loop();
                break;
            
            default:
                break;

        }
        delay(1000);

    } else {
        switch (app_stage)
            {
            //  If the user code expires and the user has not responded 
            //  to the access request, then the OAuth2.0 authorization 
            //  will fail.
             case App_Stage::OAUTH2:
                oauth2_loop();
                break;
    
            case App_Stage::CALENDAR:
                calendar_loop();
                break;
    
            case App_Stage::WAITING:
                //no events found last time, go check again
                change_app_stage_to(App_Stage::CALENDAR); 
                break;
                
             case App_Stage::EVENT_TOO_LONG:
                change_app_stage_to(App_Stage::CALENDAR); 
                break;
               
            case App_Stage::PENDING:
                //done waiting - stat the pending event alredy retrieved from 
                change_app_stage_to(App_Stage::ACTIVE); 
                break;
                
            case App_Stage::ACTIVE:
                //Active state complete - go look for more events
                change_app_stage_to(App_Stage::CALENDAR); 
                break;
                
            case App_Stage::MANUAL:
                Control.turn_off_relays();
                //Manual state complete - go look for more events
                change_app_stage_to(App_Stage::CALENDAR); 
    
                break;
            
            default:
                break;
            

        }   
        
        polling_time = millis();
    }
    
}


//*****************************************************************************
//  @section Google OAuth2.0 protocol.
//*****************************************************************************
//*****************************************************************************
//
//! @brief OAuth2.0 main function.
//!
//! @return None. 
//
//*****************************************************************************
void oauth2_loop(void)
{
    //  Execute the OAuth2.0 authorization algorithm.
    //  OAuth2.loop() should run freely without delay
    //  until the application has been authorized.
    OAuth2.loop();
    
    if (OAuth2.authorized())
    {
        //  In case that the access token had to be refreshed and the Calendar 
        //  stage were interrupted, then go back and finish the user request. 
        //  Otherwise, switch to Assitant mode and wait for a new request.

        if (last_app_stage == App_Stage::CALENDAR)
        {
            change_app_stage_to(App_Stage::CALENDAR);
        }
        else
        {
            DEBUG_PRINT("OAuth2 WAITING");
            change_app_stage_to(App_Stage::WAITING);
            //  Only during initialization, inform the user device is ready.    
        }
    }
    else if (OAuth2.failed())
    {
        DEBUG_PRINT("OAuth2.failed()");
        change_app_stage_to(App_Stage::FAILED);
    }
}

//*****************************************************************************
//  @section Google Calendar API.
//*****************************************************************************
//*****************************************************************************
//
//! @brief Calendar main function.
//!
//! @return None. 
//
//*****************************************************************************
void calendar_loop(void)
{
    //  Publish the event and wait until the
    //  application-level response handler is called.
    if (event_state == Event_State::PUBLISHING)
    {
        //  If the access token has not expired yet, use the API.
        //  Otherwise, change stage to OAuth2.0 to refresh token.
        if (OAuth2.is_token_valid())
        {
            //  OAuth2 is passed to get the access token.
            Calendar.publish(OAuth2);
        }
        else
        {
            change_app_stage_to(App_Stage::OAUTH2);
        }
    }
    print_event_state();
}

//*****************************************************************************
//
//! @brief Calendar application-level response handler.
//
//*****************************************************************************
void calendar_handler(void)
{

    if (!Calendar.failed())
    {
        //  Check if the webhook returned any user event/activity/meeting... 

        if (Calendar.is_event_pending())
        {
            
            if ((Calendar.get_time_remaining() * 1000) < 0) {
                //this will turn on any relays
                change_app_stage_to(App_Stage::ACTIVE);
                sprintf(currentState, "Actve: " + Calendar.get_event_title());
            } else {
                //Control.process_event( Calendar.get_event_title() );
                change_app_stage_to(App_Stage::PENDING);
                sprintf(currentState, "Pending: " + Calendar.get_event_title());
            }

        } else {

            delay(1000);
            DEBUG_PRINT("No pending events!");
            //disable all relays
            change_app_stage_to(App_Stage::WAITING);
            sprintf(currentState, "Waiting");

        }
    }
    else
    {
        change_app_stage_to(App_Stage::FAILED);
    }
}




//*****************************************************************************
//  @section General application functions.
//*****************************************************************************
//*****************************************************************************
//
//! @brief Changes the current stage of the application and subscribes the 
//!        webhook reponse handlers, if necessary.
//!
//!	@param[in] new_stage Stage at which the application is set.
//!
//! @return None. 
//
//*****************************************************************************
void change_app_stage_to(App_Stage new_stage)
{
    //  Save the previous application stage in case of a failure.
    unsigned long now = millis();


    switch (new_stage)
    {
        case App_Stage::OAUTH2:
            DEBUG_PRINT("Stage changed to: OAUTH2");
            polling_rate = 1000;
            polling_time = millis();
            break;

        case App_Stage::CALENDAR:
            DEBUG_PRINT("Stage: CALENDAR");
            polling_rate = 1000;
            polling_time = millis();
            Calendar.print_error();
            break;
            
        case App_Stage::WAITING:
            DEBUG_PRINT("Stage: WAITING");
            Control.turn_off_relays();
            polling_rate = Calendar.get_time_remaining() * 1000 ;
            if (polling_rate < 1000)  polling_rate = 1000; //never wait less than 1 second should an error occur
            polling_time = millis();
            update_time_remaining(now);
            DEBUG_PRINT(timeRemaining);
            break;
            
        case App_Stage::EVENT_TOO_LONG:
            DEBUG_PRINT("Stage: EVENT_TOO_LONG");
            Control.turn_off_relays();
            polling_rate = (1 * 60 * 60 * 1000) ;
            polling_time = millis();
            update_time_remaining(now);
            DEBUG_PRINT(timeRemaining);
            break;
            
        case App_Stage::PENDING:
            DEBUG_PRINT("Stage: PENDING");
            Control.turn_off_relays();
            polling_rate = Calendar.get_time_remaining() * 1000 ;
            if (polling_rate < 1000)  polling_rate = 1000; //never wait less than 1 second should an error occur
            polling_time = millis();
            update_time_remaining(now);
            DEBUG_PRINT(timeRemaining);
            break;
            
        case App_Stage::ACTIVE:
            DEBUG_PRINT("Stage: ACTIVE");
            Control.process_event(Calendar.get_event_title());
            sprintf(currentState, "Actve: " + Calendar.get_event_title());
            polling_rate = abs(Calendar.get_time_remaining() * 1000);
            if (polling_rate < 1000)  polling_rate = 1000; //never wait less than 1 second should an error occur
            polling_time = millis();
            update_time_remaining(now);
            DEBUG_PRINT(timeRemaining);
            break;
            
        case App_Stage::MANUAL:
            
            DEBUG_PRINT("Stage: MANUAL");
            update_time_remaining(now);
            DEBUG_PRINT(timeRemaining);
            break;


        default:
            break;
    }
    last_app_stage = app_stage;
    app_stage = new_stage;
    //  If the application stage changes, it is assumed 
    //  that the previous event has been completed.
    event_state = Event_State::COMPLETED;     
    //  Unsubsribe all passed events to make space for new ones.
    Particle.unsubscribe();
    //  Event handlers have to be subscribed multiple times since
    //  they are removed between intermediate stages. This is
    //  because the Argon OS does not support more than four
    //  handlers subscribed at the same time.
    //  IMPORTANT: OS handlers are the ones subscribed with the
    //  Particle.subscribe() function which are called within 
    //  the Google classes.

    if (new_stage == App_Stage::CALENDAR)
    {
        Calendar.subscribe(calendar_handler);
    }
    else if (new_stage == App_Stage::FAILED)
    {
        //  In case of failure, inform the user.
        Control.turn_off_relays();
        //Serial.println("App Stage Failure");
        delay(1000);
        DEBUG_PRINT("App Stage Failure");
    }
}

//*****************************************************************************
//
//! @brief Prints general information about the current state and updates it, 
//!        if necessary.
//!
//! @return None. 
//
//*****************************************************************************
void print_event_state(void)
{
    if (event_state == Event_State::PUBLISHING)
    {
        switch (app_stage)
        {

        case App_Stage::CALENDAR:
            //Serial.println("Calendar event published!");
            DEBUG_PRINT("Calendar event published!");
            break;

        default:
            break;
        }
        event_state = Event_State::WAIT_FOR_RESPONSE;
    }
    else if (event_state == Event_State::WAIT_FOR_RESPONSE)
    {
        //Serial.println("waiting: calendar api response...");
        //DEBUG_PRINT("waiting for a response...(main)");
        delay(1000);
    }
    //  If event completed, switch to publishing 
    //  to enable a new event to be published. 
    else if (event_state == Event_State::COMPLETED)
    {
        event_state = Event_State::PUBLISHING;
    }
}

//*****************************************************************************
//
//! @brief Prints the application error caused by the last stage. Most of this
//!        errors occur when the Google APIs return an HTTP status codes 
//!        different than 200.
//!
//! @return None. 
//
//*****************************************************************************
void print_app_error(void)
{
    switch (last_app_stage)
    {
        case App_Stage::OAUTH2:
            OAuth2.print_error();
            break;

        case App_Stage::CALENDAR:
            Calendar.print_error();
            break;
            
        default:
            break;
    }
    delay(1000);
}