#include <stdio.h>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <queue>

using namespace std;

class Node {
private:
	int size;			
	int empty;			

public:
    // constructor
	Node(){ 
    empty = -1;
	size = 0;
	state = new string("");
	moves = new string("");
    }				
	
    Node(string s){
    state = new string(s);
	size = state->size();
	empty = -1;
	for (int i = 0; i < size; i++) {	
		if (state->at(i) == '0')
			empty = i;
	}
	checkEmpty();
	moves = new string("");	
    }

	Node(string s, string m){
        state = new string(s);
	size = state->size();
	empty = -1;
	for (int i = 0; i < size; i++) {	
		if (state->at(i) == '0')
			empty = i;
	}
	checkEmpty();
	moves = new string(m);	
    }

	string* state;		// tile puzzle 
	string* moves;		// moves

    //change current state into new string
void setState(string s) {
	if (s.size() != size) {
		system("cls");
		string input;
		cin >> input;
		setState(input);
	}
	else {
		(*state) = s;
		empty = -1;
		for (int i = 0; i < size; i++) {
			if (state->at(i) == '0')
				empty = i;
		}
		checkEmpty();
	}
}


	// check if blank is within puzzle
    void checkEmpty() {
	if (empty == -1) {	
		string input;
		cout << endl << "enter string with one 0" << endl;

		cin >> input;
		setState(input);
	}
}	


    // check if a move is valid on current state
bool valid(char m) {
	int sq = sqrt(size);
	if (m == 'u' || m == 'U') {			
		if (empty < sq)
			return false;
		else
			return true;
	}
	else if (m == 'd' || m == 'D') {	
		if (empty >= (size - sq))
			return false;
		else
			return true;
	}
	else if (m == 'l' || m == 'L') {	
		if (empty % sq == 0)
			return false;
		else
			return true;
	}
	else if (m == 'r' || m == 'R') {	
		if (empty % sq == (sq - 1))
			return false;
		else
			return true;
	}
	else
		return false;					
}

string swapString(int x, int y) {
	string st = *state;
	swap(st.at(x), st.at(y));
	return st;
}
	
    //perform a move on puzzle tile
void move(char m) {
int temp;
int sq = sqrt(size);
switch (m) {										
	case 'u': case 'U': temp = empty - sq;	
	break;
	case 'd': case 'D': temp = empty + sq;	
	break;
	case 'l': case 'L': temp = empty - 1;	
	break;
	case 'r': case 'R': temp = empty + 1;	
	break;
	default:  temp = -1;
}
swap(state->at(empty), state->at(temp));		
moves->push_back(m);								
empty = temp;
}

		
// move on puzzle tile and return as new Node
Node* moveAct(char m) {
	Node* ret;
	if (!valid(m)) {		// Invalid move
		ret = new Node();
		return ret;
	}
	else {
		int temp;
		int sq = sqrt(size);
		switch (m) {										
		case 'u': case 'U': temp = empty - sq;	
		break;
		case 'd': case 'D': temp = empty + sq;	
		break;
		case 'l': case 'L': temp = empty - 1;	
		break;
		case 'r': case 'R': temp = empty + 1;	
		break;
		default:  temp = -1;
		}
		string newState = swapString(empty, temp);		
		string newMove = *moves;
		newMove.push_back(m);
		ret = new Node(newState, newMove);
		return ret;
	}
}

//sequence of moves for testing solutions
void sequence(std::string s) {
	string temp = *state;
	bool invalid = false;

	cout << "Move sequence: " << s << std::endl << std::endl;

	for (unsigned int i = 0; i < s.size(); i++) {
		if (!valid(s[i])) {	
			system("cls");
			cout << "Attempted move: " << s[i] << endl;
			cout << "State:" << endl;
			i = 100;
			invalid = true;
		}
		else {					// Perform sequence move by move
			move(s[i]);
		}
	}

	if (!invalid) {
		cout << "valid" << std::endl;
	}
	else {			
		string input;
		cout << "Please enter new sequence of moves:" << endl;
		cin >> input;
		system("cls");
		setState(temp);
		sequence(input);
	}
}		
};


class SearchAlgo {
private:
	Node* initial;			
	// frontier		
	queue<Node*> frontier;
	string* reached;			
	string final;		
	//expanded nodes		
	unsigned int numExp;			
	unsigned int numNode;			
	Node* end;					
	Node* next;						
	Node* result;					
	Node* fail;						

