
/***********************************************************************
 * Vagner Machado QCID 23651127
 * Project 3
 * Professor Tsaiyun Phillips
 * Queens College - Spring 2019
 ***********************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

/**
 * listNode class declaration
 */
class listNode
{
	friend class RadixSort;
	friend class LLQ;
	friend class LLStack;
private:
	string data;
	listNode* next;
public:
	int getData();
	listNode();
	listNode(string z);
};

/**
 * LLStack class declaration
 */
class LLStack
{
	friend class RadixSort;
private:
	listNode * top;
public:
	LLStack();
	void push(listNode newNode);
	listNode pop(ofstream &out);
	bool isEmpty();
	string printStack(ofstream &out);
};

/**
 * LLQ class declaration
 */
class LLQ
{
	friend class RadixSort;
private:
	listNode * head;
	listNode * tail;
public:
	LLQ();
	void addTail(listNode x);
	listNode deleteHead(ofstream &out);
	bool isEmpty();
	string printQueue(int table, int index, ofstream &debug);
};

class RadixSort
{
private:
	int tableSize;
	LLQ ** hashTable;
	string data;
	int currentTable;
	int previousTable;
	int longestStringLength;
	int currentPosition; //in the string
	int currentQueue;   // 0 to 255

public:
	int firstReading(ifstream &inFile);
	void loadStack (ifstream &inFile, LLStack * top);
	void dumpStack(LLStack * top, ofstream &out, int curDigit, int curTable);
	int tableIndex();
	int getMaxDigits(int digit);
	int getDigit(listNode node, int &curDigit);
	int hashIndex(char x);
	char getChar(listNode node, int position);
	RadixSort();
	string padString(string data);
	void radixSort(LLStack * stack, ofstream &sort, ofstream &debug);
	void printTable(int table, ofstream &debug);
	void output(ofstream &sort, ofstream &debug);
};


/**************************************************************
 * main method - Enables the user to use a stack or a queue
 * to insert and delete integers. Input is provided by the user
 * using the console keyboard
 *************************************************************/
int main(int argc, char *argv[])
{
	ifstream infile;
	ofstream outfile1;
	ofstream outfile2;

	infile.open(argv[1]);
	outfile1.open(argv[2]);
	outfile2.open(argv[3]);

	outfile2 << "\n\n*** Vagner's Project 3 Debugging***\n\n";
	cout << "\n\n*** Welcome to Vagner's Project 3 ***\n";
	RadixSort radix;
	radix.firstReading(infile);
	infile.close();
	infile.open(argv[1]);
	LLStack * stack = new LLStack();
	radix.loadStack(infile, stack);
	string stackData = stack->printStack(outfile2);
	cout << stackData;
	radix.radixSort(stack, outfile1, outfile2);
	cout << "\n** Sorting finished and inserted to " << argv[2] << " ***\n";
	cout << "\n** Debugging information inserted to " << argv[3] << " ***\n";
	infile.close();
	outfile1.close();
	outfile2.close();
}

/*****************************************************************
 * listNode Methods
 *****************************************************************/
listNode::listNode(string x)
{
	data = x;
	next = NULL;
}

listNode::listNode()
{
	data = "";
	next = NULL;
}


/*****************************************************************
 * LLStack Methods
 *****************************************************************/
LLStack::LLStack()
{
	top = NULL;
}

void LLStack::push(listNode newNode)
{
	listNode * temp = new listNode(newNode.data);
	if(top != NULL)
	{
		temp->next = top;
		top = temp;
	}
	else
		top = temp;
}

listNode LLStack::pop(ofstream &debug)
{
	if (isEmpty())
	{
		debug << "\n***Cannot POP, Stack is empty***\n";
		return listNode("\n\n***POP ERROR***\n\n");
	}
	listNode a(top->data);
	top = top->next;
	return a;

}

bool LLStack::isEmpty()
{
	return top == NULL;
}

string LLStack::printStack(ofstream &debug)
{
	stringstream res;
	listNode * x = top;
	res << "\n\n*** Below is the output of the Stack ***\n\n";
	res << "Top -> ";
	while (x != NULL)
	{
		res << "("  << x->data << ", ";
		x = x-> next;
		if (x == NULL)
			res << "NULL) -> ";
		else
		{
			res << x->data << ") -> ";
			x = x->next;
		}
	}
	res << " NULL\n";
	debug << res.str();
	return res.str();
}



/*****************************************************************
 * LLQ Methods
 *****************************************************************/
LLQ::LLQ()
{
	listNode * dum = new listNode("9999");
	head = dum;
	tail = dum;
}

void LLQ::addTail(listNode x)
{
	listNode * temp = new listNode(x.data);
	tail->next = temp;
	tail = temp;
}

