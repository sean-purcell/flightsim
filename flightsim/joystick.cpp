#include <boost/asio.hpp>
#include <iostream>
#include "serial.hpp"
#include "joystick.hpp"
#include "angler.hpp"

const char Joystick::REQUEST = 'R'; // Character signalling a request for data
const char * Joystick::HEADER = "\xC0\x80"; // Sequence of bytes preceding each packet sent by Launchpad
const int Joystick::LEN = 10; // Number of bytes per packet, excluding header

Joystick::Joystick(){};

/** Create a new Joystick linked to the specified serial port.
 */
Joystick::Joystick(std::string port_name)
{
    pitch = 0;
    roll = 0;
    throttle = 0;
    num_pending = 0;
    port = new Serial(port_name, LEN, HEADER, Serial::MODE_FINISH);
}

/** Deconstruct Joystick object and serial port.
 */
Joystick::~Joystick()
{
    delete port;
}

/** Empty serial port input buffer.
 */
void Joystick::flush()
{
    num_pending = 0;
    port->flush();
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
    if (port->mode == Serial::MODE_FINISH) return update_mode_finish(timeout);
    else if (port->mode == Serial::MODE_ABORT) return update_mode_abort(timeout);
    return false;
}

/** Attempt to read data from serial port.
 *
 * Return true if read successful and false if timed out.
 * If timeout, abort read.
 */
bool Joystick::update_mode_abort(int timeout)
{
    if (num_pending == 0)
        if (!write_request())
            return false;

    if (!port->read(LEN, timeout))
        return false;

    --num_pending;
    recalc_angles();
    return true;
}

/** Attempt to read data from serial port.
 *
 * Return true if read succesful and false if timed out.
 * If timeout, suspend read and return. Read will
 * be completed with next invocation of poll().
 */
bool Joystick::update_mode_finish(int timeout)
{
    // If in MODE_FINISH, poll serial port for any unfinished tasks that can execute immediately
    bool read = false;
    if (num_pending > 0)
        if (port->poll())
            read = true;

    if (port->get_status() != Serial::IDLE)
        return false;

    // If serial is IDLE, then it either has nothing to do or it has just completed a read.
    if (num_pending > 0)
        --num_pending;

    // If no requests are pending, send a request.
    if (num_pending == 0)
        write_request();

    // Try to read if not done so already
    if (!read)
    {
        if (port->read(LEN, timeout))
        {
            read = true;
            --num_pending;
        }
    }
    if (read)
        recalc_angles();

    return read;
}

/** Write request to Launchpad for a data packet.
 *
 * Return true if request was written; return false if error.
 */
bool Joystick::write_request()
{
    if (port->write(&REQUEST, 1))
    {
        ++num_pending;
        return true;
    }
    return false;
}



/** Helper method for update function.
 *
 * Extract shorts from data buffer in serial port and invoke angler methods
 * to get pitch and roll angle. Update the pitch and roll fields.
 */
void Joystick::recalc_angles()
{
    short x = getShort(port->data, 0);
    short y = getShort(port->data, 2);
    short z = getShort(port->data, 4);
    roll = jroll(x, y, z);
    pitch = jpitch(x, y, z);
    throttle = getInt(port->data, 6);
}


