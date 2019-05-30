#pragma once
#include <thread>
#include <vector>
class SmChartServer
{
public:
	void Init();
	void Start();

	SmChartServer() : _server_thread(&SmChartServer::ThreadMain, this) {}
	~SmChartServer() {
		_server_thread.join();
	}
private:
	std::thread _server_thread;
	void ThreadMain();
};

