#include <iostream>
#include <string>
#include <SFML/Network.hpp>

#define local_port 3200

int myfork(const std::string& address)
{
    std::cout << "started remote fork client" << std::endl;

	sf::TcpSocket client;
	sf::Socket::Status stat = client.connect("localhost", local_port);
	if (stat != sf::Socket::Done) {
		std::cout << "cannot establish connection with local daemon" << "std::endl";
		return -1;
	}

	// send current pid
	std::string msg = std::to_string(getpid()) + " " + address;
	// send message
	stat = client.send(msg.c_str(), msg.size());
	if (stat != sf::Socket::Done) {
		std::cout << "Cannot send address to local daemon" << std::endl;
		return -1;
	}

	// disconnect and reconnect to sync
	client.disconnect();

	sf::TcpListener listener;
	stat = listener.listen(3100);
	if (stat != sf::Socket::Done) {
		std::cout << "Cannot listen to return pid" << std::endl;
	}

	stat = listener.accept(client);
	if (stat != sf::Socket::Done) {
		std::cout << "Cannot accept pid" << std::endl;
	}
	char data[1000];
	std::size_t rec;
	stat = client.receive(data, 1000, rec);
	if (stat != sf::Socket::Done) {
		std::cout << "Cannot receive return pid" << std::endl;
		return -1;
	}

	data[rec] = '\0';
	int ret = atoi(data);

    return ret;
}
