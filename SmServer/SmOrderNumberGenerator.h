#pragma once
class SmOrderNumberGenerator
{
private:
	static int _ID;
public:
	static void SetID(int id) {
		if (id > _ID)
			_ID = id;
	}
	static int GetID() {
		return ++_ID;
	}
};

