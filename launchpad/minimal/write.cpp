#include <boost/asio.hpp>
#include <iostream>

/** Minimal working example.
 * Prompts user for byte and writes byte to serial mort.
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

        std::cout << "Opened serial port. Type an int in the range [0, 255].\n";

        // Input loop

        int input = 0;
        unsigned char command[1] = {0};

        while (true)
        {
            std::cout << ">>> ";
            std::cin >> input;
            if (input < 0 || input > 255)
                break;
            command[0] = (unsigned char)input;
            boost::asio::write(port, boost::asio::buffer(command, 1));
        }
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }

    return 0;
}
