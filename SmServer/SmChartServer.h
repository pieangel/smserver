#pragma once
#include <thread>
#include <vector>
class SmChartServer
{
public:
	void Init();
	void Start();

	SmChartServer() : the_thread(&SmChartServer::ThreadMain, this) {}
	~SmChartServer() {
		stop_thread = true;
		the_thread.join();
	}
private:
	std::thread the_thread;
	bool stop_thread = false; // Super simple thread stopping.
	void ThreadMain();
};

