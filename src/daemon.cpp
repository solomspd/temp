#include <bits/types/siginfo_t.h>
#include <boost/asio.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <sched.h>
#include <string>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
#include <vector>

#include "remote_proc.hpp"

int rfork(const std::string& adress);

int main(int argc, char* argv[])
{
    std::cout << "started remote fork server daemon" << std::endl;
    std::vector<remote_proc> db;
    return 0;
}

int rfork(const std::string& address)
{
    std::cout << "started remote fork procedure" << std::endl;
    std::cout << "lets begin" << std::endl
	      << "Process ID: " << getpid() << std::endl;

    int pid = fork();
    if (!pid) {
	// KILL and core dump child
	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP); // turns out ptrace stops after it gets invoked
    } else {
	//TODO: change over to modern smart pointers
	//std::unique_ptr<siginfo_t> child_siginfo;
	//std::unique_ptr<int> brk = sbrk(0); // this is a pointer to the end of the heap
	//std::unique_ptr<int> wstat;
	siginfo_t* child_siginfo;
	void* brk = sbrk(0); // this is a pointer to the end of the heap
	int* wstat = new int;
	wait(wstat);
	//int wait = waitid(P_PID, pid, child_siginfo, WSTOPPED); // turns out regular wait doesnt just wait for termination but also stops
	std::vector<char*> proc((long)brk);
	std::string dir = "/proc/" + std::to_string(pid);
	std::vector<std::pair<unsigned long, unsigned long>> maps;
	std::ifstream map_f(dir + "/maps");
	if (!map_f.is_open()) {
	    throw "Cannot open child memory map";
	}
	std::string temp_str;
	const std::regex exp("([0-9A-Fa-f]+)-([0-9A-Fa-f]+) ([-r])");
	std::smatch sm;
	while (std::getline(map_f, temp_str)) {
	    std::regex_search(temp_str, sm, exp);
	    maps.push_back(std::make_pair(std::stoi(sm[0]), std::stoi(sm[1])));
	}
	map_f.close();
	std::ifstream mem_f(dir + "/mem", std::ios::binary);
	if (!mem_f.is_open()) {
	    throw "Cannot Open child memory";
	}
	for (auto i : maps) {
	    int diff = i.first - i.second;
	    mem_f.seekg(i.first);
	    char* mem_block = new char[diff];
	    mem_f.read(mem_block, diff);
	    proc.push_back(mem_block);
	}
    }

    return 0; // TODO: Change to appropriate error or validation code
}
