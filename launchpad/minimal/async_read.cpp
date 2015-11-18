#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace std;

boost::asio::serial_port *port = NULL;
boost::asio::deadline_timer *timer = NULL;
string status = "waiting";
char data[2] = {0};

void time_out(const boost::system::error_code& error)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return; // timeout cancelled; no need to do anything
    }

    // We have timed out, so kill the read operation.
    // The handlers for the cancelled operations will be passed
    // the boost::asio::error::operation_aborted error

    cout << "    Time out." << endl;
    port->cancel();
    status = "waiting";
}

void read_complete(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (error == boost::asio::error::operation_aborted)
    /* Read timed out. */
    {
        printf("    Read did not complete.\n");
    }
    else
    /* Read finished. Cancel the timeout timer if necessary. */
    {
        timer->cancel();
        cout << "    Read complete: ";
        unsigned short a = ((data[0] << 8) | (data[1]));
        cout << a << endl;
    }
    status = "waiting";
}

/** Minimal working example.
 * Reads from serial port and prints to console.
 */
int main()
{
    // Set up

    boost::asio::serial_port_base::baud_rate BAUD( 9600 );
    boost::asio::serial_port_base::character_size CHARSIZE( 8 );
    boost::asio::serial_port_base::flow_control FLOW( boost::asio::serial_port_base::flow_control::none );
    boost::asio::serial_port_base::parity PARITY( boost::asio::serial_port_base::parity::none );
    boost::asio::serial_port_base::stop_bits STOP( boost::asio::serial_port_base::stop_bits::one );

    std::string port_name = "COM3";
    //std::cout << "Serial port: ";
    //std::cin >> port_name;

    boost::asio::io_service io;
    try
    {
        port = new boost::asio::serial_port(io, port_name);

        port->set_option( BAUD );
        port->set_option( CHARSIZE );
        port->set_option( FLOW );
        port->set_option( PARITY );
        port->set_option( STOP );

        timer = new boost::asio::deadline_timer(port->get_io_service());

        // Open

        std::cout << "Opened serial port. Type in timeout duration, or -2 to quit.\n";

        // Input loop

        int timeout = 0;

        while (true)
        {
            std::cout << ">>> ";
            std::cin >> timeout;
            if (timeout < -2)
                break;

            // Read short

            if (status != "waiting")
                cout << "    Can't read; previous read operation still ongoing." << endl;
            else if (status == "waiting")
            {
                status = "reading";
                port->get_io_service().reset();
                timer->expires_from_now(boost::posix_time::milliseconds(timeout));
                boost::asio::async_read
                (
                    *port, boost::asio::buffer(data, 2),
                    boost::bind
                    (
                        &read_complete,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
                timer->async_wait
                (
                    boost::bind
                    (
                        &time_out,
                        boost::asio::placeholders::error
                    )
                );
                port->get_io_service().run();
            }
        }
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }

    return 0;
}
