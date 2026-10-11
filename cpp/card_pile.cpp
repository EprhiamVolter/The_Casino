#ifndef card_pile_cpp
#define card_pile_cpp

#include "card_pile.h"

#include <algorithm>
#include <sstream>

card_pile::card_pile(board * boardP) :
	my_board(boardP),
	pile(new vector<card*>)
		{
}

card_pile::card_pile(board * boardP, card * cardP, int ammount) :
	my_board(boardP),
	pile(new vector<card*>)
		{
	add(cardP, ammount);
}

bool card_pile::empty() {
	return pile->empty();
}

int card_pile::size() {
	return pile->size();
}

card * card_pile::pop() {
	card * c = nullptr;
	if (size() > 0) {
		c = pile->back();
		pile->pop_back();
	}
	return c;
}

card * card_pile::remove(int position) {
	card * c = pile->at(position);
	pile->erase(pile->begin() + position);
	return c;
}

card * card_pile::add(card * cardP, int ammount) {
	if (cardP != nullptr && ammount > 0) {
		for (int i = 1; i < ammount; i++) {
			card * copy = new card(cardP);
			pile->push_back(copy);
		}
		pile->push_back(cardP);
	}
	return cardP;
}
		
void card_pile::shuffle() {
    std::shuffle(pile->begin(), pile->end(), my_board->gen);
}

card * card_pile::at(int position) {
	return pile->at(position);
}

string card_pile::to_string() {
	ostringstream oss;
	if (!empty()) {
		int size1 = size() - 1;
		for (int i = 0; i < size1; i++) {
			oss << my_board->coll->cards->at(pile->at(i)->id)->name << ", ";
		}
		oss << my_board->coll->cards->at(pile->at(size1)->id)->name;
	}
	return oss.str();
}

#endif