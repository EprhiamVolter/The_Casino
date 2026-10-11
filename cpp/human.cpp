#ifndef human_cpp
#define human_cpp

#include "human.h"

#include <iostream>
#include <sstream>

human::human(player * playingP, int modeP) : 
	mode(modeP) 
		{
	playing = playingP;
	human_player = true;
}

string human::get_word() {
	size_t end = cmd.find(' ');
	word = cmd.substr(0, end);
	if (end == string::npos) {
		cmd.clear();
	} else {
		cmd = cmd.substr(end + 1, string::npos);
	}
	return word;
}

bool human::handle_info(string message) {
	while (true) {
		try {
			cout << message;
			getline(cin, cmd);
			get_word();
			
			while (word.empty()) {
				getline(cin,cmd);
				get_word();
			} if (word == "help" || word == "Help") {
				cout << "\texit : Exits\n" <<
				"Information:\n" <<
				"\thelp : Prints this list of commands\n" << 
				"\tboard : Prints all data on the board visible to the current player\n" << 
				"\tlog : Prints the log\n" << 
				"Game Actions\n" <<
				"\tr | roll : rolls in the current spin\n" <<
				"\ts | settle : settles the current spin\n" <<
				"\te | end : Ends the current turn\n";
			} else if (word == "board") {
				cout << "\n" << playing->my_board->to_string(playing->id);
			} else if (word == "log") {
				cout << "\n" << playing->my_board->log->to_string_log(0,0,-1,playing->id) << "\n";
			} else if (word == "exit") {
				playing->my_board->exit(playing->id);
				return false;
			} else {
				return true;
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
}

bool word_is_num(string word) {
	return (word.size() > 0) && (word[0] == '-' || (word[0] >= '0' && word[0] <= '9'));
}


int human::play() {
	board * b = playing->my_board;
	if (mode == 1 && (playing->spin == settled || playing->spin == missed)) {
		playing->my_board->end_turn();
		return 0;
	}
	ostringstream oss;
	oss << "Round " << b->round << ", Player " << b->turn << ", Position " << playing->position << ", Spin ";
	string spin_state;
	switch(playing->spin) {
		case before:
			spin_state = "before";
			break;
		case started:
			spin_state = "started";
			break;
		case settled:
			spin_state = "settled";
			break;
		case missed:
			spin_state = "missed";
			break;
	}
	oss << spin_state << "\n";
	while (handle_info(oss.str())) {
		try {
			if (word == "roll" || word == "r") {
				int result = playing->roll();
				if (result >= 0) {
					cout << "rolled a " << result << " for a total of " << playing->roll_sum << "\n";
					return 0;
				} else switch(result) {
					case -1 : cout << "Alredy spinned this turn\n"; break;
					default : cout << "Unknown error code: " << result << "\n";
				}
			} else if (word == "settle" || word == "s") {
				int result = playing->settle();
				if (result >= 0) {
					cout << "settled for " << playing->roll_sum << "\n";
					return 0;
				} else switch(result) {
					case -1 : cout << "Alredy spinned this turn\n"; break;
					default : cout << "Unknown error code: " << result << "\n";
				}
			} else if (word == "play" || word == "p") {
				get_word();
				if (word_is_num(word)) {
					int pos = stoi(word);
					int result = playing->play_from_hand(pos);
					if (result >= 0) {
						cout << b->coll->cards->at(result)->name << " played\n";
						return result;
					} else switch(result) {
						case -1 : cout << pos << " is out of range, hand has " << playing->hand->size() << " Cards\n"; break;
						case -3 : cout << "Cards can't be played now\n"; break;
						default : cout << "Unknown error\n";
					}
				} else if (b->coll->ids->count(word) == 1) {
					string name = word;
					int result = playing->play_by_id(b->coll->ids->at(word));
					if (result >= 0) {
						cout << name << " played\n";
						return result;
					} else switch(result) {
						case -2 : cout << name << " is not in the hand\n"; break;
						case -3 : cout << "Cards can't be played now\n"; break;
						default : cout << "Unknown error\n";
					}
				} else {
					cout << "\"" << word << "\" not found in the collection\n";
				}
			} else if (word == "end" || word == "e") {
				playing->my_board->end_turn();
				return 0;
				
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
	return 1;
}

vector<int> human::choose_cards(vector<card*> * from, vector<int> pos, int min, int max, bool repetitive, string message, int id) {
	board * b = playing->my_board;
	vector<int> ret;
	int choosed = 0;
	if (pos.size() == 0) {
		for (int i = 0; i < from->size(); i++) {
			pos.push_back(i);
		}
	}
	if (max < pos.size()) {
		max = pos.size();
	}
	ostringstream opts;
	opts << "Player " << playing->id << " : " << message << " Options:\n";
	string mess2 = opts.str();
	opts << b->coll->cards->at(from->at(pos.at(0))->id)->name;
	for (int i = 1; i < pos.size(); i++) {
		opts << ", " << b->coll->cards->at(from->at(pos.at(i))->id)->name;
	}
	opts << "\n";
	while (choosed < max && handle_info(opts.str())) {
		try {
			int position = 0;
			if (word_is_num(word)) {
				position = stoi(word);
				if (position == -1) {
					if (choosed >= min) {
						break;
					} else {
						cout << "Must choose at least " << min << " Cards. So far " << choosed << endl; 
						continue;
					}
				} else if (position < 0 || position >= pos.size()) {
					cout << position << " out of range " << pos.size() << endl;
					continue;
				}
			} else {
				if (word == "none" || word == "None") {
					if (choosed >= min) {
						break;
					} else {
						cout << "Must choose at least " << min << " Cards. So far " << choosed << endl; 
						continue;
					}
				} else if (b->coll->ids->count(word) == 0) {
					cout << word << " not found in the collection\n";
					continue;
				}
				int id = b->coll->ids->at(word);
				while (position < pos.size() && from->at(pos.at(position))->id != id) {
					position++;
				}
				if (position == pos.size()) {
					cout << word << " not in options\n";
					continue;
				}
			}
			ret.push_back(pos.at(position));
			choosed++;
			if (!repetitive) {
				pos.erase(pos.begin() + position);
				opts = ostringstream();
				opts << mess2 << b->coll->cards->at(from->at(pos.at(0))->id)->name;
				for (int i = 1; i < pos.size(); i++) {
					opts << ", " << b->coll->cards->at(from->at(pos.at(i))->id)->name;
				}
				opts << "\n";
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
	return ret;
}

int human::choose_player(vector<int> pos, string message, int id) {
	board * b = playing->my_board;
	if (pos.size() <= 1) {
		bool others = pos.size() == 1 && pos.at(0) == -1; 
		for (int i = 0; i < playing->my_board->player_count; i++) {
			if (!others || i != playing->id) {
				pos.push_back(i);
			}
		}
	}
	ostringstream opts;
	opts << "Player " << playing->id << " : " << message << " Options:\n";
	string mess2 = opts.str();
	opts << b->coll->cards->at(pos.at(0))->name;
	for (int i = 1; i < pos.size(); i++) {
		opts << ", " << b->coll->cards->at(pos.at(i))->name;
	}
	opts << "\n";
	while (true) {
		try {
			handle_info(opts.str());
			if (word_is_num(word)) {
				int choosen = stoi(word);
				for (int i = 0; i < pos.size(); i++) {
					if (choosen == pos.at(i)) {
						return choosen;
					}
				}
				cout << "Must be a player in the options, " <<choosen << " is not\n";
			} else {
				cout << "\"" << word << "\" not recognized, answer with the player number\n";
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
}

bool human::may(string message, int id) {
	ostringstream oss;
	oss << "Player " << playing->id << " : " << message;
	while (true) {
		try {
			handle_info(oss.str());
			if (word == "yes" || word == "Yes" || word == "y" || word == "Y") {
				return true;
			} else if (word == "no" || word == "No" || word == "not" || word == "Not" || word == "n" || word == "N") {
				return false;
			} else {
				cout << "\"" << word << "\" not recognized, answer with \"yes\" or \"no\"\n";
			}
		} catch (const exception& e) {
			cout << "ERROR: " << e.what() << endl;
		}
	}
}

#endif