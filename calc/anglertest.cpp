#include <iostream>
#include <cstdio>
#include <SFML/System.hpp>
#include <boost/asio.hpp>
#include "angler.hpp"
#include "rotation.hpp"
#include "quaternion.hpp"
#include "serial.hpp"

using namespace std;

// Dummy values for windowinfo.hpp

int fov=90;
float ratio=std::tan(fov/2.0);
int screenwidth=1000;
int screenheight=850;
sf::Vector2i size(screenwidth, screenheight);

int main()
{
    check_asserts();
    short x, y, z;
    double a, b, c, d;
    char in;

    while (1)
    {
        cout << "1. Joystick" << endl;
        cout << "2. Euler to quaternion" << endl;
        cout << "3. Quaternion to Euler" << endl;
        cout << "4. Blocking Serial Read" << endl;
        cout << "5. Timeout Serial Read Abort" << endl;
        cout << "6. Timeout Serial Read Finish" << endl;
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
            cout << "Input roll, pitch, yaw. Input yaw = 360 to exit." << endl;
            while (1)
            {
                cin >> a >> b >> c;
                if (c == 360)
                    break;
                a = radians(a);
                b = radians(b);
                c = radians(c);
                Quaternion q = Euler(a, b, c).toQuaternion();
                cout << "Quaternion: " << toString(q) << endl;
            }

        }
        else if (in == '3')
        {
            cout << "Input quaternion comonents. Input all 0 to exit." << endl;
            while (1)
            {
                cin >> a >> b >> c >> d;
                if (a == b && b == c && c == d && d == 0)
                    break;
                Euler e = Euler::fromRotation(Quaternion(a, b, c, d));
                cout << "Tait-Bryan angles: " << e.toString() << endl;
            }
        }
        else if (in == '4')
        {
            string port_name;
            cout << "Serial port: ";
            cin >> port_name;

            Serial port(port_name, 6, "\x80\x80");
            try
            {
                if (port.status == Serial::IDLE)
                {
                   int input;
                   cout << "Opened serial port. Type in number of bytes to try and read.\n";

                   do
                   {
                       cin >> input;
                       for (int i = 0; i < input; ++i)
                       {
                           if (port.read(6))
                           {
                               short x = getShort(port.data, 0);
                               short y = getShort(port.data, 2);
                               short z = getShort(port.data, 4);
                               printf(">>> Input: %d %d %d\n", x, y, z);
                               cout << "    Change in pitch: " << jpitch(x, y, z) << endl;
                               cout << "    Change in roll: " << jroll(x, y, z) << endl;
                           }
                       }
                   }
                   while (input > 0);
                }
            }
            catch (const boost::system::system_error &ex)
            {
                std::cout << ex.what() << "\n";
                std::cout << "Error: Could not open serial port.\n";
            }
        }
        else if (in == '5')
        {
            string port_name;
            cout << "Serial port: ";
            cin >> port_name;

            Serial port(port_name, 6, "\x80\x80");
            if (port.status == Serial::IDLE)
            {
               int input;
               cout << "Opened serial port. Type in timeout duration in milliseconds, or -2 to quit.\n";

               port.mode = Serial::MODE_ABORT;

               do
               {
                   cin >> input;
                   if (port.read(6, input))
                   {
                       short x = getShort(port.data, 0);
                       short y = getShort(port.data, 2);
                       short z = getShort(port.data, 4);
                       printf(">>> Input: %d %d %d\n", x, y, z);
                       cout << "    Change in pitch: " << jpitch(x, y, z) << endl;
                       cout << "    Change in roll: " << jroll(x, y, z) << endl;
                   }
                   else
                   {
                       cout << ">>> TIMEOUT" << endl;
                   }
               }
               while (input > -2);
            }
        }
        else if (in == '6')
        {

        }
        else
            break;
    }
    return 0;
}
