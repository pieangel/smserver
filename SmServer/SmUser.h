#pragma once
#include <string>
class SmWebsocketSession;
class SmUser
{
private:
	std::string _Id;
	std::string _Password;
	std::string _Cert;
	SmWebsocketSession* _Socket = nullptr;
public:
	std::string Id() const { return _Id; }
	void Id(std::string val) { _Id = val; }
	std::string Password() const { return _Password; }
	void Password(std::string val) { _Password = val; }
	std::string Cert() const { return _Cert; }
	void Cert(std::string val) { _Cert = val; }
	SmWebsocketSession* Socket() const { return _Socket; }
	void Socket(SmWebsocketSession* val) { _Socket = val; }
};

