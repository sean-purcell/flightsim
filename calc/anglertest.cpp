#include <iostream>
#include <cstdio>
#include <SFML/System.hpp>
#include <boost/asio.hpp>
#include "angler.hpp"
#include "rotation.hpp"
#include "quaternion.hpp"
#include "serialin.hpp"

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
        cout << "4. Serial" << endl;
        cout << "0. Exit" << endl;
        cin >> in;

        if (in == '1')
        {
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

            boost::asio::serial_port * port = initSerial(port_name);
            if (port)
            {
               cout << "Opened serial port." << endl;
               char data[6] = {0};
               char header[] = "\xFE\xFF";
               while (true)
               {
                   readSerial(port, data, 6, header);
                   short x = getShort(data, 0);
                   short y = getShort(data, 2);
                   short z = getShort(data, 4);
                   cout << "Change in pitch: " << jpitch(x, y, z) << endl;
                   cout << "Change in roll: " << jroll(x, y, z) << endl;
               }
            }
        }
        else
            break;
    }
    return 0;
}
