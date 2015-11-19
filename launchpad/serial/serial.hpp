#ifndef SERIALIN_HPP
#define SERIALIN_HPP

#include <boost/asio.hpp>

short getShort(const char *buffer, int start);

class Serial
{
    public:
        /* Constants */
        static const int INVALID;
        static const int IDLE;
        static const int SEEKING;
        static const int READING;
        static const int WRITING;
        static const int ABORT;
        static const int MODE_ABORT; // abort read operation if timeout
        static const int MODE_FINISH; // if timeout, return, but continue trying to read

        /* Backend stuff */
        boost::asio::serial_port *port;
        boost::asio::deadline_timer *timer;
        int status;

        /* User can access these directly */
        void (*read_notify)(const boost::system::error_code &ec, std::size_t bytes_transferred);
        void (*write_notify)(const boost::system::error_code &ec, std::size_t bytes_transferred);
        std::string header;
        char * data;
        int len;
        int mode;

        /* Constructors and methods */
        Serial();
        ~Serial();
        Serial(std::string port_name, int buffer_len, std::string header="");
        void reinit(std::string port_name);
        bool read(int nbytes, int timeout=-1);
        void abort();

        /* Async handles */
        void async_next(const boost::system::error_code& error, std::size_t bytes_transferred);
        void time_out(const boost::system::error_code& error);

    private:
        /* Hidden stuff */
        bool blocking_read(int nbytes);
        void blocking_seek_header();

        bool timeout_read(int nbytes, int timeout);
        void async_seek_header(int bytes_transferred);
        void async_read(int nbytes, int bytes_transferred);

        int seeker;
        int nbytes;
};

#endif

