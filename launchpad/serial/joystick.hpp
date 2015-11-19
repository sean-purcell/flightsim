#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

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
};
#endif // JOYSTICK_HPP
