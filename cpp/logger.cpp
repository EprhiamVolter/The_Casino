#ifndef logger_cpp
#define logger_cpp

#include "logger.h"

logger::logger(board * my_boardP) :
	my_board(my_boardP),
	next_id(0),
	log_origin(nullptr),
	log_pointer(nullptr)
		{
	
}

void logger::add(string report, bool go_to_new, int detail, int_set * visibility, string alt_report) {
	log_node * next = new log_node;
	next->id = next_id;
	next_id++;
	next->report = report;
	next->round = my_board->round;
	next->turn = my_board->turn;
	next->detail = detail;
	next->visibility = visibility;
	next->alternative = !alt_report.empty();
	next->alt_report = alt_report;
	if (log_pointer == nullptr) {
		log_origin = log_pointer = next;
		next->pa = next->pb = next->nb = next->fc = next->lc = nullptr;
	} else {
		next->pa = log_pointer;
		next->pb = log_pointer->lc;
		next->nb = next->fc = next->lc = nullptr;
		log_pointer->lc = next;
		if (next->pb == nullptr) {
			log_pointer->fc = next;
		} else {
			next->pb->nb = next;
		}
	}
	if (go_to_new) {
		log_pointer = next;
	}
}

void logger::step_up() {
	log_pointer = log_pointer->pa;
}

log_node * logger::next_log(log_node * it) {
	if (it == nullptr) {
		return nullptr;
	} else {
		log_node * nextc = next_log(it->fc);
		if (nextc != nullptr) {
			return nextc;
		} else {
			return next_log(it->nb);
		}
	}
}

string logger::to_string_log(int since_round, int since_turn, int detail, int by) {
	log_node * it = log_origin;
	
	while (it != nullptr && it->round < since_round) {
		it = next_log(it);
	}
	while (it != nullptr && it->turn < since_turn) {
		it = next_log(it);
	}
	return to_string_log_rec(it == log_origin ? it->fc : it, 0, detail == -1 ? max_detail : detail, by);
}

string logger::to_string_log_rec(log_node * origin, int depth, int detail, int by) {
	string ret;
	log_node * it = origin;
	
	if (by == -1) {
		while (it != nullptr) {
			if (it->detail <= detail) {
				ret += string(depth,'\t') + it->report + "\n";
				ret += to_string_log_rec(it->fc, depth + 1, detail, by);
			} else {
				ret += to_string_log_rec(it->fc, depth, detail, by);
			}
			it = it->nb;
		}
	} else {
		while (it != nullptr) {
			if (it->detail <= detail) {
				if (it->visibility == nullptr || it->visibility->elem(by)) {
					ret += string(depth,'\t') + it->report + "\n";
				} else if (it->alternative) {
					ret += string(depth,'\t') + it->alt_report + "\n";
				}
				ret += to_string_log_rec(it->fc, depth + 1, detail, by);
			} else {
				ret += to_string_log_rec(it->fc, depth, detail, by);
			}
			it = it->nb;
		}
	}
	return ret;
}

#endif