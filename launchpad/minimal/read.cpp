#include <boost/asio.hpp>
#include <iostream>

using namespace std;

/** Minimal working example.
 * Reads from serial port and prints to console.
 */
int main()
{
    // Set up

    boost::asio::serial_port_base::baud_rate BAUD( 9600 );
    boost::asio::serial_port_base::character_size CSIZE( 8 );
    boost::asio::serial_port_base::flow_control FLOW( boost::asio::serial_port_base::flow_control::none );
    boost::asio::serial_port_base::parity PARITY( boost::asio::serial_port_base::parity::none );
    boost::asio::serial_port_base::stop_bits STOP( boost::asio::serial_port_base::stop_bits::one );

    std::string port_name;
    std::cout << "Serial port: ";
    std::cin >> port_name;

    boost::asio::io_service io;
    try
    {
       boost::asio::serial_port port(io, port_name);

        port.set_option( BAUD );
        port.set_option( CSIZE );
        port.set_option( FLOW );
        port.set_option( PARITY );
        port.set_option( STOP );

        // Open

        std::cout << "Opened serial port. Type in number of bytes to try and read.\n";

        // Input loop

        int input = 0;
        unsigned char data[1] = {0};

        while (true)
        {
            std::cout << ">>> ";
            std::cin >> input;
            if (input < 1)
                break;

            while (input != 0)
            {
                boost::asio::read(port, boost::asio::buffer(data, 1));
                std::cout << data[0] << " ";
                --input;
            }
            std::cout << "\n";
        }
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }

    return 0;
}
