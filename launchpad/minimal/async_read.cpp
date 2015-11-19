#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>

using namespace std;

class Serial
{
    public:
        boost::asio::serial_port *port = NULL;
        boost::asio::deadline_timer *timer = NULL;
        string status = "waiting";
        char * data = new char[16];
        int len = 16;

        void init(std::string port_name)
        {
            boost::asio::io_service *io = new boost::asio::io_service();
            port = new boost::asio::serial_port(*io, port_name);

            boost::asio::serial_port_base::baud_rate BAUD( 9600 );
            boost::asio::serial_port_base::character_size CHARSIZE( 8 );
            boost::asio::serial_port_base::flow_control FLOW( boost::asio::serial_port_base::flow_control::none );
            boost::asio::serial_port_base::parity PARITY( boost::asio::serial_port_base::parity::none );
            boost::asio::serial_port_base::stop_bits STOP( boost::asio::serial_port_base::stop_bits::one );

            port->set_option( BAUD );
            port->set_option( CHARSIZE );
            port->set_option( FLOW );
            port->set_option( PARITY );
            port->set_option( STOP );

            timer = new boost::asio::deadline_timer(port->get_io_service());
        }

        void read(int timeout, int num)
        {
            if (status != "waiting")
                cout << "    Can't read; previous read operation still ongoing." << endl;
            else if (status == "waiting")
            {
                status = "reading";
                port->get_io_service().reset();
                timer->expires_from_now(boost::posix_time::milliseconds(timeout));
                cout << "    Reading " << num << " with timeout " << timeout << endl;
                boost::asio::async_read
                (
                    *port, boost::asio::buffer(data, 2*num),
                    boost::bind
                    (
                        &Serial::read_complete, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred
                    )
                );
                timer->async_wait
                (
                    boost::bind
                    (
                        &Serial::time_out, this,
                        boost::asio::placeholders::error
                    )
                );
                port->get_io_service().run();
            }
        }

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
            cout << "Result: " << error.message() << endl;
            if (error == boost::asio::error::operation_aborted)
            /* Read timed out. */
            {
                cout << "    Read did not complete. Only " << bytes_transferred /2 << " shorts (" << bytes_transferred << " bytes) read: ";
            }
            else
            /* Read finished. Cancel the timeout timer if necessary. */
            {
                timer->cancel();
                cout << "    Read complete: " << bytes_transferred /2 << " shorts (" << bytes_transferred << " bytes) read: ";;
            }
            for (int i = 0; i < bytes_transferred / 2; ++i)
            {
                unsigned short a = ((data[2*i] << 8) | (data[2*i+1]));
                cout << a << " ";
            }
            cout << endl;
            status = "waiting";
        }
};

/** Minimal working example.
 * Reads from serial port and prints to console.
 */
int main()
{
    // Set up

    std::string port_name = "COM3";
    //std::cout << "Serial port: ";
    //std::cin >> port_name;

    try
    {
        Serial serial;
        serial.init(port_name);

        // Open

        std::cout << "Opened serial port. Type in [number of shorts to read] and [timeout duration].\n";

        // Input loop

        int timeout = 0, num = 0;

        while (true)
        {
            std::cout << ">>> ";
            std::cin >> num >> timeout;
            if (timeout < -2)
                break;
            if (num < 1)
                break;

            if (2*num > serial.len)
            {
                serial.data = new char[2*num];
                serial.len = 2*num;
            }

            // Read short

            serial.read(timeout, num);
        }
    }
    catch (const boost::system::system_error &ex)
    {
        std::cout << ex.what() << "\n";
        std::cout << "Error: Could not open serial port.\n";
    }

    return 0;
}
