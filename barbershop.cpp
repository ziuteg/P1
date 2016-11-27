#include "barbershop.h"

void barber_function(barbershop& shop, int i) {
	barber barb(i);
	while(!shop.closed()) {
		if(shop.begin_work(barb)) {
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
	if (full()) { // waiting room is full, exit
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" << cust.get_id() << "] couldn't enter the barbershop. The waiting room is full!\n";
		}
		return false;
	}
	barber *my_barber; // initialize pointer to the barber that will serve the current customer
	if (!barbers_asleep.empty()) { // check if there are sleeping barbers (it should imply that the waiting room is empty)
		my_barber = barbers_asleep.front();
		barbers_asleep.pop();
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" << cust.get_id() << "] notices that BARBER[" << my_barber->get_id() << "] is asleep.\n"; 
		}
	}
	else { // there are no sleeping barbers, customer takes his place in the waiting room
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "CUSTOMER[" <<  cust.get_id() << "] awaits in the waiting room.\n";
		}
		waiting_room.push(&cust);
		cust.cv.wait(lock, [&cust]{ return cust.is_selected(); }); // customer waits for the barber to serve him. (wait should unlock mtx_cust)
		my_barber = cust.get_barber();
	}
	cust.select(my_barber);
	return true;
}

void barbershop::get_hair_cut(customer& cust) {
	std::unique_lock<std::mutex> lock(mtx_cut);
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] sits on BARBERS'[" << cust.get_barber()->get_id() << "] chair and waits to get haircut.\n";
	}
	cust.get_barber()->use_chair();
	cust.get_barber()->cv.notify_one(); // we notify the selected barber that there's a customer on his chair
	cust.cv.wait(lock, [&cust]{ return cust.got_haircut(); }); // customer waits for the barber to finish his haircut
}

void barbershop::pay(customer& cust) {
	std::unique_lock<std::mutex> cash_register_lock(mtx_cash);
	std::unique_lock<std::mutex> payment_lock(mtx_pay);
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] pays BARBER[" << cust.get_barber()->get_id() << "].\n";
	}
	cust.get_barber()->pay(); // pay barber
	cust.get_barber()->cv.notify_one(); // notify him about payment
	cust.cv.wait(payment_lock, [&cust]{ return !cust.is_selected(); }); // wait until he accepts payment
	{
		std::lock_guard<std::mutex> lck_print(mtx_print);
		std::cout << "CUSTOMER[" << cust.get_id() << "] goes home.\n";
	}
}

bool barbershop::begin_work(barber& barb) {
	std::unique_lock<std::mutex> lock(mtx_enter);
	customer *my_customer; // initialize pointer to the customer to be served
	if(waiting_room.empty()) { // check if waiting room is empty
		barbers_asleep.push(&barb); // add barber to the sleeping barbers queue
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "BARBER[" << barb.get_id() << "] checks the waiting room, but it's empty. He goes to sleep.\n";
		}
		barb.cv.wait(lock, [&barb, this]{ return (barb.is_selected() || closed()); }); // wait until some customer wakes barber up
		if(closed()) {
			return false; // shop was closed while barber was asleep
		}
		my_customer = barb.get_customer();
	} 
	else { // there are customers in the waiting room
		my_customer = waiting_room.front();
		waiting_room.pop();
		barb.select(my_customer); // barber selects customer to be served
		my_customer->cv.notify_one(); // barber tells customer to sit on his chair
		{
			std::lock_guard<std::mutex> lck_print(mtx_print);
			std::cout << "BARBER[" << barb.get_id() << "] invites CUSTOMER[" << my_customer->get_id() << "].\n";
		}
		barb.cv.wait(lock, [&barb]{ return barb.is_chair_taken(); });  // barber waits until customer sits on his chair
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
	//std::this_thread::sleep_for(std::chrono::milliseconds(20));
	barb.get_customer()->cv.notify_one(); // tell customer that his haircut is done
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
	while(!barbers_asleep.empty()) {
		barbers_asleep.front()->cv.notify_one();
		barbers_asleep.pop();
	}
}

/*
void customer::select(barber* barb) {
	this->barb = barb;
	selected = true;
	if(!barb->is_selected())
		barb->select(this);
}

void barber::select(customer* cust) {
	this->cust = cust;
	selected = true;
	if(!cust->is_selected())
		cust->select(this);
}
*/