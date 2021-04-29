#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <random>
#include <time.h>
#include <climits>
using namespace std;

class TextGenerator {
	ifstream inputFile;
	string seed;
	map<string, map<char, float>> data;

public:

	void getSeed(int markovOrder, int numData = INT_MAX)
	{
		reset();
		map<string, int> chainfreqs; // the string, its frequency
		string chain = "";
		for (int i = 0; i < markovOrder; ++i)
			chain += inputFile.get(); // initialize to size markovOrder string at beginning of inputFile

		int c, i = 0;
		while ((c = inputFile.get()) != EOF && i++ < numData)
		{
			// making a spot if the chain doesn't exist, adding if the chain does exist
			if (!chainfreqs[chain])
				chainfreqs[chain] = 1;
			else
				chainfreqs[chain] += 1;

			chain = chain.substr(1) + (char)c; // chain size should stay the same
		}

		string ret;
		int mostAppearances = 0;
		for (auto& i : chainfreqs)
		{
			if (i.second > mostAppearances)
			{
				mostAppearances = i.second;
				ret = i.first;
			}
		}
		cout << "Seed: " << ret << endl;
		seed = ret;
	}

	void getData(int markovOrder, int numData = INT_MAX) // return seedString
	{
		reset();
		int numChars = 0;
		string currentChain = "";

		// initialize currentChain to beginning of text
		for (int i = 0; i < markovOrder; ++i)
			currentChain += inputFile.get();

		int c;
		// loop through input file until end of file or numData is reached
		while ((c = inputFile.get()) != EOF && numChars++ < numData)
		{
			// if the current chain does not exist in the data, make a place for it
			if (!data[currentChain][c])
				data[currentChain][c] = 1;

			// otherwise add 1 to the frequency of the char after said chain
			else
				data[currentChain][c] += 1;
			currentChain = currentChain.substr(1) + (char)c;
		}

		// percentize everything
		map<string, map<char, float>>::iterator it;
		for (it = data.begin(); it != data.end(); it++)
		{
			int totalAppearances = 0;
			for (auto& k : it->second)
			{
				totalAppearances += k.second;
			}
			for (auto& k : it->second)
			{
				k.second = k.second / totalAppearances * 100;
			}
		}
	}

	char getBlock(string chain)
	{
		float luckyNumber = rand() % 100;
		char nextChar = 'A';
		int nextCharLikeliness = INT_MAX;
		for (auto& i : data[chain])
		{
			if (abs(i.second - luckyNumber) < nextCharLikeliness) {
				nextCharLikeliness = abs(i.second - luckyNumber);
				nextChar = i.first;
			}
		}
		return nextChar;
	}

	void generateText(int markovOrder, string outputFileName, int numChars)
	{
		reset();
		int numData = INT_MAX;
		getSeed(markovOrder, numData);
		getData(markovOrder, numData);
		string currentChain = seed;
		char c;
		ofstream outFile(outputFileName);

		// start off with the seed
		outFile << seed;
		cout << seed;

		// starting after the seed, loop until numChars
		for (int i = seed.length(); i < numChars; ++i)
		{
			c = getBlock(currentChain);
			outFile << c;
			cout << c;
			currentChain = currentChain.substr(1) + c;
		}
	}

	void reset()
	{
		inputFile.clear();
		inputFile.seekg(0, ios::beg);
	}

	TextGenerator(string inputFileName)
	{
		inputFile.open(inputFileName);
	}
};

int main()
{
	int c;
	string filename;
	cout << "what file?\n";
	cin >> filename;
	cout << '\n';
	TextGenerator tg(filename);
	while (c != -1)
	{
		cout << "Your markov order: ";
		cin >> c;
		cout << '\n';
		srand(time(NULL));
		tg.generateText(c, "myOutputFile.txt", 1000);
		tg.reset();
		cout << '\n';
	}
	return 0;
}