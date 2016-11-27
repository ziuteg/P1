#ifndef __BARBERSHOP_H__
#define __BARBERSHOP_H__

#include <condition_variable>
#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <queue>

#include "barber.h"
#include "customer.h"

#define WAITING_ROOM_CAP 5

class barbershop {
	std::mutex mtx_enter;
	std::mutex mtx_cash;
	std::mutex mtx_pay;
	std::mutex mtx_print;
	std::mutex mtx_cut;
	bool exit_flag;
	bool cash_register;
	std::queue <customer*> waiting_room;
	//std::queue <customer*> sofa;
	std::queue <barber*> barbers_asleep;

public:
	barbershop(int barber_cnt, int customer_cnt) {
		exit_flag = false;
		cash_register = false;
	}
	void close();
	bool closed() { return exit_flag; }
	bool full() { return waiting_room.size() == WAITING_ROOM_CAP; }

	bool begin_work(barber& barb);
	void cut_hair(barber& barb);
	void accept_payment(barber& barb);

	bool enter(customer& cust);
	void get_hair_cut(customer& cust);
	void pay(customer& cust);
};

void barber_function(barbershop& shop, int i);
void customer_function(barbershop& shop, int i);

#endif
