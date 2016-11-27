#include "customer.h"
#include "barber.h"

void customer::select(barber* barb) {
	this->barb = barb;
	selected = true;
	if(!barb->is_selected())
		barb->select(this);
}
