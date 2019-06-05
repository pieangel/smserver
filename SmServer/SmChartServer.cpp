//#include "pch.h"
#include "SmChartServer.h"
#include "SmListener.h"
#include "SmWebsocketSession.h"
#include "SmHttpSession.h"
#include "SmSessionManager.h"
#include <string>
#include "SmConfigManager.h"
#include "Xml/pugixml.hpp"

void SmChartServer::ThreadMain()
{
	Start();
}

void SmChartServer::Init()
{
}

void SmChartServer::Start()
{
	SmConfigManager* configMgr = SmConfigManager::GetInstance();
	std::string appPath = configMgr->GetApplicationPath();
	std::string configPath = appPath;
	configPath.append("\\Config\\Config.xml");
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(configPath.c_str());
	pugi::xml_node app = doc.first_child();
	pugi::xml_node server_info = doc.child("application").child("server_info");

	std::string addr = server_info.child("ip").text().as_string();
	std::string por = server_info.child("port").text().as_string();
	std::string path = server_info.child("root_path").text().as_string();
	std::string thread_count = server_info.child("thread_count").text().as_string();

	//std::string addr = "192.168.0.38";
	//std::string addr = "192.168.1.9";
	//std::string por = "9991";
	//std::string path = "C:\\Server\\";
	//std::string thread_count = "5";
	auto const address = net::ip::make_address(addr.c_str());
	auto const port = static_cast<unsigned short>(std::atoi(por.c_str()));
	auto const doc_root = path;
	auto const threads = std::max<int>(1, std::atoi(thread_count.c_str()));

	// The io_context is required for all I/O
	net::io_context ioc{ threads };

	// Create and launch a listening port
	std::make_shared<SmListener>(
		ioc,
		tcp::endpoint{ address, port },
		std::make_shared<SmSessionManager>(doc_root))->run();

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	net::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&](beast::error_code const&, int)
		{
			// Stop the `io_context`. This will cause `run()`
			// to return immediately, eventually destroying the
			// `io_context` and all of the sockets in it.
			ioc.stop();
		});

	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for (auto i = threads - 1; i > 0; --i)
		v.emplace_back(
			[&ioc]
			{
				ioc.run();
			});
	ioc.run();

	// (If we get here, it means we got a SIGINT or SIGTERM)

	// Block until all the threads exit
	for (auto& t : v)
		t.join();
}
