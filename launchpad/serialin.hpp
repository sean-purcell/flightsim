#ifndef SERIALIN_HPP
#define SERIALIN_HPP

#include <boost/asio.hpp>

short getShort(const char *buffer, int start);
void readSerial(boost::asio::serial_port * port, char * data, int len, const char header[]);
boost::asio::serial_port * initSerial(std::string port_name);

#endif

