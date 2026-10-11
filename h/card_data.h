#ifndef card_data_h
#define card_data_h

#include <string>

using namespace std;

class player;
class card;

class card_data {
	public:
		int id;
		int ammount;
		bool prize;
		string name;
		string card_set;
		string text;
		
		card_data(int idP, int ammountP, bool prizeP, string nameP, string card_setP, void (*playP)(player * playing), void (*handleP)(player * playing, card * this_card), string textP);
		void (*play)(player * playing);
		void (*handle)(player * playing, card * this_card);
		string to_string();
};

#include "player.h"
#include "card.h"

#endif