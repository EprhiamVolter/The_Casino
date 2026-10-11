#ifndef board_cpp
#define board_cpp

#include "board.h"

#include <chrono>
#include <algorithm>
#include <sstream>

board::board(collection * my_coll, vector<int> * player_types) :
	coll(my_coll),
	complete(false),
	gen(chrono::system_clock::now().time_since_epoch().count()),
	dice(uniform_int_distribution<int>(1, 6)),
	log(new logger(this)),
	round(0),
	turn(0),
	player_count(player_types->size()),
	players(new vector<player*>),
	draw_pile(new card_pile(this)),
	discard_pile(new card_pile(this)),
	prize_pile(new card_pile(this)),
	prize_discard(new card_pile(this)),
	ongoing(new map<int,int>())
		{
	log->add("root", true, 0);
	ostringstream oss;
	oss << "Game setup for " << player_count << " players";
	log->add(oss.str(), true, 1);
	shuffle(player_types->begin(),player_types->end(),gen);
	for (int i = 0; i < player_count; i++) { 
		player * p = new player(this, i, player_types->at(i));
		players->push_back(p);
	}
	
	oss = ostringstream();
	oss << "Sets: ";
	for (set<string>::iterator it = coll->loaded->begin(); it != coll->loaded->end(); it++) {
		oss << *it << " ";
	}
	log->add(oss.str(), true, 1);
	
	for (map<int, card_data*>::iterator it = coll->cards->begin(); it != coll->cards->end(); it++) {
		if (it->second->prize) {
			prize_pile->add(new card(it->second->id),it->second->ammount);
		} else {
			draw_pile->add(new card(it->second->id),it->second->ammount);
		}
	}
	draw_pile->shuffle();
	prize_pile->shuffle();
	oss = ostringstream(); 
	oss << "Draw pile: " << draw_pile->to_string();
	log->add(oss.str(), false, 10, new int_set(player_count));
	oss = ostringstream(); 
	oss << "Prize pile: " << prize_pile->to_string();
	log->add(oss.str(), false, 10, new int_set(player_count));
	
	for (int i = 0; i < player_count; i++) {
		draw(i, this->start_hand, true);
	}
	
	log->step_up();
	log->step_up();
	log->add("Round 0", true, 2);
	oss = ostringstream();
	oss << "Player 0, " << (players->at(0)->decide->human_player ? "Human" : "AI") << ", Position 0";
	log->add(oss.str(), true, 3);
	players->at(0)->start_turn();
}

void board::exit(int by) {
	ostringstream oss;
	oss << "Game abandoned by " << by;
	log->add(oss.str(), false, 1);
	complete = true;
	winner = -1;
}

void board::win(int by) {
	ostringstream oss;
	oss << "Game won by " << by;
	log->add(oss.str(), false, 1);
	complete = true;
	winner = by;
}

void board::draw(int player_id, int ammount, bool identify) {
	if (ongoing->count(coll->ids->at("vacation")) != 0) {
		ostringstream ossfail;
		if (identify) {
			ossfail << "Player " << player_id << "fails to draw ";
		} else {
			ossfail << "Fails to draw ";
		}
		ossfail << ammount << (ammount == 1 ? " card" : " cards") << " becouse player " << ongoing->at(coll->ids->at("vacation")) << " has vacation ongoing";
		log->add(ossfail.str(), false, 7);
	} else {
		int last_drawn = 0;
		ostringstream oss;
		if (identify) {
			oss << "Player " << player_id << " draws ";
		} else {
			oss << "Draws ";
		}
		string intro = oss.str();
		for (int i = 0; i < ammount; i++) {
			if (draw_pile->empty()) {
				if (last_drawn > 0) {
					ostringstream ossalt;
					ossalt << last_drawn << (last_drawn == 1 ? " card" : " cards");
					log->add(oss.str(), false, 7, new int_set(player_count, {player_id}), ossalt.str());
					last_drawn = 0;
					oss = ostringstream();
					oss << intro;
				}
				if (!discard_pile->empty()) {
					vector<card*> * swap = draw_pile->pile;
					draw_pile->pile = discard_pile->pile;
					discard_pile->pile = swap;
					draw_pile->shuffle();
					log->add("Discard pile is shuffled into the draw pile", true, 10);
					ostringstream ossalt; 
					ossalt << "New draw pile: " << draw_pile->to_string();
					log->add(ossalt.str(), false, 10, new int_set(player_count));
					log->step_up();
					card * card_drawn = draw_pile->pop();
					players->at(player_id)->hand->add(card_drawn);
					oss << (last_drawn > 0 ? ", " : "") << coll->cards->at(card_drawn->id)->name;
					last_drawn++;
				} else {
					ostringstream ossfail;
					if (identify) {
						ossfail << "Player " << player_id << " fails to draw ";
					} else {
						ossfail <<"Fails to draw ";
					}
					ossfail << ammount - i << (ammount - i == 1 ? " card" : " cards") << " becouse draw pile is empty";
					log->add(ossfail.str(), false, 7);
					break;
				}
			} else {
				card * card_drawn = draw_pile->pop();
				players->at(player_id)->hand->add(card_drawn);
				oss << (last_drawn > 0 ? ", " : "") << coll->cards->at(card_drawn->id)->name;
				last_drawn++;
			}
		}
		if (last_drawn > 0) {
			ostringstream ossalt;
			ossalt << intro << last_drawn << (last_drawn == 1 ? " card" : " cards");
			log->add(oss.str(), false, 7, new int_set(player_count, {player_id}), ossalt.str());
		}
	}
}

