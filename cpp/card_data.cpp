#ifndef card_data_cpp
#define card_data_cpp

#include "card_data.h"
#include <sstream>

card_data::card_data(int idP, int ammountP, bool prizeP, string nameP, string card_setP, void (*playP)(player * playing), void (*handleP)(player * playing, card * this_card), string textP) :
	id(idP),
	ammount(ammountP),
	prize(prizeP),
	name(nameP),
	card_set(card_setP),
	play(playP),
	handle(handleP),
	text(textP)
		{
			
}

string card_data::to_string() {
	ostringstream oss;
	oss << "Id " << id << (prize ? ", Prize" : ", Regular") << ", Name " << name << ", Set " << card_set << "\nText " << text << "\n";
	return oss.str();
}

#endif