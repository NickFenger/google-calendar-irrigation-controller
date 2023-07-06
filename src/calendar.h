#ifndef __CALENDAR_H__
#define __CALENDAR_H__

//  Foward declaration.
class Google_OAuth2;

//*****************************************************************************
//
//! @brief Google Calendar class.
//!
//! This class uses the Google Calendar API to read user calendar events only.
//! It requires an OAuth2.0 access token to perfom the HTTP requests.
//!
//! Source: https://developers.google.com/calendar/v3/reference/events/list
//
//*****************************************************************************
class Google_Calendar
{
    private:
        //  Typedef function pointer for the user webhook reponse handler.
        typedef void (*Event_Callback)(void);
        Event_Callback callback;
        
        //  Google calendar param.
        const String CALENDAR_ID;
        const int8_t TIME_ZONE;
        //how far ahead to serach for an event's start time (ms) (lower numbers increase API call frequency)
        const  int32_t SEARCH_TIME = (120 * 60 * 1000);
        
        //  Particle webhook event name.
        const String WEBHOOK_EVENT_NAME = "calendar_event";
        
        //  Calendar API event data.
        String event_location;

        String event_title;

        
        time_t event_start_datetime;
        time_t event_end_datetime;
        
        int32_t time_left;
        int32_t event_length;
        
        bool event_pending;
        char statusText[32];
        
        //  Http status code and error response returned from webhooks.
        String http_error;
        uint16_t http_status_code;

        //  Private member functions.
        void parser(const char *event, const char *data);
        time_t convert_datetime(String event_date_time);

    protected:
        //  Particle webhook event handlers.
        void response_handler(const char *event, const char *data);
        void error_handler(const char *event, const char *data);

    public:
        //  Class constructor.
        Google_Calendar(const String &calendar_id, const int8_t &time_zone);
        
        //  Public member functions.
        void subscribe(Event_Callback callback);
        void publish(const Google_OAuth2 &oauth2);
        bool is_event_pending(void);
        bool failed(void);
        void print_error(void);
        String get_event_title(void);
        String get_event_location(void);
        time_t get_event_start_datetime(void);
        time_t get_event_end_datetime(void);
        String get_status_text(void);

        int32_t get_time_remaining(void);
        

};

#endif  //  __CALENDAR_H__