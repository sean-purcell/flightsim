#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

#include "serial.hpp"

class Joystick
{
    public:
        static const char REQUEST;
        static const char * HEADER;
        static const int LEN;

        float pitch, roll;
        int num_pending;    // number of pending requests sent to Launchpad
        Serial *port;

        Joystick();
        Joystick(std::string port_name);
        bool update(int timeout=-1);
        void flush();
        ~Joystick();

    private:
        bool update_mode_abort(int timeout);
        bool update_mode_finish(int timeout);
        bool write_request();
        void recalc_angles();
};
#endif // JOYSTICK_HPP
