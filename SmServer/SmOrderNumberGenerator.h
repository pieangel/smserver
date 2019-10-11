#pragma once
class SmOrderNumberGenerator
{
private:
	static int _ID;
public:
	static void SetID(int id);
	static int GetID() {
		return ++_ID;
	}
	static void LoadOrderNo();
};

