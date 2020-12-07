#include <SFML/Network.hpp>
#include <criu/criu.h>
#include <filesystem>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define local_port 3200
#define remote_port 3201

int checkpoint(int proc, const std::string &adress);

int restore(int proc);

void local_listener();

void remote_listener();

struct remote_proc{
	int orig_pid;
	std::string addr;
};

std::vector<remote_proc> db;

int main(int argc, char *argv[]) {
	std::cout << "started remote fork server daemon" << std::endl;
	std::thread local(local_listener);
	std::thread remote(remote_listener);
	local.join();
	remote.join();
	return 0;
}

void local_listener() {

	while (true) {
		// start listener and wait for new request for a fork
		sf::TcpListener listener;

		sf::Socket::Status stat = listener.listen(local_port);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot connect to web socket" << std::endl;
		}

		sf::TcpSocket connection;
		// start connection
		stat = listener.accept(connection);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot accept data from socket" << std::endl;
			return;
		}

		char data[1000]; // temporary variable to store data from local process
		std::size_t rec;
		// get pid and target address of local process
		stat = connection.receive(data, 1000, rec);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot receive pid and address from web socket" << std::endl;
			return;
		}
		data[rec] = '\0';
		std::string msg(data);
		int delim_n = msg.find(' ');
		std::string remote_addr = msg.substr(delim_n+1,msg.length());
		std::string s_pid = msg.substr(0,delim_n);
		int pid = std::stoi(s_pid);
		std::cout << "Received pid " << pid << " addr " << remote_addr << std::endl;
		int l_pid = atoi(data);
		data[rec] = '\0';

		// add this new process to be forked to our list of processes to keep track of
		remote_proc new_proc;
		new_proc.addr = connection.getRemoteAddress().toString();
		new_proc.orig_pid = l_pid;
		db.push_back(new_proc);
		std::string send = std::to_string(db.size());

		connection.disconnect();
		checkpoint(l_pid, remote_addr);

		// disconnect and connect to sync TCP calls
		stat = connection.connect("localhost", local_port);
		// send back our own psudo process id of fork to be created on remote host
		if (connection.send(send.c_str(), send.size()) != sf::Socket::Done) {
			std::cout << "Cannot send identifier back";
		}

		std::cout << "Completed checkpoint" << std::endl;


	}

}

void remote_listener() {

	while (true) {
		// start listener
		sf::TcpListener listener;
		sf::Socket::Status stat = listener.listen(remote_port);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot listen to web socket" << std::endl;
			return;
		}

		// accept connection from remote
		sf::TcpSocket connection;
		stat = listener.accept(connection);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot accept TCP connection" << std::endl;
		}

		std::cout << "Started receiving Pid" << std::endl;

		// get pid
		char data[1000];
		std::size_t  rec;
		stat = connection.receive(data, 1000, rec);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot receive original pid of forked process" << std::endl;
		}
		data[rec] = '\0';
		int proc = atoi(data);

		connection.disconnect();
		restore(proc);

		stat = connection.connect("localhost", local_port);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot connect to client" << std::endl;
		}

		stat = connection.send(0,4);
		if (stat != sf::Socket::Done) {
			std::cout << "Cannot send back pid of forked child" << std::endl;
		}

		std::cout << "Completed restore" << std::endl;

	}

}

int checkpoint(int proc, const std::string &address) {
	std::cout << "started remote fork procedure" << std::endl;

	// create checkpoint through CRIU
	criu_init_opts();

	criu_set_service_address("/var/run/criu_service.socket");

	criu_set_pid(proc);

	std::string img_path = "/tmp/dumped_process_" + std::to_string(proc);

	mkdir(img_path.c_str(), 0700);

	int dir_disc = open(img_path.c_str(), O_DIRECTORY);

	criu_set_images_dir_fd(dir_disc);

	criu_set_log_file("dump.log");
	criu_set_log_level(4);

	criu_dump();

	// Send files over network through FTP server.
	sf::Ftp ftp;
	ftp.connect(address, 21, sf::seconds(30));

	sf::Ftp::Response resp = ftp.login(); // Anonymous login to FTP server

	if (resp.isOk()) {

		std::filesystem::path dir(img_path);
		for (auto &file: std::filesystem::directory_iterator(dir)) {
			std::string local_dir = file.path().string();
			std::string remote_dir = "/tmp/restore_proc_" + file.path().filename().string();
			ftp.upload(local_dir, remote_dir, sf::Ftp::Binary);
		}

		ftp.disconnect();

		// delete checkpoint files after sending them to the server.
		std::filesystem::remove_all(dir);

		// send pid to remote node so we can start restore over there
		sf::TcpSocket connection;
		if (connection.connect(address, remote_port) != sf::Socket::Done) {
			std::cout << "cannot connect to remote host" << std::endl;
		}

		std::string data = std::to_string(proc);
		if (connection.send(data.c_str(), data.size()) != sf::Socket::Done) {
			std::cout << "Cannot send data to remote host" << std::endl;
		}

	} else {
		std::cout << "Cannot connect to ftp server, make sure anonymous permissions are enabled" << std::endl;
	}

	return 0;
}

int restore(int proc) {
	std::string img_path = "/tmp/restore_proc_" + std::to_string(proc);

	criu_init_opts();

	criu_set_service_address(NULL);

	int re_dir = open(img_path.c_str(), O_DIRECTORY);
	criu_set_images_dir_fd(re_dir);

	criu_set_log_file("restore.log");
	criu_set_log_level(4);

	criu_restore();

	// delete temporary files we will not use anymore
	std::filesystem::remove_all(std::filesystem::path(img_path));


	return 0;
}
