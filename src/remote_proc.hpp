#pragma once

#include <boost/asio.hpp>
#include <string>

class remote_proc {
private:
    boost::asio::ip::address ip;
    std::string host_name;
    unsigned int id;

public:
    remote_proc();
    ~remote_proc();
};