int board::draw_prize(int player_id, bool identify) {
	if (ongoing->count(coll->ids->at("sold_out")) != 0) {
		ostringstream ossfail;
		if (identify) {
			ossfail << "Player " << player_id << " fails ";
		} else {
			ossfail << "Fails ";
		}
		ossfail << " to draw a prize becouse " << ongoing->at(coll->ids->at("sold_out")) << " has sold out ongoing";
		log->add(ossfail.str(), false, 7);
		return -1;
	} else {
		vector<card*> prizes_drawn;
		for (int i = 0; i < players->at(player_id)->prize_options; i++) {
			if (prize_pile->empty()) {
				if (prize_discard->empty()) {
					ostringstream oss;
					if (identify) {
						oss << "Player " << player_id << " fails to draw a prize becouse prize pile is empty";
					} else {
						oss << "Fails to draw a prize becouse prize pile is empty";
					}
					log->add(oss.str(), false, 6);
					return -1;
				} else {
					vector<card*> * swap = prize_pile->pile;
					prize_pile->pile = prize_discard->pile;
					prize_discard->pile = swap;
					prize_pile->shuffle();
					log->add("Prize discard pile is shuffled into the prize pile", true, 10);
					ostringstream oss; 
					oss << "New prize pile: " << prize_pile->to_string();
					log->add(oss.str(), false, 10, new int_set(player_count));
					log->step_up();
				}
			}
			prizes_drawn.push_back(prize_pile->pop());
		}
		if (prizes_drawn.size() > 0) {
			int prize_choosen;
			if (prizes_drawn.size() > 1) {
				vector<int> choosen = players->at(player_id)->decide->choose_cards(&prizes_drawn,{},1,1,false,"Free will: choose a prize", 15);
				prize_choosen = choosen.at(0);
				ostringstream oss;
				if (identify) {
					oss << "Player " << player_id << " discards ";
				} else {
					oss << "Discards ";
				}
				bool discarded = false;
				for (int i = 0; i < prizes_drawn.size(); i++) {
					if (i != prize_choosen) {
						prize_discard->add(prizes_drawn.at(i));
						oss << (discarded ? ", " : "") << coll->cards->at(prizes_drawn.at(i)->id)->name;
						discarded = true;
					}
				}
				log->add(oss.str(), false, 6);
			} else {
				prize_choosen = 0;
			}
			card * prize_obtained = prizes_drawn.at(prize_choosen);
			ostringstream oss;
			if (identify) {
				oss << "Player " << player_id << " obtains ";
			} else {
				oss << "Obtains ";
			}
			oss << coll->cards->at(prize_obtained->id)->name << " prize";
			log->add(oss.str(), true, 6);
			coll->cards->at(prize_obtained->id)->play(players->at(player_id));
			coll->cards->at(prize_obtained->id)->handle(players->at(player_id), prize_obtained);
			log->step_up();
			return prize_obtained->id;
		}
		return -1;
	}
}

void board::end_turn() {
	log->step_up();
	if (turn == player_count - 1) {
		log->step_up();
		round++;
		if (round >= 1000) {
			log->add("Round timeout", false, 1);
			exit(-1);
		}
		ostringstream oss;
		oss << "Round " << round;
		log->add(oss.str(), true, 2);
		turn = 0;
		oss = ostringstream();
		oss << "Player 0, " << (players->at(turn)->decide->human_player ? "Human" : "AI") << ", Position " << players->at(turn)->position;
		log->add(oss.str(), true, 3);
	} else {
		turn++;
		ostringstream oss;
		oss << "Player " << turn << (players->at(turn)->decide->human_player ? ", Human" : ", AI") << ", Position " << players->at(turn)->position;
		log->add(oss.str(), true, 3);
	}
	players->at(turn)->start_turn();
}

string board::to_string(int by) {
	ostringstream oss;
	oss << (complete ? (winner == -1 ? "Abandoned" : std::to_string(winner) + " Won" ) : "Ongoing") << ", Players " << player_count << ", Round " << round << ", Turn " << turn;
	if (by == -1) {
		oss << "\nDraw: " << draw_pile->to_string() << "\nPrize: " << prize_pile->to_string() << "\n";
	} else {
		oss << ", Draw: " << draw_pile->size() << " cards, Prize: " << prize_pile->size() << " cards\n";
	}
	oss << "Discard: " << discard_pile->to_string() << "\nPrize discard: " << prize_discard->to_string() << "\n";
	for (int i = 0; i < player_count; i++) {
		oss << players->at(i)->to_string(by == -1 || by == i);
	}
	oss << "\n";
	return oss.str();
}

#endif 