listNode LLQ::deleteHead(ofstream &debug)
{
	if (isEmpty())
	{
		debug << "\n***Cannot DEQUEUE, Queue is empty***\n";
		return listNode("\n\n***DEQUEUE ERROR***\n\n");
	}
	else
	{
		listNode a (head->next->data);
		if(head->next->next != NULL)
			head->next = head->next->next;
		else
		{
			head->next = NULL;
			tail = head;
		}
		return a;
	}
}

bool LLQ::isEmpty()
{
	return head == tail;
}

string LLQ::printQueue(int table, int index, ofstream &debug)
{
	stringstream res;
	listNode * x = head;
	res << "Table [" << table << "][" << index << "]: ";
	while (x != NULL)
	{
		res << "(" + x->data + ", ";
		x = x-> next;
		if (x == NULL)
			res << "NULL) -> ";
		else
		{
			res << x->data + ") -> ";
			x = x->next;
		}
	}
	res << " NULL\n";
	debug << res.str();
	return res.str();
}

/*****************************************************************
 * RadixSort Methods
 *****************************************************************/

RadixSort::RadixSort()
{
	hashTable = new LLQ * [2];
	tableSize = 256;
	currentTable = 0;
	currentPosition = 0;
	currentQueue = 0;

	for (int v = 0 ; v < 2; v++)
	{
		hashTable[v] = new LLQ[tableSize];
		for (int j = 0; j < tableSize; j++)
		{
			hashTable[v][j] = LLQ();
		}
	}
}

string RadixSort::padString(string data)
{
	int diff = longestStringLength - data.length();
	string padded = "";
	for(int i = 0; i < diff ; i++)
		padded += " ";
	return data += padded;
}

int RadixSort::firstReading(ifstream &inFile)
{
	longestStringLength = 0;
	string word;
	while(inFile >> word)
	{
		if(word.length() >  longestStringLength)
		{
			longestStringLength = word.length();
			data = word; //for informational purpose
		}
	}
	return longestStringLength;
}

void RadixSort::loadStack (ifstream &inFile, LLStack * top)
{
	string data;
	while(inFile >> data)
	{
		string paddedData = padString(data);
		listNode node = listNode(paddedData);
		top->push(node);
	}
}

char RadixSort::getChar(listNode node, int position)
{
	string data = node.data;
	int length = data.length();
	return data.at(length - 1 - position);
}

int RadixSort::hashIndex(char x)
{
	return (int) x;
}
void RadixSort::dumpStack(LLStack * top, ofstream &debug, int curDigit, int curTable)
{
	while(!top->isEmpty())
	{
		listNode temp(top->pop(debug).data);
		char c = getChar(temp, currentPosition);
		int hash = hashIndex(c);
		hashTable[curTable][hash].addTail(temp);
	}
	debug << "\n*** Below are the non-empty Queues ***\n\n";
	cout << "\n*** Below are the non-empty Queues ***\n\n";
	printTable(curTable, debug);
}

void RadixSort::radixSort(LLStack * stack, ofstream &sort, ofstream &debug)
{
	dumpStack(stack, debug, currentPosition, currentTable);
	currentPosition++;
	currentTable = 1;
	previousTable = 0;
	currentQueue = 0;

	while (currentPosition < longestStringLength)
	{
		while (currentQueue <= tableSize - 1)
		{
			while (!hashTable[previousTable][currentQueue].isEmpty())
			{
				listNode temp = listNode(hashTable[previousTable][currentQueue].deleteHead(debug).data);
				char c = getChar(temp, currentPosition);
				int hash = hashIndex(c);
				hashTable[currentTable][hash].addTail(temp);
			}
			currentQueue++;
		}
		printTable(currentTable, debug);
		int t = currentTable;
		currentTable = previousTable;
		previousTable = t;
		currentQueue = 0;
		currentPosition++;
	}
	output(sort, debug);
}

void RadixSort::printTable(int curTable, ofstream& debug)
{
	for (int q = 0; q < tableSize; q++)
		if(!hashTable[curTable][q].isEmpty())
		{
			string temp = hashTable[curTable][q].printQueue(curTable, q, debug);
			cout << temp;
		}
	debug << "\n\n";
	cout << "\n\n";
}

void RadixSort::output(ofstream &sort, ofstream &debug)
{
	int total = 0;
	int column = 0;
	for (int j = 0; j < tableSize; j++)
	{
		if(!hashTable[previousTable][j].isEmpty())
		{
			while(!hashTable[previousTable][j].isEmpty())
			{
				string data = hashTable[previousTable][j].deleteHead(debug).data;
				sort << data << " ";
				total++;
				if (++column % 5 == 0) // 5 PER COLUMN.
					sort << "\n";
			}
		}
	}
	stringstream stat;
	stat << "\n** Processed " << total << " strings **\n"
			<<"\n** Longest string:  " << data << " **\n"
			<<"\n** Number of characters in largest string  " << longestStringLength << " digits **\n";
	debug << stat.str();
	cout << stat.str();
}


