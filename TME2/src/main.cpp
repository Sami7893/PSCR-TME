#include <iostream>
#include <fstream>
#include <regex>
#include <chrono>
#include <string>
#include <algorithm>
#include <vector>

// helper to clean a token (keep original comments near the logic)
static std::string cleanWord(const std::string& raw) {
	// une regex qui reconnait les caractères anormaux (négation des lettres)
	static const std::regex re( R"([^a-zA-Z])");
	// élimine la ponctuation et les caractères spéciaux
	std::string w = std::regex_replace(raw, re, "");
	// passe en lowercase
	std::transform(w.begin(), w.end(), w.begin(), ::tolower);
	return w;
}

int main(int argc, char** argv) {
	using namespace std;
	using namespace std::chrono;

	// Allow filename as optional first argument, default to project-root/WarAndPeace.txt
	// Optional second argument is mode (e.g. "count" or "unique" or "frequence").
	string filename = "../WarAndPeace.txt";
	string mode = "frequence";
	if (argc > 1) filename = argv[1];
	if (argc > 2) mode = argv[2];

	ifstream input(filename);
	if (!input.is_open()) {
		cerr << "Could not open '" << filename << "'. Please provide a readable text file as the first argument." << endl;
		cerr << "Usage: " << (argc>0?argv[0]:"TME2") << " [path/to/textfile]" << endl;
		return 2;
	}
	cout << "Parsing " << filename << " (mode=" << mode << ")" << endl;
	
	auto start = steady_clock::now();
	
	// prochain mot lu
	string word;

	if (mode == "count") {
		size_t nombre_lu = 0;
	
		// default counting mode: count total words
		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);
			/*
			// word est maintenant "tout propre"
			if (nombre_lu % 100 == 0)
				// on affiche un mot "propre" sur 100
				cout << nombre_lu << ": "<< word << endl;*/
			nombre_lu++;
		}
	
	input.close();
	cout << "Finished parsing." << endl;
	cout << "Found a total of " << nombre_lu << " words." << endl;

	} else if (mode == "unique") {
		// skeleton for unique mode
		// before the loop: declare a vector "seen"
		std::vector<string> seen;

		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);
			// add to seen if it is new
			bool b = false;
			for(auto& w : seen){
				if(w==word){
					b=true;
				}
			}
			if(!b)seen.push_back(word);
		}
	input.close();
	// TODO
	cout << "Found " << seen.size() << " unique words." << endl;

	} else if (mode == "frequence") {
		std::vector<std::pair<string,int>> freq;
		std::pair<string,int> mot;

		while (input >> word) {
			// élimine la ponctuation et les caractères spéciaux
			word = cleanWord(word);
			bool b = false;
			for(std::pair<string,int>& w : freq){
				if(w.first==word){
					b=true;
					++w.second;
					break;
				}
			}
			if(!b)freq.push_back({word, 1});
		}
	input.close();
	// TODO
	for(std::pair<string,int>& f : freq){
		if(f.first == "war" or f.first == "peace" or f.first == "toto")
		cout << f.first << " : " << f.second << endl;
	}

	std::sort(freq.begin(), freq.end(), [] (std::pair<string,int>& a, std::pair<string,int>& b) { return a.second < b.second ;});
	
	for(std::pair<string,int>& f : freq){
		cout << f.first << " : " << f.second << endl;
	}

	} else {
		// unknown mode: print usage and exit
		cerr << "Unknown mode '" << mode << "'. Supported modes: count, unique" << endl;
		input.close();
		return 1;
	}

	// print a single total runtime for successful runs
	auto end = steady_clock::now();
	cout << "Total runtime (wall clock) : " << duration_cast<milliseconds>(end - start).count() << " ms" << endl;

	return 0;
}


