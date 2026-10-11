#ifndef board_h
#define board_h

#include "int_set.h"

#include <random>
#include <vector> 
#include <string>
#include <map> 
#include <tuple> 

using namespace std;

class logger;
class player;
class collection;
class card_pile;
class card;

class board {
	public:
		inline static const int spin_max = 10;
		inline static const int end_pos = 20;
		inline static const int start_hand = 1;
	
		collection * coll;
		bool complete;
		int winner;
		mt19937 gen; 
		uniform_int_distribution<int> dice;
		logger * log;
		int round;
		int turn;
		int player_count;
		vector<player *> * players;
		card_pile * draw_pile;
		card_pile * discard_pile;
		card_pile * prize_pile;
		card_pile * prize_discard;
		map<int,int> * ongoing;
		
		board(collection * my_coll, vector<int> * player_types);
		void exit(int by);
		void win(int by);
		void draw(int player_id, int ammount = 1, bool idenify = false);
		int draw_prize(int player_id, bool identify = false);
		void end_turn();
		string to_string(int by = -1);
};

#include "logger.h"
#include "player.h"
#include "collection.h"
#include "card_pile.h"
#include "card.h"

#endif