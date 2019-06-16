#pragma once
enum class SmProtocol {
	req_none = 0,
	// 로그인 요청
	req_login,
	// 로그인 응답
	res_login,
	// 로그아웃 요청
	req_logout,
	// 로그아웃 응답
	res_logout,
	// 실시간 시세 등록 요청
	req_register_symbol,
	// 실시간 시세 등록 응답
	res_register_symbol,
	// 실시간 시세 해제 요청
	req_unregister_symbol,
	// 실시간 시세 해제 응답
	res_unregister_symbol,
	// 주기 차트 데이터 등록 요청
	req_register_chart_cycle_data,
	// 주기 차트 데이터 등록 응답
	res_register_chart_cycle_data,
	// 신규 주문 요청
	req_order_new,
	// 신규 주문 요청 응답
	res_order_new,
	// 정정 주문 요청
	req_order_modify,
	// 정정 주문 응답
	res_order_modify,
	// 취소 주문 요청
	req_order_cancel,
	// 취소 주문 응답
	res_order_cancel,
	// 주문 접수확인 응답
	res_order_accepted,
	// 주문 체결 응답
	res_order_filled,
	// 차트데이터 요청
	req_chart_data,
	// 차트 데이터 응답
	res_chart_data,
	// 시세 데이터 요청
	req_sise_data,
	// 시세 데이터 응답
	res_sise_data,
	// 실시간 호가 전송
	res_realtime_hoga,
	// 실시간 시세 전송
	res_realtime_sise,
	// 주기 차트 데이터 전송
	res_chart_cycle_data,
	// 주기 차트 데이터 해제 요청
	req_unregister_chart_cycle_data,
	// 주기 차트 데이터 해제 응답
	res_unregister_chart_cycle_data,
	// 호가데이터 요청
	req_hoga_data,
	// 호가데이터 응답
	res_hoga_data
};