#ifndef __CUSTOMER_H__
#define __CUSTOMER_H__

#include <condition_variable>

class barber;

class customer {
	barber *barb;
	int id;
	bool selected;
	bool haircut;

public:
	std::condition_variable cv;
	customer(int id)
		: id(id) {
		selected = false;
		haircut = false;
	}
	int get_id() { return id; }
	void free() { selected = false; }
	void select(barber *barb);
	void cut_hair() { haircut = true; }
	bool is_selected() { return selected; }
	bool got_haircut() { return haircut; }
	barber *get_barber() { return barb; }
};

#endif