    public:
	//constructor
	SearchAlgo(){
    initial = new Node();
	final = "";
	reached = new string[0];
	numExp = 0;
	numNode = 0;
	result = new Node();
	fail = new Node();
	end = new Node();
	next = new Node();
    }							
	
    
    SearchAlgo(string state, string fin){
    initial = new Node(state);
	final = fin;
	reached = new string[1000000];
	next = new Node();
	result = new Node("0");
	fail = new Node();
	numExp = 0;
	numNode = 0;
	end = new Node("0");
    }

	int getNumExp() { 
        return numExp;
    }	
	
    
    //expand parent children nodes
    Node* expand(Node* n, int i){
        char last = ' ';
	if (!(*n).moves->empty())
		last = (*n).moves->back();

	if (i == 0 && last != 'D')		
		return (*n).moveAct('U');
	else if (i == 1 && last != 'U')
		return (*n).moveAct('D');
	else if (i == 2 && last != 'R')
		return (*n).moveAct('L');
	else if (i == 3 && last != 'L')
		return (*n).moveAct('R');
	else {
		Node* emp = new Node();
		return emp;
	}
    }					
	
    
    // Breadth-First-Search
    Node* BFS(){									
    Node* n = initial;				
	if ((*n->state) == final)		//checks if the current state is goal
		return n;
	frontier.push(n);		
	numNode++;
	reached[0] = *(n->state);		// reached

	string s = "";
	bool inReach = false;

	while (!frontier.empty()) {		// frontier is not empy
		n = frontier.front();		// POP(frontier)
		frontier.pop();
		numExp++;
		for (int i = 0; i < 4; i++) {
			next = expand(n, i);
			if (*(next->state) != string("")) {
				s = *(next->state);			// s <- child.STATE
				if (s == final)				// if goal is met
					return next;
				inReach = false;
				for (unsigned int j = 0; j < numNode; j++) {
					if (s == reached[j])
						inReach = true;
				}
				if (!inReach) {				
					reached[numNode] = s;	
					numNode++;
					frontier.push(next);	
				}
			}
		}
	}
	return fail;
    }

    // Depth-First-Search
	Node* DFS(int limit){
    Node* n = initial;
	return DFSrecursive(n, limit);
    }	


    // used in DFS
	Node* DFSrecursive(Node* n, int limit){
        bool cut = false;						// set end to false
	if ((*n->state) == final)				// if goal is met
		return n;
	else if (n->moves->size() == limit)		
		return end;
	else {
		numExp++;
		for (int i = 0; i < 4; i++) {	
			next = expand(n, i);
			if (*(next->state) != string("")) {
				result = DFSrecursive(next, limit);				
				if (*(result->state) == string("0"))
					cut = true;
				else if (*(result->state) != string(""))
					return result;
			}
		}
	}
	if (cut)			// if end
		return end;
	else {
		return fail;
	}
    }		
	

     // Iterative Deeping Search
    Node* IDS(){
        for (int i = 0; i < INT_MAX; i++) {				// depth goes to infinity 
		result = DFS(i);							
		if (*(result->state) != string("0"))	// result != end
			return result;
	}
	return result;
    }				
};

int main()
{
    string input;
    string goal; 
    cout << "Enter initial state: ";
    cin >> input;
    cout << "Enter goal state: ";
    cin >> goal;
    cout << endl;

	int choice;
	cout << "Plase select the corresponding number for the algorithms you would like to run the search with" << endl;
	cout << "1. Breadth First Seach" << endl;
	cout << "2. Depth First Seach" << endl;
	cout << "3. Iterative Deepening Depth First Seach" << endl;
	cin >> choice;

    SearchAlgo* stringSearch = new SearchAlgo(input, goal);
	Node* final = new Node();
    
	if(choice == 1){
		final = stringSearch->BFS();
		int extendedN = stringSearch->getNumExp();
		string movesF = (*final->moves);

		cout << "Expanded node: " << extendedN << endl;
		cout << "Sequence of moves: " << movesF << endl;
	}

	else if(choice == 2){
		final = stringSearch->DFS(30);
		int extendedN = stringSearch->getNumExp();
		string movesF = (*final->moves);

		cout << "Expanded node: " << extendedN << endl;
		cout << "Sequence of moves: " << movesF << endl;
	}

	else if (choice == 3){
		final = stringSearch->IDS();
		int extendedN = stringSearch->getNumExp();
		string movesF = (*final->moves);

		cout << "Expanded node: " << extendedN << endl;
		cout << "Sequence of moves: " << movesF << endl;
	}
	
	else{
		cout << "choice invalid" << endl;
	}

    return 0;
}

