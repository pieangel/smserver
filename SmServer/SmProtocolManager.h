#pragma once
#include "Global/TemplateSingleton.h"
#include <string>
#include "Json/json.hpp"
#include "SmServiceDefine.h"
#include <mutex>
class SmWebsocketSession;
// 모든 데이터는 메모리에서만 보낸다.
// 서버가 로드할 때 데이터베이스에서 모든 데이터를 읽어 온 다음에 시작한다.
// 클라이언트에는 무조건 메모리에 적재된 자료 구조에서만 전송을 하며
// 데이터 베이스에 접근하지 않는다.
class SmProtocolManager : public TemplateSingleton<SmProtocolManager>
{
public:
	SmProtocolManager();
	~SmProtocolManager();

	void OnMessage(std::string message, SmWebsocketSession* socket);
private:
	void OnReqChartData(std::string message);
	void ParseMessage(std::string message, SmWebsocketSession* socket);
	void SendResult(std::string user_id, int result_code, std::string result_msg);
	void SendResult(int session_id, int result_code, std::string result_msg);
	void OnReqLogin(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnLogout(nlohmann::json& obj);
	void OnRegisterSymbol(nlohmann::json& obj);
	void OnUnregisterSymbol(nlohmann::json& obj);
	void OnRegisterChartCycleData(nlohmann::json& obj);
	void OnUnregisterChartCycleData(nlohmann::json& obj);
	void OnOrder(nlohmann::json& obj);
	void OnReqChartData(nlohmann::json& obj);
	void OnReqChartData(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnReqSiseData(nlohmann::json& obj);
	void OnReqHogaData(nlohmann::json& obj);
	void OnReqSymbolMaster(nlohmann::json& obj);
	void OnReqSymbolMasterAll(nlohmann::json& obj);
	void OnReqSiseDataAll(nlohmann::json& obj);
	void OnReqRecentSiseDataAll(nlohmann::json& obj);
	void OnReqRegisterRecentRealtimeSiseAll(nlohmann::json& obj);
	void OnReqMarketList(nlohmann::json& obj);
	void OnReqSymbolListByCategory(nlohmann::json& obj);
	void OnReqChartDataResend(nlohmann::json& obj);
	// 시세를 업데이트 한다. 모두에게 보낸다.
	// 시세를 업데이트 할때 차트 데이터는 업데이트 하지 않는다. 
	// 차트 데이터는 독립적으로 운영된다. 각 클라이언트들은 원본 차트데이터를 보존하고 있어야 하며
	// 업데이트는 화면상으로만 진행해야 한다.
	void OnReqUpdateQuote(nlohmann::json& obj);
	// 호가를 업데이트 한다. 모두에게 보낸다.
	void OnReqUpdateHoga(nlohmann::json& obj);
	void OnReqUpdateChartData(nlohmann::json& obj);
	void OnReqRegisterUser(nlohmann::json& obj);
	void OnReqUnregisterUser(nlohmann::json& obj);
	void OnReqChartDataOneByOne(nlohmann::json& obj, SmWebsocketSession* socket);
	void OnReqAccountList(nlohmann::json& obj);
	// 접수확인 주문들을 보낸다.
	void OnReqAcceptedList(nlohmann::json& obj);
	// 체결된 주문들을 보낸다.
	void OnReqFilledList(nlohmann::json& obj);
	// 요청한 주문들을 보낸다.
	void OnReqOrderList(nlohmann::json& obj);
	// 요청한 포지션들을 보낸다.
	void OnReqPositionList(nlohmann::json& obj);
	void SendResult(std::string user_id, SmProtocol protocol, int result_code, std::string result_msg);
	void SendResult(int session_id, SmProtocol protocol, int result_code, std::string result_msg);
	// 요청한 사용자에게만 보낸다.
	void OnReqResendChartDataOneByOne(nlohmann::json& obj);
	// 모두에게 보낸다.
	void OnReqCycleDataResendOneByOne(nlohmann::json& obj);
	void OnReqResetAccount(nlohmann::json& obj);
	void OnReqRecentHogaDataAll(nlohmann::json& obj);
};

