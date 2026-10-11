#ifndef AI_h
#define AI_h

#include "decider.h"

using namespace std;

class player;

class AI : public decider {
	public:
		AI(player * playingP);
		virtual int play();
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id);
		virtual int choose_player(vector<int> pos, string message, int id);
		virtual bool may(string message, int id);
};

#include "player.h"

#endif