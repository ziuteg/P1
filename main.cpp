#include "barbershop.h"

void run(int barber_cnt, int customer_cnt) {
	barbershop shop(barber_cnt, customer_cnt);
	std::thread barber_thread[barber_cnt];
	std::thread customer_thread[customer_cnt];

	for (int i = 0; i < barber_cnt; ++i) {
		barber_thread[i] = std::thread(barber_function, std::ref(shop), i);
	}

	for (int i = 0; i < customer_cnt; ++i) {
		customer_thread[i] = std::thread(customer_function, std::ref(shop), i);
	}

	for (int i = 0; i < customer_cnt; ++i) {
		customer_thread[i].join();
	}
	
	shop.close();

	for (int i = 0; i < barber_cnt; ++i) {
		barber_thread[i].join();
	}
}

int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " [barbers] [customers]\n";
		return 1;
	}

	run(atoi(argv[1]), atoi(argv[2]));

	return 0;
}