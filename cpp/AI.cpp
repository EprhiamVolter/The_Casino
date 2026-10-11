#ifndef AI_cpp
#define AI_cpp

#include "AI.h"

#include <iostream>
#include <sstream>
#include <random>
#include <algorithm>

AI::AI(player * playingP) {
	playing = playingP;
	human_player = false;
}

int AI::play() {
	if (playing->spin == settled || playing->spin == missed) {
		playing->my_board->end_turn();
	} else if (playing->roll_sum > 6 || playing->position + playing->roll_sum >= playing->my_board->end_pos) {
		playing->settle();
	} else {
		playing->roll();
	}
	return 0;
}

vector<int> AI::choose_cards(vector<card*> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
	board * b = playing->my_board;
	if (pos.size() == 0) {
		for (int i = 0; i < from->size(); i++) {
			pos.push_back(i);
		}
	}
	if (max < pos.size()) {
		max = pos.size();
	}
	uniform_int_distribution<int> dist(min, max);
	int ammount = dist(b->gen);
	vector<int> ret;
	ret.reserve(ammount);
	if (repetitive) {
		uniform_int_distribution<int> dist2(0, pos.size() - 1);
		for (int i = 0; i < ammount; i++) {
			ret.push_back(pos.at(dist2(b->gen)));
		}
	} else {
		shuffle(pos.begin(), pos.end(), b->gen);
		for (int i = 0; i < ammount; i++) {
			ret.push_back(pos.at(i));
		}
	}
	return ret;
}

int AI::choose_player(vector<int> pos, string message, int id) {
	board * b = playing->my_board;
	if (pos.size() <= 1) {
		bool others = pos.size() == 1 && pos.at(0) == -1; 
		for (int i = 0; i < playing->my_board->player_count; i++) {
			if (!others || i != playing->id) {
				pos.push_back(i);
			}
		}
	}
	uniform_int_distribution<int> dist(0, pos.size() - 1);
	return pos.at(dist(b->gen));
}

bool AI::may(string message, int id) {
	return true;
}

#endif