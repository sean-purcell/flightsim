#include <boost/asio.hpp>
#include <iostream>
#include <cstdlib>
#include "serialin.hpp"

/** Assuming big endian (MSB first).
 */
short getShort(const char *buffer, int start)
{
    short result = (buffer[start] << 8) | (buffer[start+1]);
    return result;
}

/** Reads from serial port until it comes across the specified header.
 * Then it reads len bytes from serial port into a char array and returns it.
 * Returned array does not include header.
 */
void readSerial(boost::asio::serial_port * port, char * data, int len = 6, const char header[] = "\xFE\xFF")
{
    int m = 0;
    while (header[m] != 0)
    {
        boost::asio::read(*port, boost::asio::buffer(data, 1));
        if (data[0] == header[m])
            ++m;
        else
            m = 0;
    }
    boost::asio::read(*port, boost::asio::buffer(data, len));
}

boost::asio::serial_port * initSerial(std::string port_name)
{
    boost::asio::serial_port_base::baud_rate BAUD( 9600 );
    boost::asio::serial_port_base::character_size CHARSIZE( 8 );
    boost::asio::serial_port_base::flow_control FLOW(  boost::asio::serial_port_base::flow_control::none );
    boost::asio::serial_port_base::parity PARITY(  boost::asio::serial_port_base::parity::none );
    boost::asio::serial_port_base::stop_bits STOP(  boost::asio::serial_port_base::stop_bits::one );
    boost::asio::serial_port *port = NULL;
    boost::asio::io_service io;
    try
    {
        boost::asio::serial_port new_port(io, port_name);
        port = &new_port;
        port->set_option( BAUD );
        port->set_option( CHARSIZE );
        port->set_option( FLOW );
        port->set_option( PARITY );
        port->set_option( STOP );
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }
    return port;
}

