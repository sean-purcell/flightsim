#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <cstdlib>
#include "serial.hpp"

// TODO: destructor for new serial_port
// TODO: timeout read with async read
// TODO: flush?
// Reference: http://www.ridgesolutions.ie/index.php/2012/12/13/boost-c-read-from-serial-port-with-timeout-example/

const int Serial::INVALID = -1;
const int Serial::IDLE = 0;
const int Serial::SEEKING = 1;
const int Serial::READING = 2;
const int Serial::WRITING = 3;
const int Serial::MODE_ABORT = 42;
const int Serial::MODE_FINISH = 43;

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
Serial::Serial(std::string port_name, int buffer_len, std::string header)
{
    // Public stuff
    port = NULL;
    timer = NULL;
    mode = MODE_FINISH;
    this->header = header;
    data = new char[buffer_len];
    len = buffer_len;

    // Private stuff
    status = INVALID;
    seeker = 0;
    nbytes = 0;

    // Init serial port
    reinit(port_name);
}

/** Delete Serial object
 */
Serial::~Serial()
{
    if (!port)
    {
        port->close();
        timer->cancel();
        delete timer;
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
        timer->cancel();
        port->close();
        delete timer;
        delete port;
        timer = NULL;
        port = NULL;
        seeker = 0;
        nbytes = 0;
    }
    try
    {
        port = new boost::asio::serial_port(io, port_name);
        timer = new boost::asio::deadline_timer(port->get_io_service());
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
 * Otherwise, method will return when either all bytes have been read or if operation times out.
 * If operation times out in MODE_ABORT, method will cancel read and reset seeker.
 * If operation times out in MODE_FINISH, method will return, but read operation will
 * continue in the background until it is completed.
 *
 * Method will return true if read completed, and false otherwise.
 * Regardless, method is guaranteed to return in less than or equal to [timeout] milliseconds.
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

    printf(">>> Entering.\n");

    return timeout_read(nbytes, timeout);
}

/** Read from serial port until header is found.
 * Then read nbytes into data char array.
 * Return true if nbytes are read. Return false if error.
 * Method will block until reading operation is complete.
 */
bool Serial::blocking_read(int nbytes)
{
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

/** Read data from serial port, and return when either
 * all bytes have been read or if operation times out.
 *
 * Args:
 *      nbytes (int): Number of bytes to read
 *      timeout (int): Timeout duration in milliseconds
 */
bool Serial::timeout_read(int nbytes, int timeout)
{
    // Start seeking thread

    printf("   status: %d\n", status);
    status = SEEKING;
    printf("   status: %d\n", status);
    this->nbytes = nbytes;
    printf("   nbytes: %d\n", nbytes);
    port->get_io_service().reset();
    printf("   RESET\n", nbytes);
    //async_next(boost::system::errc::make_error_code(boost::system::errc::success), 0);
    //boost::asio::async_read(*port, boost::asio::buffer(data, 1), boost::bind(&dummy, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    printf("   ENTER\n", nbytes);

    // Start timeout timer

    timer->expires_from_now(boost::posix_time::milliseconds(timeout));
    printf("   SET EXPIRE %p\n", timer);
    std::cout << "    XP = " << boost::posix_time::to_iso_string(timer->expires_at()) << std::endl;
    timer->async_wait
    (
        boost::bind
        (
            &Serial::time_out, this,
            boost::asio::placeholders::error
        )
    );
    printf("   ASYNC WAIT\n", nbytes);
    port->get_io_service().run(); // TODO: Wait, if time_out doesn't cancel reading operations, wouldn't this block until those are done?
    printf("   RUN\n", nbytes);

    return status == IDLE;
}

/** Handler method for async_read.
 *
 * Central calling station for seeking, reading, and notifying methods.
 * Enter loop by calling this method by setting error to boost::system::errc::success
 * and bytes_transferred to 0. This indicates that this is an initializing iteration,
 * and that no bytes have been read or written yet.
 */
void Serial::async_next(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    printf("   -- Async %s %d\n", error.message().c_str(), bytes_transferred);
    printf("      Status %d\n", status);
    if (error == boost::asio::error::operation_aborted)
    /* Read timed out, so reset seeker and return. */
    {
        printf("      ABORT\n");
        status = IDLE;
        seeker = 0;
        return;
    }
    if (error)
    /* Some unknown error, uh-oh. Print error and flag serial port as invalid. */
    {
        status = INVALID;
        std::cout << "Read error: " << error.message() << std::endl;
        return;
    }
    if (status == SEEKING)
    /* We're in the middle of finding the header in the data stream.
     * Go on to the next byte.
     */
    {
        printf("      SEEK\n");
        async_seek_header(bytes_transferred);
    }
    else if (status == READING)
    /* We found the header in its entirety and now we need to read
     * the important bytes into the data char array.
     */
    {
        printf("      READ\n");
        async_read(this->nbytes, bytes_transferred);
    }
    else if (status == IDLE)
    /* We finished reading, so clean up. Cancel the timeout timer if necessary.
     * If there is a notify function set for reading operations, invoke it.
     */
    {
        printf("      FINISH\n");
        timer->cancel();
        if (read_notify)
            read_notify(error, bytes_transferred);
    }
}

void dummy(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    printf("        LLOLOLOLOKLOLOLOLOLOLOKOKOKFPKSFIJIOSJFIOJDIO\n");
}

/** Handler method invoked by async_next.
 *
 * If byte was read, compare byte with corresponding byte in header.
 * If match, increment the seeker index and async_read the next byte.
 * Otherwise, reset the seeker index to 0.
 *
 * If the header has been fully matched, i.e. seeker == len(header),
 * then set status to READING and invoke async_next.
 */
void Serial::async_seek_header(int bytes_transferred)
{
    if (bytes_transferred)
    /* Check if byte correspondings with header. */
    {
        printf("          + seek > compare\n");
        if (data[0] == header[seeker]) ++seeker;
        else seeker = 0;
    }
    if (seeker < header.length())
    /* We're not done finding the header yet. Read the next byte. */
    {
        printf("          + seek > async_read\n");
        boost::asio::async_read
        (
            *port, boost::asio::buffer(data, 1),
            boost::bind
            (
                &dummy,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }
    else
    /* We found the header. Go on to the data reading operation. */
    {
        printf("          + seek > end\n");
        seeker = 0;
        status = READING;
        async_next(boost::system::errc::make_error_code(boost::system::errc::success), 0);
    }
}

/** Handler method invoked by async_next.
 *
 * If bytes were read, set status to IDLE and invoke
 * async_next one last time to clean up.
 *
 * Otherwise, async_read the bytes.
 */
void Serial::async_read(int nbytes, int bytes_transferred)
{
    if (bytes_transferred)
    /* Yup, we're done. Let's just clean up. */
    {
        status = IDLE;
        async_next(boost::system::errc::make_error_code(boost::system::errc::success), bytes_transferred);
    }
    else
    /* Still need to read the actual data. */
    {
        boost::asio::async_read
        (
            *port, boost::asio::buffer(data, nbytes),
            boost::bind
            (
                &Serial::async_next, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
            )
        );
    }
}

/** Handler method for deadline_timer.
 *
 * If timed out, cancel all outstanding read/write operations if in MODE_ABORT mode.
 * Otherwise, do nothing.
 */
void Serial::time_out(const boost::system::error_code& error)
{
    if (error == boost::asio::error::operation_aborted)
    {
        return; // timeout cancelled; no need to do anything
    }

    // We have timed out, so kill the read operation.
    // The handlers for the cancelled operations will be passed
    // the boost::asio::error::operation_aborted error
    if (mode == MODE_ABORT)
    {
        port->cancel();
        seeker = 0;
        status = IDLE;
    }
}

/** Cancel any ongoing async operations.
 */
void Serial::abort()
{
    timer->cancel();
    port->cancel();
    seeker = 0;
    nbytes = 0;
    status = IDLE;
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
