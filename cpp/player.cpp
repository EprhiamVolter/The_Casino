#ifndef player_cpp
#define player_cpp

#include "player.h"

#include "human.h"
#include "AI.h"
#include <sstream>
#include <algorithm>

player::player(board * my_boardP, int idP, int mode) : 
	my_board(my_boardP),
	id(idP), 
	position(0),
	spin(before),
	roll_sum(0),
	hand(new card_pile(my_boardP)),
	temp_spin_max(my_board->spin_max),
	settle_prizes(1),
	prize_options(1),
	played_this_turn(0),
	yellow_played(false),
	ongoing(new card_pile(my_boardP))
		{
	switch(mode) {
		case 0: case 1: decide = new human(this, mode); break;
		case 2 : decide = new AI(this); break;
	}
}

int player::roll(int result) {
	if (spin == settled || spin == missed) {
		return -1;
	} else {
		spin = started;
		if (result == -1) {
			result = my_board->dice(my_board->gen);
		}
		roll_sum += result;
		ostringstream oss;
		oss << "Rolls a " << result <<  " for a total of " << roll_sum;
		my_board->log->add(oss.str(), true, 9);
		if (roll_sum > temp_spin_max) {
			oss = ostringstream();
			oss << "Misses a spin with " << roll_sum;
			my_board->log->add(oss.str(), false, 5);
			spin = missed;
		}
		my_board->log->step_up();
		return result;
	}
}

int player::settle() {
	if (spin == settled || spin == missed) {
		return -1;
	} else {
		spin = settled;
		ostringstream oss;
		oss << "Settles a spin with " << roll_sum;
		my_board->log->add(oss.str(), true, 5);
		move(roll_sum);
		if (my_board->spin_max <= roll_sum) {
			for (int i = 0; i < settle_prizes; i++) {
				my_board->draw_prize(id);
			}
		}
		my_board->log->step_up();
		return roll_sum;
	}
}

int player::play_from_hand(int pos) {
	if ((pos < 0) || (pos >= hand->size())) {
		return -1;
	} else if (my_board->ongoing->count(my_board->coll->ids->at("yield_sign")) != 0) {
		return -3;
	} else {
		card * played = hand->remove(pos);
		ostringstream oss;
		oss << "Plays " << my_board->coll->cards->at(played->id)->name;
		my_board->log->add(oss.str(), true, 6);
		my_board->coll->cards->at(played->id)->play(this);
		my_board->coll->cards->at(played->id)->handle(this, played);
		my_board->log->step_up();
		played_this_turn++;
		return played->id;
	}
}

int player::play_by_id(int id) {
	int i = 0;
	while (i < hand->size() && hand->at(i)->id != id) {
		i++;
	}
	if (i == hand->size()) {
		return -2;
	} else {
		return play_from_hand(i);
	}
}

void player::draw(int ammount) {
	my_board->draw(id, ammount);
}

void player::start_turn() {
	spin = before;
	roll_sum = 0;
	temp_spin_max = my_board->spin_max;
	settle_prizes = 1;
	prize_options = 1;
	played_this_turn = 0;
	yellow_played = false;
	while (!ongoing->empty()) {
		card * exp = ongoing->pop();
		my_board->ongoing->erase(exp->id);
		my_board->discard_pile->add(exp);
	}
	my_board->draw(id, 1);
}

void player::move(int ammount) {
	if (my_board->ongoing->count(my_board->coll->ids->at("speed_limit")) != 0 && (ammount > 5 || ammount < -5)) {
		if (ammount > 5) {
			ammount = 5;
		} else if (ammount < -5) {
			ammount = -5;
		}
		ostringstream oss;
		oss << "Movement reduced to " << ammount << " becouse player " << my_board->ongoing->at(my_board->coll->ids->at("speed_limit")) << " has speed limit ongoing";
		my_board->log->add(oss.str(), false, 4);
	}
	if (my_board->ongoing->count(my_board->coll->ids->at("portable_ice_wall")) != 0) {
		int wall = my_board->players->at(my_board->ongoing->at(my_board->coll->ids->at("portable_ice_wall")))->position;
		if (wall > position && wall <= position + ammount) {
			ammount = wall - position - 1;
			ostringstream oss;
			oss << "Movement reduced to " << ammount << " becouse player " << my_board->ongoing->at(my_board->coll->ids->at("portable_ice_wall"));
			oss << " has portable ice wall ongoing at " << wall;
			my_board->log->add(oss.str(), false, 4);
		}
	}
	ostringstream oss;
	oss << "Player " << id << " moves by " << ammount << ". From " << position << " to " << position + ammount;
	my_board->log->add(oss.str(), true, 4);
	position += ammount;
	if (position >= my_board->end_pos) {
		my_board->win(id);
	}
	my_board->log->step_up();
}

void player::teleport(int pos) {
	ostringstream oss;
	oss << "Player " << id << " teleports to " << pos << " from " << position;
	my_board->log->add(oss.str(), true, 4);
	position = pos;
	if (position >= my_board->end_pos) {
		my_board->win(id);
	}
	my_board->log->step_up();
}

void player::discard(vector<int> pos) {
	sort(pos.begin(), pos.end());
	ostringstream oss;
	oss << "Player " << id << " discards ";
	for (int i = pos.size() - 1; i > 0; i--) {
		card * disc = hand->remove(pos.at(i));
		oss << my_board->coll->cards->at(disc->id)->name << ", ";
		my_board->discard_pile->add(disc);
	}
	card * disc = hand->remove(pos.at(0));
	oss << my_board->coll->cards->at(disc->id)->name;
	my_board->discard_pile->add(disc);
	my_board->log->add(oss.str(), false, 8);
}

string player::to_string(bool self) {
	ostringstream oss;
	oss << "Player " << id << (decide->human_player ? " Human" : " AI") << ", Position " << position;
	if (self) {
		oss << "\n\tHand: " << hand->to_string() << "\n";
	} else {
		oss << ", Hand " << hand->size() << " cards\n";
	}
	return oss.str();
}

#endif 