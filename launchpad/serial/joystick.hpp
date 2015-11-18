#ifndef JOYSTICK_HPP
#define JOYSTICK_HPP

class Joystick
{
    public:
        float pitch, roll;
        Joystick();
        Joystick(std::string port_name);
        bool update();

    private:
        boost::asio::serial_port *port;
        char *header;
        char *buffer;
};
#endif // JOYSTICK_HPP
