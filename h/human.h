#ifndef human_h
#define human_h

#include "decider.h"

#include <string>

using namespace std;

class player;

class human : public decider {
	public:
		int mode;
		
		human(player * playingP, int modeP);
		virtual int play();
		virtual vector<int> choose_cards(vector<card *> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id);
		virtual int choose_player(vector<int> pos, string message, int id);
		virtual bool may(string message, int id);
		
	private:
		string cmd;
		string word;
		string get_word();
		bool handle_info(string message);
};

#include "player.h"

#endif