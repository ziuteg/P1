#ifndef __BARBER_H__
#define __BARBER_H__

#include <condition_variable>

class customer;

class barber {
	customer *cust;
	int id;
	bool selected;
	bool paid;
	bool chair;

public:
	std::condition_variable cv;
	barber(int id)
		: id(id) {
		selected = false;
		paid = false;
		chair = false;
	}
	void free() {
		selected = false;
		paid = false;
		chair = false;
	}
	int get_id() { return id; }
	void select(customer* cust);
	void pay() { paid = true; }
	void use_chair() { chair = true; }
	bool got_paid() { return paid; }
	bool is_selected() { return selected; }
	bool is_chair_taken() { return chair; }
	customer *get_customer() { return cust; }
};

#endif