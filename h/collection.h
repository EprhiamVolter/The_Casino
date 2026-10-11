#ifndef collection_h
#define collection_h

#include <string>
#include <map>
#include <set>
#include <vector>

using namespace std;

class card_data;

class collection {
	public:
		map<int, card_data*> * cards;
		map<string, int> * ids;
		set<string> * loaded;
		
		collection(bool load_allP = true);
		void load_card(card_data * card);
		void load_sets(set<string> sets);
		void load_all();
};

#include "card_data.h"

#endif