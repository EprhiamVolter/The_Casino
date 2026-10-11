#ifndef logger_h
#define logger_h

#include "int_set.h"

#include <string>

using namespace std;

class board;

struct log_node {
	int id;					// sequential entry id
	string report;			// full message text
	int round;				// round the entry is made
	int turn;				// turn the entry is made
	int detail;				// level of detail of the entry
	int_set * visibility;	// players that can see this entry 
	bool alternative;
	string alt_report;		// players outside visibility see alt_report if alternative is true, otherwise nothing
	
	log_node * pa;			// parent
	log_node * fc; 		// first child
	log_node * lc;			// last child
	log_node * nb;			// next brother
	log_node * pb;			// previous brother
};

/*
log detail:
 0: Root
 1: Game start and game end
 2: Round starts
 3: Turn starts
 4: Player moves
 5: Spin result
 6: Card played and prize drawn
 7: Turn ends with draw
 8: Special effects 
 9: Roll result
10: Card and prize shuffle
*/

class logger {
	public:
		inline static const int max_detail = 10;
		
		board * my_board;
		int next_id;
		log_node * log_origin;
		log_node * log_pointer;
		
		logger(board * my_boardP);
		void add(string report, bool go_to_new = false, int detail = 0, int_set * visibility = nullptr, string alt_report = "");
		void step_up();
		log_node * next_log(log_node * it);
		string to_string_log(int since_round = 0, int since_turn = 0, int detail = -1, int by = -1);
		string to_string_log_rec(log_node * origin, int depth, int detail, int by);
};

#include "board.h"

#endif