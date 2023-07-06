#ifndef __APP_H__
#define __APP_H__
#include "apikeys.h"

//*****************************************************************************
//
//	The following are enumeration classes for the application stages and 
//  event states.
//
//*****************************************************************************

enum class App_Stage : uint8_t
{
    OAUTH2,
    CALENDAR,
    FAILED,
    WAITING,
    EVENT_TOO_LONG,
    PENDING,
    ACTIVE,
    MANUAL,
    DISCONNECTED
};
App_Stage app_stage;
App_Stage last_app_stage;

enum class Event_State : uint8_t
{
    PUBLISHING,
    WAIT_FOR_RESPONSE,
    COMPLETED
};
Event_State event_state;

//*****************************************************************************
//
//	The following are global definitios to configure your application.
//
//*****************************************************************************


//  Set your time zone here. You MUST consider Daylight saving time (DST)

const int8_t TIME_ZONE = -7;

//Don't forget to update the webhook device ID's if you change devices


//*****************************************************************************
//
//	The following are global objects for the DFPlayer and Google classes.
//
//*****************************************************************************


std::unique_ptr<Google_OAuth2> OAuth2;
Google_Calendar Calendar(CALENDAR_ID, TIME_ZONE);
Relay_Control Control(TIME_ZONE);



//*****************************************************************************
//
//  Prototypes for the application functions.
//
//*****************************************************************************

void oauth2_loop(void);
void calendar_loop(void);
void calendar_handler(void);

void print_app_error(void);
void print_event_state(void);
void change_app_stage_to(App_Stage new_stage);

#endif // __APP_H__