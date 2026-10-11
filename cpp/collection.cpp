#ifndef collection_cpp
#define collection_cpp

#include "collection.h"

#include <algorithm>

// Does nothing
void nothing(player * playing) {
	
}

// Discards this card
void discard(player * playing, card * this_card) {
	playing->my_board->discard_pile->add(this_card);
}

// Sets ongoing effect
void ongoing(player * playing, card * this_card) {
	playing->ongoing->add(this_card);
	playing->my_board->ongoing->emplace(this_card->id, playing->id);
}

// Discard this prize
void prize_discard(player * playing, card * this_card) {
	playing->my_board->prize_discard->add(this_card);
}

// If this is the last card in your hand, move forward 7 spaces
void ace_up_your_sleeve_play(player * playing) {
	if (playing->hand->empty()) {
		playing->move(7);
	}
}

// Drag a player to your position, then move back 6 cells
void bad_blood_play(player * playing) {
	int who = playing->decide->choose_player({-1}, "Bad blood: Drag a player to your position, then move back 6 cells", 1);
	playing->my_board->players->at(who)->teleport(playing->position);
	playing->move(-6);
}

// If you played this card before starting your spin, you get double the prizes on settle
void bet_play(player * playing) {
	if (playing->spin == before) {
		playing->settle_prizes *= 2;
	}
}

// If you played 3 other cards this turn, draw 2 cards
void broadway_secretary_play(player * playing) {
	if (playing->played_this_turn >= 3) {
		playing->draw(2);
	}		
}

// All players win a prize
void casino_heist_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		playing->my_board->draw_prize(i);
	}
}

// Increase your maximum spin by 2 this turn (the prize is still 10 or more)
void cheat_game_play(player * playing) {
	playing->temp_spin_max += 2;
}

// All players secretly show you their hands
void clairvoyance_play(player * playing) {
	
}

// Move forward 3 cells for every other card you've played this turn
void combo_play(player * playing) {
	playing->move(3 * playing->played_this_turn);
}

// Cancels the effect of another card
void confiscate_play(player * playing) {
	
}

// If another player rolls exactly 7, their turn ends
void doom_play(player * playing) {
	
}

// All other players roll a die and move back the number they rolled
void earthquake_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		if (i != playing->id) {
			playing->my_board->players->at(i)->move(- playing->my_board->dice(playing->my_board->gen));
		}
	}
}
 
// All players with more cards than you before playing this card must discard 2 cards
void envy_play(player * playing) {
	int min_cards = playing->hand->size() + 2;
	for (int i = 0; i < playing->my_board->player_count; i++) {
		player * p = playing->my_board->players->at(i);
		if (i != playing->id && p->hand->size() >= min_cards) {
			vector<int> dis_pos = p->decide->choose_cards(p->hand->pile,{},2,2,false,"Envy: Discard 2 cards", 11);
			p->discard(dis_pos);
		}
	}
}

// If you missed a spin this turn, move forward 6 spaces
void error_play(player * playing) {
	if (playing->spin == missed) {
		playing->move(6);
	}
}

// TODO logging
// A player with no cards moves back 10 spaces
void expropriation_play(player * playing) {
	vector<int> pos;
	for (int i = 0; i < playing->my_board->player_count; i++) {
		if (i != playing->id && playing->my_board->players->at(i)->hand->empty()) {
			pos.push_back(i);
		}
	}
	if (!pos.empty()) {
		int who = playing->decide->choose_player(pos,"Expropiation: A player with no cards moves back 10 spaces", 13);
		playing->my_board->players->at(who)->move(-10);
	}
}

// Move forward 4 cells
void free_pass_play(player * playing) {
	playing->move(4);
}

// This turn, before claiming a prize, choose from the first 3 options
void free_will_play(player * playing) {
	playing->prize_options = 3;
}

// TODO logging
// Take the top card from the discard pile
void graveyard_keeper_play(player * playing) {
	card * corpse = playing->my_board->discard_pile->pop();
	if (corpse != nullptr) {
		playing->hand->add(corpse);
	}
}

// If you have 6 other cards, win a prize
void happy_birthday_play(player * playing) {
	if (playing->hand->size() >= 6) {
		playing->my_board->draw_prize(playing->id);
	}
}

