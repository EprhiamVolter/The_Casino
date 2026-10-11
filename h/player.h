#ifndef player_h
#define player_h

#include <string>
#include <vector>

using namespace std;

class board;
class decider;
class card_pile;

enum spin_phase {
	before,
	started,
	settled,
	missed,
};

class player {
	public:
		board * my_board; 
		int id;
		decider * decide;
		int position;
		spin_phase spin;
		int roll_sum;
		card_pile * hand;
		int temp_spin_max;
		int settle_prizes;
		int prize_options;
		int played_this_turn;
		int yellow_played;
		card_pile * ongoing;
		
		player(board * my_boardP, int idP, int mode);
		int roll(int result = -1);
		int settle();
		int play_from_hand(int pos);
		int play_by_id(int id);
		void draw(int ammount);
		void start_turn();
		void move(int ammount);
		void teleport(int pos);
		void discard(vector<int> pos);
		string to_string(bool self = false);
};

#include "board.h"
#include "decider.h"
#include "card_pile.h"

#endif