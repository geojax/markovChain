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
	map<string, map<string, float>> data;

	string getChars(int numChars)
	{
		string ret = "";
		for (int i = 0; i < numChars; ++i)
		{
			char c = inputFile.get();
			if (c == EOF) return "";
			ret += c;
			c = inputFile.get();
			if (c == EOF) return ret;
			ret += c;
			return ret;
		}
	}

	void getSeed(int markovOrder, int numData, int blockSize)
	{
		reset();
		map<string, int> chainfreqs; // the string, its frequency
		string chain = "";
		for (int i = 0; i < markovOrder * blockSize; ++i)
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

	void getData(int markovOrder, int numData, int blockSize) // return seedString
	{
		reset();
		int numChars = 0;
		string currentChain = "";

		string str = "";

		// initialize currentChain to blocks at beginning of text
		for (int i = 0; i < markovOrder * blockSize; ++i)
			currentChain += inputFile.get();

		// initialize str to proper size after currentChain
		for (int i = 0; i < blockSize; ++i)
			str += inputFile.get();

		// loop through input file until end of file or numData is reached
		while (str != "" && numChars++ < numData)
		{
			// if the current chain does not exist with a str frequency in the data, make a place for it
			if (!data[currentChain].count(str))
				data[currentChain][str] = 1;

			// otherwise add 1 to the frequency of the char after said chain
			else
				data[currentChain][str] += 1;

			// currentChain adds the first char of str and loses its first char
			// ADDED BACK IN --> NOT -- I'm getting rid of this --> despite blocksize we still move through only char by char to get every
			// combination of 2 chars (maybe unnecessary? move through by blocks instead of chars?)
			currentChain = currentChain.substr(1) + str[0];

			// str maintains a size equal to blockSize
			//str = "";
			//for (int i = 0; i < blockSize; ++i)
			str = str.substr(1) + (char)inputFile.get();
		}

		// percentize everything
		for (auto& i : data)
		{
			int totalAppearances = 0;
			for (auto& k : i.second)
			{
				totalAppearances += k.second;
			}
			for (auto& k : i.second)
			{
				k.second = k.second / totalAppearances * 100;
			}
		}
	}

	string getBlock(string chain)
	{
		float luckyNumber = rand() % 100;
		string nextBlock = "AB";
		int nextBlockLikeliness = INT_MAX;
		for (auto& i : data[chain])
		{
			if (abs(i.second - luckyNumber) < nextBlockLikeliness) {
				nextBlockLikeliness = abs(i.second - luckyNumber);
				nextBlock = i.first;
			}
		}
		return nextBlock;
	}

public:
	void generateText(int markovOrder, string outputFileName, int numChars, int blockSize)
	{
		reset();
		int numData = 10000;
		getSeed(markovOrder, numData, blockSize);
		getData(markovOrder, numData, blockSize);
		string currentChain = seed;
		string nextBlock;
		ofstream outFile(outputFileName);

		// start off with the seed
		outFile << seed;
		cout << seed;

		// starting after the seed, loop until numChars
		for (int i = seed.length(); i < numChars; ++i)
		{
			nextBlock = getBlock(currentChain);
			outFile << nextBlock;
			cout << nextBlock;
			currentChain = currentChain.substr(blockSize) + nextBlock;
		}
		data.clear();
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

	~TextGenerator()
	{
		inputFile.close();
		data.clear();
	}
};

int main()
{
	srand(time(NULL));

	int c = 0;
	int end = 0;
	int blockSize;
	string filename;
	cout << "what file?\n";
	cin >> filename;
	cout << '\n';
	TextGenerator tg(filename);

	while (end != 1)
	{
		cout << "Your markov order: ";
		cin >> c;
		cout << "\nYour block size: ";
		cin >> blockSize;
		cout << '\n';

		tg.generateText(c, "myOutputFile.txt", 1000, blockSize);
		tg.reset();
		cout << '\n';
		cout << "1 to end: ";
		cin >> end;
		cout << '\n';
	}

	tg.~TextGenerator();
	return 0;
}