// All players on even numbered spaces move forward 8 cells
void ladders_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		player * p = playing->my_board->players->at(i);
		if (p->position % 2 == 0) {
			p->move(8);
		}
	}
}

// Add 2 to your spin
void plug_play(player * playing) {
	playing->roll(2);
}

// Draw a card
void recycle_play(player * playing) {
	playing->draw(1);
}

// Reduce the result of your spin by 1 (which may cancel out missing)
void rewind_play(player * playing) {
	playing->roll_sum--;
	if (playing->spin == missed && playing->roll_sum <= playing->temp_spin_max) {
		playing->spin = started;
	}
}

// Discard 2 other cards to receive a prize
void sacrifice_to_cthulhu_play(player * playing) {
	if (playing->hand->size() >= 2) {
		vector<int> disc = playing->decide->choose_cards(playing->hand->pile,{},2,2,false,
			"Sacrifice_To_Cthulhu: Discard 2 other cards to receive a prize", 23);
		playing->discard(disc);
		playing->my_board->draw_prize(playing->id);
	}
}

// Roll the dice; move forward that many spaces
void slot_machine_play(player * playing) {
	playing->move(playing->my_board->dice(playing->my_board->gen));
}


// All players on odd numbered cells move back 7 cells
void snakes_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		player * p = playing->my_board->players->at(i);
		if (p->position % 2 == 1) {
			p->move(-7);
		}
	}
}

// TODO: needs logging
// Give a card to another player; they must give you 2 cards (it can be the one you gave them)
void trade_play(player * playing) {
	if (!playing->hand->empty()) {
		vector<int> give_pos = playing->decide->choose_cards(playing->hand->pile,{},1,1,false,
			"Trade: Give a card to another player; they must give you 2 cards (it can be the one you gave them)", 28);
		card * give_card = playing->hand->remove(give_pos.at(0));
		int give_to = playing->decide->choose_player({-1}, "Trade: Choose player to give a card; they must give you 2 cards (it can be the one you gave them)", 28);
		player * p2 = playing->my_board->players->at(give_to);
		p2->hand->add(give_card);
		vector<int> give2_pos = p2->decide->choose_cards(p2->hand->pile,{},2,2,false,
			"Trade: Give 2 cards from your hand to player" + to_string(playing->id), 28);
		sort(give2_pos.begin(), give2_pos.end());
		playing->hand->add(p2->hand->remove(give2_pos.at(1)));
		playing->hand->add(p2->hand->remove(give2_pos.at(0)));
	}
}

// If you played another yellow card this turn, one player moves back 29 cells
void yellow_card_play(player * playing) {
	if (playing->yellow_played) {
		int who = playing->decide->choose_player({-1},"Yellow card: One player moves back 29 cells", 30);
		playing->my_board->players->at(who)->move(-29);
	}
	playing->yellow_played = true;
}

// The other players discard 3 cards
void bankruptcy_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		if (i != playing->id) {
			player * p = playing->my_board->players->at(i);
			vector<int> dis_pos = p->decide->choose_cards(p->hand->pile,{},3,3,false,"Bankruptcy: Discard 3 cards", 32);
			p->discard(dis_pos);
		}
	}
}

// The other players draw 2 cards; you draw 5 cards
void feast_play(player * playing) {
	playing->my_board->draw(playing->id, 5, true);
	for (int i = 0; i < playing->my_board->player_count; i++) {
		if (i != playing->id) {
			playing->my_board->draw(i, 2, true);
		}
	}
}

// The other players move back 5 cells for each card in their hand
void greed_play(player * playing) {
	for (int i = 0; i < playing->my_board->player_count; i++) {
		if (i != playing->id) {
			playing->my_board->players->at(i)->move(-5 * playing->my_board->players->at(i)->hand->size());
		}
	}
}

// Teleport to another player's position or draw 2 cards
void migos_box_play(player * playing) {
	if (playing->decide->may("Mi-go's box: May teleport to another player's position, otherwise draw 2 cards", 35)) {
		int who = playing->decide->choose_player({-1}, "Mi-go's box: Teleport to another player's position", 35);
		playing->teleport(playing->my_board->players->at(who)->position);
	} else {
		playing->my_board->draw(playing->id, 2);
	}
}

