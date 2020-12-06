#include <iostream>
#include <string>

#include "remote_proc.hpp"

int rfork(const std::string& address);

int main(int argc, char* argv[])
{ // Pre demo testing process. remove when project is complete
    std::string addr = "192.168.1.20";
    rfork(addr);
    return 0;
}

int rfork(const std::string& address)
{
    std::cout << "started remote fork client" << std::endl;
    std::cout << "lets begin" << std::endl
	      << "Process ID: " << getpid() << std::endl;

    return 0; // TODO: Change to appropriate error or validation code
}
