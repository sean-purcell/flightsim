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

        Joystick();
        Joystick(std::string port_name);
        bool update(int timeout=-1);
        ~Joystick();

    private:
        Serial *port;
};
#endif // JOYSTICK_HPP
