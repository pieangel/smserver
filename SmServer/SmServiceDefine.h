#pragma once
enum class SmProtocol {
	req_none = 0,
	// �α��� ��û
	req_login,
	// �α��� ����
	res_login,
	// �α׾ƿ� ��û
	req_logout,
	// �α׾ƿ� ����
	res_logout,
	// �ǽð� �ü� ��� ��û
	req_register_symbol,
	// �ǽð� �ü� ��� ����
	res_register_symbol,
	// �ǽð� �ü� ���� ��û
	req_unregister_symbol,
	// �ǽð� �ü� ���� ����
	res_unregister_symbol,
	// �ֱ� ��Ʈ ������ ��� ��û
	req_register_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ��� ����
	res_register_chart_cycle_data,
	// �ű� �ֹ� ��û
	req_order_new,
	// �ű� �ֹ� ��û ����
	res_order_new,
	// ���� �ֹ� ��û
	req_order_modify,
	// ���� �ֹ� ����
	res_order_modify,
	// ��� �ֹ� ��û
	req_order_cancel,
	// ��� �ֹ� ����
	res_order_cancel,
	// �ֹ� ����Ȯ�� ����
	res_order_accepted,
	// �ֹ� ü�� ����
	res_order_filled,
	// ��Ʈ������ ��û
	req_chart_data,
	// ��Ʈ ������ ����
	res_chart_data,
	// �ü� ������ ��û
	req_sise_data,
	// �ü� ������ ����
	res_sise_data,
	// �ǽð� ȣ�� ����
	res_realtime_hoga,
	// �ǽð� �ü� ����
	res_realtime_sise,
	// �ֱ� ��Ʈ ������ ����
	res_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ���� ��û
	req_unregister_chart_cycle_data,
	// �ֱ� ��Ʈ ������ ���� ����
	res_unregister_chart_cycle_data,
	// ȣ�������� ��û
	req_hoga_data,
	// ȣ�������� ����
	res_hoga_data
};