#include "barber.h"
#include "customer.h"

void barber::select(customer* cust) {
	this->cust = cust;
	selected = true;
	if(!cust->is_selected())
		cust->select(this);
}