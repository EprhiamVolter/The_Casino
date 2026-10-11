#ifndef decider_h
#define decider_h

#include <vector>
#include <string>

using namespace std;

class player;
class card;

class decider {
	public:
		player * playing;
		bool human_player;
		
		virtual int play() = 0;
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) = 0;
		virtual int choose_player(vector<int> pos, string message, int id) = 0;
		virtual bool may(string message, int id) = 0;
};

#include "player.h"
#include "card.h"

#endif