// A player moves back 29 cells
void red_card_play(player * playing) {
	int who = playing->decide->choose_player({-1}, "Red card: A player moves back 29 cells", 36);
	playing->my_board->players->at(who)->move(-29);
}

// Play another turn
void temporal_collider_play(player * playing) {
	
}

// Steal 2 random cards from another player
void thiefs_gloves_play(player * playing) {
	int who = playing->decide->choose_player({-1}, "Thief's gloves: Steal 2 random cards from another player", 38);
	int hand_size = playing->my_board->players->at(who)->hand->size();
	if (hand_size >= 2) {
		vector<int> pos;
		for (int i = 0; i < hand_size; i++) {
			pos.push_back(i);
		}
		shuffle(pos.begin(), pos.end(), playing->my_board->gen);
		playing->my_board->players->at(who)->discard({pos.at(0), pos.at(1)});
	} else if (hand_size == 1) {
		playing->my_board->players->at(who)->discard({0});
	}
}

// Draw 3 cards
void three_wishes_play(player * playing) {
	playing->my_board->draw(playing->id, 3);
}

// Immediately play the top 4 cards of the deck in that order
void unstable_concoction_play(player * playing) {
	
}

// Teleport a player to cell 57
void vip_pass_play(player * playing) {
	int who = playing->decide->choose_player({}, "Red card: Teleport a player to cell 57", 41);
	playing->my_board->players->at(who)->teleport(57);
}

// Move forward 13 cells
void zero_km_car_play(player * playing) {
	playing->move(13);
}



