#ifndef __RELAYCONTROL_H__
#define __RELAYCONTROL_H__

//  Foward declaration.

//class Google_Calendar;

class Relay_Control
{
    private:
    
        const int8_t TIME_ZONE;
    
        bool relay1_LastState;
        bool relay2_LastState;
        bool relay3_LastState;
        bool relay4_LastState;
        
        bool relay1;
        bool relay2;
        bool relay3;
        bool relay4;
            
        bool timerState;
        
        uint32_t lastUpdateTime;
    
    
        void setup_relays(); 
        bool relay1TimerActive(); 
        bool relay2TimerActive(); 
        bool relay3TimerActive(); 
        bool relay4TimerActive(); 
 
        
    protected:
        //  Particle webhook event handlers.


    public:
        //  Class constructor.
        Relay_Control(const int8_t &time_zone);
        
        void process_event(String event_title);
        void turn_off_relays();
        void relay_loop();
        
        void control_relay(int relay_num, bool state);

 
};

#endif  //  __RELAYCONTROL_H__