#ifndef SERIALIN_HPP
#define SERIALIN_HPP

#include <boost/asio.hpp>

short getShort(const char *buffer, int start);
bool readSerial(boost::asio::serial_port * port, char * data, int len, const char header[]);
boost::asio::serial_port * initSerial(std::string port_name);

class Serial
{
    public:
        boost::asio::serial_port *port;
        std::string header;
        char * data;
        int len;
        int status;

        Serial();
        Serial(std::string port_name, int buffer_len, std::string header);
        void init(std::string port_name);
        bool read(int nbytes);
    private:
        void seek_header();

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