void collection::load_sets(set<string> sets) {
	if (sets.count("original") == 1 && loaded->count("original") == 0) {
		loaded->insert("original");
		load_card(new card_data( 0,1,false,"ace_up_your_sleeve","original",ace_up_your_sleeve_play,discard,"If this is the last card in your hand, move forward 7 spaces"));
		load_card(new card_data( 1,1,false,"bad_blood","original",bad_blood_play,discard,"Drag a player to your position, then move back 6 cells"));
		load_card(new card_data( 2,1,false,"bet","original",bet_play,discard,"If you played this card before starting your spin, you get double the prizes on settle"));
		load_card(new card_data( 3,1,false,"broadway_secretary","original",broadway_secretary_play,discard,"If you played 3 other cards this turn, draw 2 cards"));
		load_card(new card_data( 4,1,false,"casino_heist","original",casino_heist_play,discard,"All players win a prize"));
		load_card(new card_data( 5,1,false,"cheat_game","original",cheat_game_play,discard,"Increase your maximum spin by 2 this turn (the prize is still 10 or more)"));
		load_card(new card_data( 6,0,false,"clairvoyance","original",clairvoyance_play,discard,"All players secretly show you their hands"));
		load_card(new card_data( 7,1,false,"combo","original",combo_play,discard,"Move forward 3 cells for every other card you've played this turn"));
		load_card(new card_data( 8,0,false,"confiscate","original",confiscate_play,discard,"Cancels the effect of another card"));
		load_card(new card_data( 9,0,false,"doom","original",doom_play,discard,"If another player rolls exactly 7, their turn ends"));
		load_card(new card_data(10,1,false,"earthquake","original",earthquake_play,discard,"All other players roll a die and move back the number they rolled"));
		load_card(new card_data(11,1,false,"envy","original",envy_play,discard,"All players with more cards than you before playing this card must discard 2 cards"));
		load_card(new card_data(12,1,false,"error","original",error_play,discard,"If you missed a spin this turn, move forward 6 spaces"));
		load_card(new card_data(13,1,false,"expropriation","original",expropriation_play,discard,"A player with no cards moves back 10 spaces"));
		load_card(new card_data(14,1,false,"free_pass","original",free_pass_play,discard,"Move forward 4 cells"));
		load_card(new card_data(15,1,false,"free_will","original",free_will_play,discard,"This turn, before claiming a prize, choose from the first 3 options"));
		load_card(new card_data(16,1,false,"graveyard_keeper","original",graveyard_keeper_play,discard,"Take the top card from the discard pile"));
		load_card(new card_data(17,1,false,"happy_birthday","original",happy_birthday_play,discard,"If you have 6 other cards, win a prize"));
		load_card(new card_data(18,1,false,"ladders","original",ladders_play,discard,"All players on odd numbered cells move back 7 cells"));
		load_card(new card_data(19,1,false,"plug","original",plug_play,discard,"Add 2 to your spin"));
		load_card(new card_data(20,1,false,"portable_ice_wall","original",nothing,ongoing,"Until your next turn, any player who would catch up to you remains one space behind you"));
		load_card(new card_data(21,1,false,"recycle","original",recycle_play,discard,"Draw a card"));
		load_card(new card_data(22,1,false,"rewind","original",rewind_play,discard,"Reduce the result of your spin by 1 (which may cancel out missing)"));
		load_card(new card_data(23,1,false,"sacrifice_to_cthulhu","original",sacrifice_to_cthulhu_play,discard,"Discard 2 other cards to receive a prize"));
		load_card(new card_data(24,1,false,"slot_machine","original",slot_machine_play,discard,"Roll the dice; move forward that many spaces"));
		load_card(new card_data(25,1,false,"snakes","original",snakes_play,discard,"All players on odd numbered cells move back 7 cells"));
		load_card(new card_data(26,1,false,"sold_out","original",nothing,ongoing,"No one can win prizes until your next turn"));
		load_card(new card_data(27,1,false,"speed_limit","original",nothing,ongoing,"Until your next turn, players cannot move more that 5 spaces at a time"));
		load_card(new card_data(28,1,false,"trade","original",trade_play,discard,"Give a card to another player; they must give you 2 cards (it can be the one you gave them)"));
		load_card(new card_data(29,1,false,"vacation","original",nothing,ongoing,"No player may draw cards until your next turn"));
		load_card(new card_data(30,2,false,"yellow_card","original",yellow_card_play,discard,"If you played another yellow card this turn, one player moves back 29 cells"));
		load_card(new card_data(31,1,false,"yield_sign","original",nothing,ongoing,"No player may play cards until your next turn"));
		
		load_card(new card_data(32,1,true,"bankruptcy","original",bankruptcy_play,prize_discard,"The other players discard 3 cards"));
		load_card(new card_data(33,1,true,"feast","original",feast_play,prize_discard,"The other players draw 2 cards; you draw 5 cards"));
		load_card(new card_data(34,1,true,"greed","original",greed_play,prize_discard,"The other players move back 5 cells for each card in their hand"));
		load_card(new card_data(35,1,true,"migos box","original",migos_box_play,prize_discard,"Teleport to another player's position or draw 2 cards"));
		load_card(new card_data(36,1,true,"red_card","original",red_card_play,prize_discard,"A player moves back 29 cells"));
		load_card(new card_data(37,0,true,"temporal_collider","original",temporal_collider_play,prize_discard,"Play another turn"));
		load_card(new card_data(38,1,true,"thiefs_gloves","original",thiefs_gloves_play,prize_discard,"Steal 2 random cards from another player"));
		load_card(new card_data(39,1,true,"three_wishes","original",three_wishes_play,prize_discard,"Draw 3 cards"));
		load_card(new card_data(40,0,true,"unstable_concoction","original",unstable_concoction_play,prize_discard,"Immediately play the top 4 cards of the deck in that order"));
		load_card(new card_data(41,1,true,"vip_pass","original",vip_pass_play,prize_discard,"Teleport a player to cell 57"));
		load_card(new card_data(42,1,true,"zero_km_car","original",zero_km_car_play,prize_discard,"Move forward 13 cells"));
	}
}

collection::collection(bool load_allP) :
	cards(new map<int, card_data*>),
	ids(new map<string, int>),
	loaded(new set<string>)
		{
	if (load_allP) {
		load_all();
	}
}

void collection::load_card(card_data * card) {
	cards->emplace(card->id,card);
	ids->emplace(card->name,card->id);
}

void collection::load_all() {
	load_sets({"original"});
}

#endif