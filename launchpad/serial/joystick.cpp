#include <boost/asio.hpp>
#include "serial.hpp"
#include "joystick.hpp"
#include "angler.hpp"

Joystick::Joystick(){};

Joystick::Joystick(std::string port_name)
{
    pitch = 0;
    roll = 0;
    port = initSerial(port_name);
    header = "\xFE\xFF";
    buffer = new char[6];
}

/** Query Launchpad for updated accelerometer values.
 * Update the pitch and roll fields accordingly.
 * This is a blocking call.
 * Return true if the fields have been updated.
 * Return false if reading error or no serial port connected.
 */
bool Joystick::update()
{
    if (!port)
        return false;
    if (!readSerial(port, buffer, 6, header))
        return false;
    short x = getShort(buffer, 0);
    short y = getShort(buffer, 2);
    short z = getShort(buffer, 4);
    roll = jroll(x, y, z);
    pitch = jpitch(x, y, z);
    return true;
}
