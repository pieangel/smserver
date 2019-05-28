#include "SmScheduler.h"
#include "Scheduler/Scheduler.h"
#include <chrono>
#include <iomanip>
#include "SmRealtimeSymbolServiceManager.h"
SmScheduler::SmScheduler()
{
	_Scheduler = new Bosma::Scheduler(12);
}

SmScheduler::~SmScheduler()
{
	if (_Scheduler) {
		delete _Scheduler;
		_Scheduler = nullptr;
	}
}

void SmScheduler::StartSymbolService()
{
	std::chrono::milliseconds milSec(200);
	_Scheduler->every(milSec, [this]() { this->SendRealtimeSymbol(); });
}

void SmScheduler::SendRealtimeSymbol()
{
	SmRealtimeSymbolServiceManager* rtlSymServiceMgr = SmRealtimeSymbolServiceManager::GetInstance();
	rtlSymServiceMgr->SendInfo();
}
