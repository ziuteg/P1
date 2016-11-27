#include "barbershop.h"

void barber_function(barbershop& shop, int i) {
	barber barb(i);
	while (!shop.closed()) {
		if (shop.begin_work(barb)) {
			shop.cut_hair(barb);
			shop.accept_payment(barb);
		}
	}
}

void customer_function(barbershop& shop, int i) {
	customer cust(i);
	if (shop.enter(cust)) {
		shop.get_hair_cut(cust);
		shop.pay(cust);
	}
}

bool barbershop::enter(customer& cust) {
	std::unique_lock<std::mutex> lock(mtx_enter);

	barber *my_barber;
	if (!barbers_asleep.empty()) {
		my_barber = barbers_asleep.front();
		barbers_asleep.pop();
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" << cust.get_id() << "] notices that BARBER[" << my_barber->get_id() << "] is asleep.\n"; 
		}
		cust.select(my_barber);
	}
	else if (!sofa_full()) {
		sofa.push(&cust);
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" <<  cust.get_id() << "] sits on sofa.\n";
		}
		cust.cv.wait(lock, [&cust]{ return cust.is_selected(); });
	}
	else if (!waiting_room_full()) {
		waiting_room.push(&cust);
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" <<  cust.get_id() << "] awaits in the waiting room.\n";
		}
		cust.cv.wait(lock, [&cust]{ return cust.is_selected(); });
	}
	else {
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" << cust.get_id() << "] couldn't enter the barbershop. The waiting room is full!\n";
		}
		return false;
	}
	return true;
}

void barbershop::get_hair_cut(customer& cust) {
	std::unique_lock<std::mutex> lock(mtx_cut);
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] sits on BARBERS'[" << cust.get_barber()->get_id() << "] chair and waits to get haircut.\n";
	}
	cust.get_barber()->use_chair();
	cust.get_barber()->cv.notify_one();
	cust.cv.wait(lock, [&cust]{ return cust.got_haircut(); });
}

void barbershop::pay(customer& cust) {
	std::unique_lock<std::mutex> cash_register_lock(mtx_cash);
	std::unique_lock<std::mutex> payment_lock(mtx_pay);
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] pays BARBER[" << cust.get_barber()->get_id() << "].\n";
	}
	cust.get_barber()->pay();
	cust.get_barber()->cv.notify_one();
	cust.cv.wait(payment_lock, [&cust]{ return !cust.is_selected(); });
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] goes home.\n";
	}
}

bool barbershop::begin_work(barber& barb) {
	std::unique_lock<std::mutex> lock(mtx_enter);
	customer *my_customer;
	if (!sofa.empty()) {
		my_customer = sofa.front();
		sofa.pop();
		barb.select(my_customer);
		my_customer->cv.notify_one();
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "BARBER[" << barb.get_id() << "] invites CUSTOMER[" << my_customer->get_id() << "].\n";
		}
		if (!waiting_room.empty()) {
			customer* moved_customer = waiting_room.front();
			waiting_room.pop();
			sofa.push(moved_customer);
			{
				std::lock_guard<std::mutex> lck_print(mtx_print);
				std::cout << "CUSTOMER[" <<  moved_customer->get_id() << "] moves from waiting room to sofa.\n";
			}
		}

		barb.cv.wait(lock, [&barb]{ return barb.is_chair_taken(); });
	} 
	else {
		barbers_asleep.push(&barb);
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "BARBER[" << barb.get_id() << "] checks the waiting room, but it's empty. He goes to sleep.\n";
		}
		barb.cv.wait(lock, [&barb, this]{ return (barb.is_selected() || closed()); });
		if (closed()) {	
			return false;
		}
		my_customer = barb.get_customer();
	}
	return true;
}

void barbershop::cut_hair(barber& barb) {
	std::unique_lock<std::mutex> lock(mtx_cut);
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "BARBER[" << barb.get_id() << "] cuts CUSTOMERS'[" << barb.get_customer()->get_id() << "] hair.\n";
	}
	barb.get_customer()->cut_hair();
	barb.get_customer()->cv.notify_one();
}

void barbershop::accept_payment(barber& barb) {
	std::unique_lock<std::mutex> lock(mtx_pay);
	barb.cv.wait(lock, [&barb]{ return barb.got_paid(); });
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "BARBER[" << barb.get_id() << "] accepts payment from CUSTOMER[" << barb.get_customer()->get_id() << "].\n";
	}
	barb.get_customer()->free();
	barb.get_customer()->cv.notify_one();
	barb.free();
}

void barbershop::close() {
	std::lock_guard<std::mutex> lock(mtx_enter);
	exit_flag = true;
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "Barbershop is going to be closed. Barbers wake up and go home.\n";
	}
	while (!barbers_asleep.empty()) {
		barbers_asleep.front()->cv.notify_one();
		barbers_asleep.pop();
	}
}
