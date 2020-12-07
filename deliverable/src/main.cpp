#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "lib.cpp"

int main(int argc, char* argv[])
{
    std::cout << "started demo process" << std::endl;

	std::string addr = "localhost";
	if (argc != 1) {
		addr = argv[1];
	}

    int forked_process = myfork(addr);

    std::cout << "id of remote process is " << forked_process << std::endl;

    struct passwd *pw = getpwuid(getuid());
    std::string dir(pw->pw_dir);
	dir += "/test.txt";
    std::ofstream out(dir);
    out << "testing fork";
    out.close();

    return 0;
}
