#include <boost/asio.hpp>
#include <iostream>
#include "serial.hpp"
#include "joystick.hpp"
#include "angler.hpp"

const char Joystick::REQUEST = 'R'; // Character signalling a request for data
const char * Joystick::HEADER = "\x80\x80"; // Sequence of bytes preceding each packet sent by Launchpad
const int Joystick::LEN = 6; // Number of bytes per packet, excluding header

Joystick::Joystick(){};

/** Create a new Joystick linked to the specified serial port.
 */
Joystick::Joystick(std::string port_name)
{
    pitch = 0;
    roll = 0;
    num_pending = 0;
    port = new Serial(port_name, LEN, HEADER, Serial::MODE_ABORT);
}

/** Deconstruct Joystick object and serial port.
 */
Joystick::~Joystick()
{
    delete port;
}

/** Query Launchpad for updated accelerometer values.
 *
 * If [timeout] is negative, block until all values have been read. Otherwise,
 * if [timeout] is positive, return when either all value have been read or if
 * operation times out. Return true if read successfully completed and the roll
 * and pitch fields have been updated. Return false if read operation timed out.
 * Return false if reading error, or no serial port connected.
 *
 * Regardless, method is guaranteed to return in less than or equal to [timeout] milliseconds.
 *
 * Args:
 *      timeout (int): max time elapsed (in milliseconds) before method returns
 */
bool Joystick::update(int timeout)
{
    if (port->get_status() == Serial::INVALID) return false;

    // If no requests are pending, write a request to Launchpad.
    if (num_pending == 0)
    {
        if (port->write(&REQUEST, 1)) ++num_pending;
        else return false;
    }

    // Attempt to read from Launchpad
    if (!port->read(LEN, timeout)) return false;
    --num_pending;

    short x = getShort(port->data, 0);
    short y = getShort(port->data, 2);
    short z = getShort(port->data, 4);
    roll = jroll(x, y, z);
    pitch = jpitch(x, y, z);

    return true;
}
