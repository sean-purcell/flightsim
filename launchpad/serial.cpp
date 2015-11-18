#include <boost/asio.hpp>
#include <iostream>
#include <cstdlib>
#include "serial.hpp"

// TODO: destructor for new serial_port
// TODO: timeout read with async read

Serial::Serial(){};

Serial::Serial(std::string port_name, int buffer_len, std::string header="")
{
    port = NULL;
    this->header = header;
    data = new char[buffer_len];
    len = buffer_len;
    status = -1;
    init(port_name);
}

void Serial::init(std::string port_name)
{
    boost::asio::serial_port_base::baud_rate BAUD( 9600 );
    boost::asio::serial_port_base::character_size CHARSIZE( 8 );
    boost::asio::serial_port_base::flow_control FLOW(  boost::asio::serial_port_base::flow_control::none );
    boost::asio::serial_port_base::parity PARITY(  boost::asio::serial_port_base::parity::none );
    boost::asio::serial_port_base::stop_bits STOP(  boost::asio::serial_port_base::stop_bits::one );
    boost::asio::io_service io;
    try
    {
        port = new boost::asio::serial_port(io, port_name);
        port->set_option( BAUD );
        port->set_option( CHARSIZE );
        port->set_option( FLOW );
        port->set_option( PARITY );
        port->set_option( STOP );
        status = 0;
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }
}

void Serial::seek_header()
{
    int m = 0;
    while (m < header.length())
    {
        boost::asio::read(*port, boost::asio::buffer(data, 1));
        if (data[0] == header[m])
            ++m;
        else
            m = 0;
    }
}

/** Reads from serial port until it comes across the specified header.
 * Then it reads len bytes from serial port into a char array and returns it.
 * Returned array does not include header.
 */
bool Serial::read(int nbytes)
{
    try
    {
        seek_header();
        boost::asio::read(*port, boost::asio::buffer(data, nbytes));
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << "Error: " << ex.what() << "\n";
        return false;
    }
    return true;
}

/** Assuming big endian (MSB first).
 */
short getShort(const char *buffer, int start)
{
    short result = (buffer[start] << 8) | (buffer[start+1]);
    return result;
}
