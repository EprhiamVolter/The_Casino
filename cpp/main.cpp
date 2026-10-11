/*
TODO:
- card implementation
 - doom, confiscate and clairvoyance
 - check correct logging
- prize implementation
 - temporal_collider and unstable_concoction
 - check correct logging
- check card effects that assume more than 1 player (such as bad blood, expropiation, thief gloves)
*/

#include "collection.h"
#include "board.h"
#include "decider.h"

#include <iostream>
#include <string>

using namespace std;

void play();
void test_log();
void initialize();
void exit();
void print_help(string comm = string());

void play(vector<int> * player_types) {
	collection * c = new collection(true);
	board * b = new board(c, player_types);
	while (!b->complete) {
		b->players->at(b->turn)->decide->play();
	}
	cout << "\nGame ended\nLog:\n" << b->log->to_string_log(0,0,-1) << "\nFinal board:\n"<< b->to_string();
}

int main() {
	initialize();
	
	return 0;
	
	string cmd;
	string word;
	
	auto get_word = [&]() -> string {
		size_t end = cmd.find(' ');
		word = cmd.substr(0, end);
		if (end == string::npos) {
			cmd.clear();
		} else {
			cmd = cmd.substr(end + 1, string::npos);
		}
		return word;
	};
	
	int commCount = 0;
	while (true) {
		commCount++;
		cout << commCount << ">";
		
		getline(cin, cmd);
		get_word();
		
		if (word.empty()) {
			
		} else if (word == "help") {
			print_help(get_word());
		} else if (word == "exit") {
			exit();
			return 0;
		} else {
			cout << "'" << word << "' command not found. Type 'help' for a list of commands\n";
		}
	}
}

void test_log() { // temporary, manual
	collection * c = new collection(false);
	board * b = new board(c, new vector<int>());
	b->log->add("");
	b->log->add("0\n");
	b->log->add("1\n", true);
	b->log->add("2\n");
	b->log->add("3\n", true);
	b->log->add("4\n");
	b->log->add("5\n");
	b->log->step_up();
	b->log->add("6\n");
	b->log->step_up();
	b->log->add("7\n");
	cout << "Log:\n" << b->log->to_string_log();
}

void initialize() {
	play(new vector<int>({2}));
}

void exit() {
	
}

void print_help(string comm) {
	if (comm.empty()) {
		cout << "List of commands. Type 'help [command_name]' for more on the command '[command_name]':\n";
		cout << "help [comm]\nexit\n";
	} else if (comm == "help") {
		cout << "help: help [comm]\n\tif 'comm' is null, prints the list of commands\n\tif 'comm' is a command name, prints usage of 'comm'\n";
	} else if (comm == "exit") {
		cout << "exit: exit\nend the program execution\n";
	} else {
		cout << "'" << comm << "' command not found. Type 'help' for a list of commands\n";
	}
}