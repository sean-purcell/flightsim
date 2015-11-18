#ifndef SERIALIN_HPP
#define SERIALIN_HPP

#include <boost/asio.hpp>

short getShort(const char *buffer, int start);

class Serial
{
    public:
        static const int INVALID;
        static const int IDLE;
        static const int SEEKING;
        static const int READING;

        boost::asio::serial_port *port;
        boost::asio::deadline_timer *timer;
        bool read_error;
        int status;

        std::string header;
        char * data;
        int len;

        Serial();
        ~Serial();
        Serial(std::string port_name, int buffer_len, std::string header);
        void reinit(std::string port_name);
        bool read(int nbytes, int timeout=-1);
    private:
        bool blocking_read(int nbytes);
        void blocking_seek_header();

        void seek_header_timeout();
        void timeout_read(int nbytes, int timeout);

};

/*class Serial
{
    public:
        static const int ERROR = -1;
        static const int IDLE = 0;
        static const int SEEKING = 1;
        static const int READING = 2;

        boost::asio::serial_port *port;
        boost::asio::deadline_timer timer;
        bool read_error;
        char * header;
        char * buffer;
        //int timeout;
        // status "seeking..."

        Serial();
        Serial(std::string port_name, int buffer_len, char * header);
        void read(int timeout);
        void seeking(const boost::system::error *error, std::size_t num_bytes);
        void read_complete(const boost::system::error *error, std::size_t num_bytes);
        void time_out(const boost::system::error *error);
    private:
        int seeker; // = 0
        int status; // = IDLE

};*/

#endif

