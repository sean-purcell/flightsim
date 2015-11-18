#include <boost/asio.hpp>
#include <iostream>
#include <cstdlib>
#include "serial.hpp"

// TODO: destructor for new serial_port
// TODO: timeout read with async read

const int Serial::INVALID = -1;
const int Serial::IDLE = 0;
const int Serial::SEEKING = 1;
const int Serial::READING = 2;

Serial::Serial(){};

/** Construct a new serial port handler.
 *
 * This class handles read and write operations based on the assumption
 * that every packet of data is prefaced with a specific sequences of bytes
 * as specified in [header]. Can do both timeout operations and blocking operations.
 *
 * Args:
 *      port_name (std::string): Name of serial port, e.g. 'COM3' or '/dev/tty.usbmodem0E217381'
 *      buffer_len (int): Length of internal buffer.
 *      header (std::string): Default is "", which causes no stream syncing to occur
 */
Serial::Serial(std::string port_name, int buffer_len, std::string header="")
{
    port = NULL;
    this->header = header;
    data = new char[buffer_len];
    len = buffer_len;
    status = INVALID;
    reinit(port_name);
}

/** Delete Serial object
 */
Serial::~Serial()
{
    if (!port)
    {
        port->close();
        delete port;
    }
}

/** Reopen the serial port to the specified port name.
 *
 * If port is not NULL, that port will be closed and set to NULL
 * before it is reinitialized to the new serial port.
 */
void Serial::reinit(std::string port_name)
{
    boost::asio::serial_port_base::baud_rate BAUD( 9600 );
    boost::asio::serial_port_base::character_size CHARSIZE( 8 );
    boost::asio::serial_port_base::flow_control FLOW(  boost::asio::serial_port_base::flow_control::none );
    boost::asio::serial_port_base::parity PARITY(  boost::asio::serial_port_base::parity::none );
    boost::asio::serial_port_base::stop_bits STOP(  boost::asio::serial_port_base::stop_bits::one );
    boost::asio::io_service io;
    if (port)
    {
        status = INVALID;
        port->close();
        port = NULL;
    }
    try
    {
        port = new boost::asio::serial_port(io, port_name);
        port->set_option( BAUD );
        port->set_option( CHARSIZE );
        port->set_option( FLOW );
        port->set_option( PARITY );
        port->set_option( STOP );
        status = IDLE;
    }
    catch (const boost::system::system_error &ex)
    {
        status = INVALID;
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }
}

/** Read nbytes from serial port into data char array.
 *
 * If [timeout] is negative, method will block until all bytes are read.
 * Otherwise, method will attempt to read all bytes. Will return true immediately
 * once all bytes have been read. Will return false if timer expires and not all
 * bytes have been read. Regardless, method is guaranteed to return in
 * less than or equal to [timeout] milliseconds.
 *
 * Args:
 *      nbytes (int): number of bytes to read
 *      timeout (int): max time elapsed (ms) before method returns
 */
bool Serial::read(int nbytes, int timeout)
{
    if (status != IDLE)
        return false;

    if (timeout < 0)
        return blocking_read(nbytes);

    return false;
}

/** Read from serial port until header is found.
 * Then read nbytes into data char array.
 * Return true if nbytes are read. Return false if error.
 * Method will block until reading operation is complete.
 */
bool Serial::blocking_read(int nbytes)
{
    status = READING;
    try
    {
        status = SEEKING;
        blocking_seek_header();
        status = READING;
        boost::asio::read(*port, boost::asio::buffer(data, nbytes));
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        status = INVALID;
        return false;
    }
    status = IDLE;
    return true;
}

/** Read from serial port until header is found.
 * Method will return with the byte at the front of the
 * serial port being the byte immediately after the end
 * of the header. Method will block until header is found
 * in its entirety.
 */
void Serial::blocking_seek_header()
{
    int m = 0;
    while (m < header.length())
    {
        boost::asio::read(*port, boost::asio::buffer(data, 1));
        if (data[0] == header[m]) ++m;
        else m = 0;
    }
}

/** Convert bytes to a short, assuming big endian (MSB first).
 *
 * Args:
 *      buffer (char *): array of bytes as chars
 *      start (int): index of the first byte in the short that we want to extract
 */
short getShort(const char *buffer, int start)
{
    short result = (buffer[start] << 8) | (buffer[start+1]);
    return result;
}
