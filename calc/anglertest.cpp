#include <iostream>
#include <cstdio>
#include <boost/asio.hpp>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include "angler.hpp"
#include "serial.hpp"
#include "joystick.hpp"

using namespace std;

void printAngles(Serial *port)
{
    short x = getShort(port->data, 0);
    short y = getShort(port->data, 2);
    short z = getShort(port->data, 4);
    printf("    Input: %d %d %d\n", x, y, z);
    cout << "    Change in pitch: " << jpitch(x, y, z) << endl;
    cout << "    Change in roll: " << jroll(x, y, z) << endl;
}

int main()
{
    check_asserts();
    short x, y, z;
    double a, b, c, d;
    char in;

    while (1)
    {
        cout << "1. Joystick" << endl;
        cout << "2. Blocking Serial Read" << endl;
        cout << "3. Timeout Serial Read Abort" << endl;
        cout << "4. Timeout Serial Read Finish" << endl;
        cout << "5. Joystick Class" << endl;
        cout << "0. Exit" << endl;
        cin >> in;

        if (in == '1')
        {
            cout << "Input gx, gy, and gz. Input all 0 to exit." << endl;
            while (1)
            {
                cin >> x >> y >> z;
                if (x == y && y == z && z == 0)
                    break;
                cout << "Change in pitch: " << jpitch(x, y, z) << endl;
                cout << "Change in roll: " << jroll(x, y, z) << endl;
            }
        }
        else if (in == '2')
        {
            string port_name;
            cout << "Serial port: ";
            cin >> port_name;

            Serial port(port_name, 6, "\x80\x80");
            try
            {
                if (port.get_status() == Serial::IDLE)
                {
                   int input;
                   cout << "Opened serial port. Type in number of packets to try and read.\n";

                   do
                   {
                       cout << ">>> ";
                       cin >> input;
                       for (int i = 0; i < input; ++i)
                       {
                           if (port.read(6))
                           {
                               printAngles(&port);
                           }
                       }
                   }
                   while (input > 0);
                   delete &port;
                }
            }
            catch (const boost::system::system_error &ex)
            {
                std::cout << ex.what() << "\n";
                std::cout << "Error: Could not open serial port.\n";
            }
        }
        else if (in == '3')
        {
            string port_name;
            cout << "Serial port: ";
            cin >> port_name;

            Serial port(port_name, 6, "\x80\x80");
            if (port.get_status() == Serial::IDLE)
            {
               int input;
               cout << "Opened serial port. Type in timeout duration in milliseconds, or -2 to quit.\n";

               port.mode = Serial::MODE_ABORT;

               do
               {
                   cout << ">>> ";
                   cin >> input;
                   if (port.read(6, input))
                   {
                       printAngles(&port);
                   }
                   else
                   {
                       cout << "    TIMEOUT" << endl;
                   }
               }
               while (input > -2);
               delete &port;
            }
        }
        else if (in == '4')
        {
            string port_name;
            cout << "Serial port: ";
            cin >> port_name;

            Serial port(port_name, 6, "\x80\x80");
            if (port.get_status() == Serial::IDLE)
            {
               int input;
               cout << "Opened serial port. Type in timeout duration in milliseconds, or -2 to quit.\n";

               port.mode = Serial::MODE_FINISH;

               do
               {
                   cout << ">>> ";
                   cin >> input;
                   if (port.get_status() != Serial::IDLE)
                   {
                       if (port.poll())
                            if (port.get_status() == Serial::IDLE)
                                printAngles(&port);
                   }
                   if (port.read(6, input))
                   {
                       printAngles(&port);
                   }
                   else
                   {
                       cout << "    Read ongoing..." << endl;
                   }
               }
               while (input > -2);
               delete &port;
            }
        }
        else if (in == '5')
        {
            std::string port_name = "/dev/tty.usbmodem0E215ED1";
            int delay, timeout;
            Joystick ctrl(port_name);
            if (ctrl.port->get_status() == Serial::IDLE)
            {
                cout << "Opened serial port. Type in [poll delay in ms] [timeout duration in ms]\n";
                cin >> delay >> timeout;

                ctrl.flush();
                while (true)
                {
                    if (ctrl.update(timeout))
                        cout << ">>> "; // Value changed
                    else
                        cout << "||| "; // Value didn't change

                    short x = getShort(ctrl.port->data, 0);
                    short y = getShort(ctrl.port->data, 2);
                    short z = getShort(ctrl.port->data, 4);
                    printf("Input: %d %d %d\n", x, y, z);
                    cout << "    Change in pitch: " << ctrl.pitch << endl;
                    cout << "    Change in roll: " << ctrl.roll << endl;
                    cout << "    Throttle: " << ctrl.throttle << endl;
                    boost::this_thread::sleep_for(boost::chrono::milliseconds(delay));
                }
            }
        }
        else
            break;
    }
    return 0;
}
