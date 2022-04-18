/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
C#, VB, Perl, Swift, Prolog, Javascript, Pascal, HTML, CSS, JS
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <limits>

using namespace std;

/*
* struct to hold the output information
* Output information: solution and numExpanded
*/
struct Puzzle{
    string solution; //the string of moves that creates the shortest path from initial state to goal state
    int numExpanded; //the number of states which were expanded to reach the goal
};

/*
* sortbysec is used in the A* searches. It sorts the state from greatest cost to least cost.
*/
bool sortbysec(const pair<string,int> &a,const pair<string,int> &b){
    return (a.second > b.second); //second is the int which holds the cost of the state.
}

/*
* getAvailableMoves generates a vector of all possible states reachable by the current state.
*/
vector<pair<string, int>> getAvailableMoves(pair<string, int> parent){

    int blankPosition = parent.first.find("0"); //the location of the zero(or blank) in the current state/string
    string temp; //temp holds the 0 character
    pair<string, int> child; //initializes variable for state which will result from moving the blank tile
    int size; //length of a single row/column of the puzzle
    
    //determines size based off of the total number of tiles. size = sqrt(total num of tiles)
    if (parent.first.size() == 9){size = 3;}
    if (parent.first.size() == 16){size = 4;}
    
    vector<pair<string, int>> neighbors; //vector to hold all the possible states from moving the blank in all possible directions
    
    //if the blank is not on the top row, the U(up) movement is possible
    if (blankPosition >= size){
        child = parent; //make the "moved to" state equal to the current state
        
        //swap the 0 character and the character in the position the 0 needs to be in
        temp = child.first.at(blankPosition);
        child.first.at(blankPosition) = child.first.at(blankPosition - size);
        child.first.replace(blankPosition - size,1,temp);  
        neighbors.push_back(child);
    }
    
    //if the blank tile is not on the buttom row, the D(down) movement is possible
    if (blankPosition < (parent.first.size() - size)){
        child = parent;
        temp = child.first.at(blankPosition);
        child.first.at(blankPosition) = child.first.at(blankPosition + size);
        child.first.replace(blankPosition + size,1,temp);  
        neighbors.push_back(child);
    }
    
    //if the blank tile is not on the left most column, the L(left) movement is possible
    if (blankPosition % size != 0){
        child = parent;
        temp = child.first.at(blankPosition);
        child.first.at(blankPosition) = child.first.at(blankPosition - 1);
        child.first.replace(blankPosition - 1,1,temp);  
        neighbors.push_back(child);
    }
    
    //if the blank tile is not on the right most column, the R(right) movement is possible
    if ((blankPosition % size) != (size - 1)){
        child = parent;
        temp = child.first.at(blankPosition);
        child.first.at(blankPosition) = child.first.at(blankPosition + 1);
        child.first.replace(blankPosition + 1,1,temp);  
        neighbors.push_back(child);
    }
    
    return neighbors; //returns the vector of states resulting in moving in all possible directions
}

/*
* outOfPlace calculates the Out-Of-Place heuristic for a given state.
*/
int outOfPlace(string input, string goal){
    int result = 0;
    
    //iterates through every character in the string.
    for (int i = 0; i < input.size(); i++){
        
        //if the character in the input and goal strings in the same position are not equal, add 1;
        if (input.at(i) != goal.at(i)){
            result +=1;
        }
    }
    return result;
}

/*
* manhattanDistance calculates the Manhattan Distance heuristic for a given state.
*/
int manhattanDistance(string input, string goal){
    int result = 0;
    int distance; //calculates number of moves from character in input to character in goal
    int index; //holds the position of the current character in goal
    
    //for every character except for the blank, calculate the moves to get from input
    //position to goal position. Add all the results up.
    for (int i = 0; i < input.size(); i++){
        if (input.substr(i, 1) == "0"){i+=1;}
        if (i < 9){
            index = goal.find(input.at(i));
            distance = abs((i/3) - (index/3)) + abs((i%3) - (index%3));
            result += distance;
        }
    }
    
    return result;
}

/*
* findSolution uses the tracking unordered_map to trace the goal to the input and return the
* required moves to get from input to goal.
*/
string findSolution(string input, string goal, unordered_map<string, string> came_from){
    string solution = ""; //variable for the string of moves
    string start = goal; //variable to hold the goal state
    string next = came_from.at(goal); //variable to hold the state used to get to goal
    
    //calculating size of column/row again
    int size;
    if (input.size() == 9){size = 3;}
    if (input.size() == 16){size = 4;}
    
    //until a next does not exist, loop to add the next move to the string
    while (came_from.count(next) != 0){
        string move = ""; //holds the move character
        int posOfZeroStart = start.find("0"); //position of the 0 in start
        int posOfZeroNext = next.find("0"); //position of the 0 in next
        
        //determines move by the result of subtracting next's zero's position from start's
        
        //  0  1  2
        //  3  4  5
        //  6  7  8
        
        //2 - 1 = 1 equals a left move
        //1 - 2 = -1 equals a right move
        //3 - 6 = -3 equals a down move
        //6 - 3 = -3 equals an up move
        
        //note: in the below logic, the characters are opposite than said above.
        //this is because we start from the goal state, not the initial state.
        
        if (posOfZeroStart - posOfZeroNext == 1)
            move = "R";
        else if (posOfZeroStart - posOfZeroNext == -1)
            move = "L";
        else if (posOfZeroStart - posOfZeroNext == (-1 * size))
            move = "U";
        else if (posOfZeroStart - posOfZeroNext == size)
            move = "D";
        
        solution += move; //adds move to the string of moves
        
        //grabs the next pair of states
        start = next;
        next = came_from.at(start);
    }
    return solution; //returns a reversed solution (to be corrected later)
}

/*
* AStarOutOfPlace calculates the sho rtest path and the number of expanded nodes using the A*
* search algorithm with the Out-Of-Place heuristic.
*/
Puzzle AStarOutOfPlace(string input, string goal){
    Puzzle puzzle = { "", 0 };
    
    vector<pair<string, int>> frontier; //list of all states found but not expanded
    frontier.push_back(make_pair(input, 0));
    
    unordered_map<string, string> came_from; //list of all discovered connections between states
    unordered_map<string, int> cost_so_far; //list of the cost so far of a state
    came_from.insert(make_pair(input, ""));
    cost_so_far.insert(make_pair(input, 0));
   
    int expand = 0; //tracks how many states have been expanded
    int new_cost = 0; //tracks cost of new state
    
    //until the frontier is empty or until the goal is found, loop
    while (!frontier.empty()){
        pair<string, int> current = frontier.back(); //grabs next state to be explored
        frontier.pop_back();
        
        //if the current state is the goal state, break from the loop
        if (current.first == goal)
            break;
        
        expand += 1; //add 1 to expand before expanding node
        
        //creates list of available states from current state
        vector<pair<string, int>> moves;
        moves = getAvailableMoves(current);
        
        /*
        for each next state, create a new state and push it onto the frontier
        only if it hasn't already been discovered or the cost to this node is lower
        than the previous time it was found
        */
        for (int i = 0; i < moves.size(); i++){
            new_cost = cost_so_far.at(current.first) + 1;
            
            if ((cost_so_far.count(moves.at(i).first) == 0) || (new_cost < cost_so_far.at(moves.at(i).first))){
               cost_so_far.insert(make_pair(moves.at(i).first, new_cost)); 
               int priority = new_cost + outOfPlace(moves.at(i).first, goal);
               frontier.push_back(make_pair(moves.at(i).first, priority));
               sort(frontier.begin(), frontier.end(), sortbysec);               //sorts the new state into the correct order for it to be expanded
               came_from.insert(make_pair(moves.at(i).first, current.first));
            }
        }
    }
    
    //finds the string of moves
    string solution = findSolution(input, goal, came_from);
    reverse(solution.begin(),solution.end()); //at this point we reverse it.
    
    //updates the output struct object
    puzzle.solution = solution;
    puzzle.numExpanded = expand;
    return puzzle; // returns the objectz
}

/*
* AStarManhattanDistance calculates the shortest path and the number of expanded nodes using the A*
* search algorithm with the Manhattan distance heuristic.
*/
Puzzle AStarManhattanDistance(string input, string goal){
    /*
    * This is an exact replica of AStarOutOfPlace only the heuristic function is changed from
    * outOfPlace to manhattanDistance
    */
    
    Puzzle puzzle = { "", 0 };
    vector<pair<string, int>> frontier;
    frontier.push_back(make_pair(input, 0));
    unordered_map<string, string> came_from;
    unordered_map<string, int> cost_so_far;
    came_from.insert(make_pair(input, ""));
    cost_so_far.insert(make_pair(input, 0));
    int expand = 0;
    int new_cost = 0;
    while (!frontier.empty()){
        pair<string, int> current = frontier.back();
        frontier.pop_back();
        
        //cout << "Expanded: " << current.first << endl;
    
        if (current.first == goal)
            break;
        
        expand += 1;
        vector<pair<string, int>> moves;
        
        moves = getAvailableMoves(current);
        
        for (int i = 0; i < moves.size(); i++){
            new_cost = cost_so_far.at(current.first) + 1;
            
            if ((cost_so_far.count(moves.at(i).first) == 0) || (new_cost < cost_so_far.at(moves.at(i).first))){
               cost_so_far.insert(make_pair(moves.at(i).first, new_cost));
               int priority = new_cost + manhattanDistance(moves.at(i).first, goal);
               frontier.push_back(make_pair(moves.at(i).first, priority));
               sort(frontier.begin(), frontier.end(), sortbysec);
               came_from.insert(make_pair(moves.at(i).first, current.first));
            }
        }
    }
    string solution = findSolution(input, goal, came_from);
    reverse(solution.begin(),solution.end());
    puzzle.solution = solution;
    puzzle.numExpanded = expand;
    return puzzle;
}

/*
* isGoal determines if the current state is the goal state. returns true if the strings are equal and
* false if the strings are not equal.
*/
bool isGoal(string input, string goal){
    if (input == goal){return true;}
    else {return false;}
}

/*
* search is a recursive function that aids AStarIterativeDeepeningOutOfPlace in finding the shortest path.
*/
int search(vector<pair<string, int>> path, int g, int bound, string goal, vector<pair<string, string>> &came_from, int &expand){
    pair<string, int> node = path.back(); //grabs the next state to be evaluated
    int f = g + outOfPlace(node.first, goal); //estimated cost of the state
    
    if (f > bound){return f;}                   //if state cost is larger than the bound, end the search iteration
    if (isGoal(node.first, goal)){return -1;}  //if the state == the goal state, end the search
    
    int min = std::numeric_limits<int>::max(); //min keeps track of the lowest cost so lowest cost can be returned
    int t; //keeps track of current cost
    
    expand += 1; //keeps track of the number of expanded nodes
    vector<pair<string, int>> moves = getAvailableMoves(node); //holds next states of current states
    
    //for all the new states, push it onto the path. Evaluate t. If t is -1, the goal has been found
    //form a move between next node and node. Finally, if t < minimum bound, minimum bound is now eaqual to t. pop the new node
    //from the path.
    for (int i = 0; i < moves.size(); i++){
        if(count(path.begin(), path.end(), moves.at(i)) == 0){
            path.push_back(moves.at(i));
            t = search(path, g + 1, bound, goal, came_from, expand);
            if (t == -1){
                came_from.push_back(make_pair(node.first, moves.at(i).first));
                return -1;
                
            }
            if (t < min){min = t;}
            path.pop_back();
        }
    }
    return min;
}

/*
* AStarIterativeDeepeningOutOfPlace calculates the shortest path and the number of expanded nodes using the IDA*
* search algorithm with the Out-Of-Place heuristic.
*/
Puzzle AStarIterativeDeepeningOutOfPlace(string input, string goal){
    Puzzle puzzle = { "", 0 };
    vector<pair<string, int>> path;
    vector<pair<string, string>> came_from;
    int bound = outOfPlace(input, goal); //bound starts as the cost of the input state
    path.push_back(make_pair(input, 0)); //pushes back the initial state onto path
    
    //similar to other algorithms
    string solution;
    int t;
    int expand;
    
    while (true){
        t = search(path, 0, bound, goal, came_from, expand); //recursive function until expanding is not possible
        //if the state is a goal state, evaluate the string of moves(solution)
        
        //note replica of findSolution.
        if (t == -1){
            
            string solution = "";
            string start;
            string next;
            int size;
            if (input.size() == 9){size = 3;}
            if (input.size() == 16){size = 4;}
            while (came_from.size() != 0){
                start = came_from.back().first;
                next = came_from.back().second;
                string move = "";
                int posOfZeroStart = start.find("0");
                int posOfZeroNext = next.find("0");
                
                if (posOfZeroStart - posOfZeroNext == 1)
                    move = "L";
                else if (posOfZeroStart - posOfZeroNext == -1)
                    move = "R";
                else if (posOfZeroStart - posOfZeroNext == (-1 * size))
                    move = "D";
                else if (posOfZeroStart - posOfZeroNext == size)
                    move = "U";
        
                solution += move;
                came_from.pop_back();
            }
            
            puzzle.solution = solution;
            puzzle.numExpanded = expand;
            return puzzle;
        }
        if (t == std::numeric_limits<int>::max()){return puzzle;}
        bound = t;
    }
    return puzzle;
}

/*
* search is a recursive function that aids AStarIterativeDeepeningManhattanDistance in finding the shortest path.
*/
int search1(vector<pair<string, int>> path, int g, int bound, string goal, vector<pair<string, string>> &came_from, int &expand){
    /*
    * This is an exact replica of search only the heuristic function is changed from
    * outOfPlace to manhattanDistance
    */
    
    pair<string, int> node = path.back();
    int f = g + manhattanDistance(node.first, goal);
    if (f > bound){return f;}
    if (isGoal(node.first, goal)){return -1;}
    int min = std::numeric_limits<int>::max();
    int t;
    
    expand += 1;
    vector<pair<string, int>> moves = getAvailableMoves(node);
    for (int i = 0; i < moves.size(); i++){
        if(count(path.begin(), path.end(), moves.at(i)) == 0){
            path.push_back(moves.at(i));
            t = search1(path, g + 1, bound, goal, came_from, expand);
            if (t == -1){
                came_from.push_back(make_pair(node.first, moves.at(i).first));
                return -1;
                
            }
            if (t < min){min = t;}
            path.pop_back();
        }
    }
    return min;
}

/*
* AStarIterativeDeepeningManhattanDistance calculates the shortest path and the number of expanded nodes using the IDA*
* search algorithm with the Manhattan distance heuristic.
*/
Puzzle AStarIterativeDeepeningManhattanDistance(string input, string goal){
    
    /*
    * This is an exact replica of AStarIterativeDeepeningOutOfPlace only the heuristic function is changed from
    * outOfPlace to manhattanDistance
    */
    
    Puzzle puzzle = { "", 0 };
    vector<pair<string, int>> path;
    vector<pair<string, string>> came_from;
    int bound = manhattanDistance(input, goal);
    path.push_back(make_pair(input, 0));
    string solution;
    int t;
    int expand;
    
    while (true){
        t = search1(path, 0, bound, goal, came_from, expand);
        if (t == -1){
            
            string solution = "";
            string start;
            string next;
            int size;
            if (input.size() == 9){size = 3;}
            if (input.size() == 16){size = 4;}
            while (came_from.size() != 0){
                start = came_from.back().first;
                next = came_from.back().second;
                string move = "";
                int posOfZeroStart = start.find("0");
                int posOfZeroNext = next.find("0");
                
                if (posOfZeroStart - posOfZeroNext == 1)
                    move = "L";
                else if (posOfZeroStart - posOfZeroNext == -1)
                    move = "R";
                else if (posOfZeroStart - posOfZeroNext == (-1 * size))
                    move = "D";
                else if (posOfZeroStart - posOfZeroNext == size)
                    move = "U";
        
                solution += move;
                came_from.pop_back();
            }
            
            puzzle.solution = solution;
            puzzle.numExpanded = expand;
            return puzzle;
        }
        if (t == std::numeric_limits<int>::max()){return puzzle;}
        bound = t;
    }
    return puzzle;
}

/*
* tests a string of moves on its initial problem state and results with a goal state.
*/
string testingAlgoritm(string input, string moves){
    //--
    string temp = "0";
    int blankPosition;
    int size;
    
    if (input.size() == 9){size = 3;}
    else {size = 4;}
    //-- similar logic in findSolution

    //for every move in the list, move the blank tile in the input state in that direction
    for (int i = 0; i < moves.size(); i++){
        blankPosition = input.find("0");
        
        //similar logic to getAvailableMoves
        if (moves.at(i) == 'L'){
            input.at(blankPosition) = input.at(blankPosition - 1); //position 0 = other
            input.replace(blankPosition - 1,1,temp);  
        }
        else if (moves.at(i) == 'R'){
            input.at(blankPosition) = input.at(blankPosition + 1); //position 0 = other
            input.replace(blankPosition + 1,1,temp);  
        }
        else if (moves.at(i) == 'U'){
            input.at(blankPosition) = input.at(blankPosition - size); //position 0 = other
            input.replace(blankPosition - size,1,temp);  
        }
        else if (moves.at(i) == 'D'){
            input.at(blankPosition) = input.at(blankPosition + size); //position 0 = other
            input.replace(blankPosition + size,1,temp);  
        }
    }
    return input;
}

int main()
{

    string input; //variable to store initial state
    string goal;  //variable to store goal state
    string mode;  //variable to store mode which determines desired algorithm
    Puzzle result; //variable to store the results of the algorithm
    
    //initializing variables using user input
    cout << "Enter initial state: ";
    cin >> input;
    cout << "Enter goal state: ";
    cin >> goal;
    cout << endl;
    cout << "00: A* w/ Out-Of-Place" << endl;
    cout << "01: A* w/ Manhattan distance" << endl;
    cout << "10: IDA* w/ Out-Of-Place" << endl;
    cout << "11: IDA* w/ Manhattan distance" << endl;
    cout << "Enter algorithm you want to use: ";
    cin >> mode;
    cout << endl;
    
    //displaying chosen states and algorithm
    cout << "Initial state: " << input << endl;
    cout << "Goal state: " << goal << endl;
    cout << "Algorithm: ";
    if (mode == "00") {cout << "A* w/ Out-Of-Place" << endl << endl;}
    else if (mode == "01") {cout << "A* w/ Manhattan distance" << endl << endl;}
    else if (mode == "10") {cout << "IDA* w/ Out-Of-Place" << endl << endl;}
    else if (mode == "11") {cout << "IDA* w/ Manhattan distance" << endl << endl;}
    cout << "Calculating solution..." << endl << endl;
    
    //if else logic to determine and use correct algorithm
    if (mode == "00") {
        result = AStarOutOfPlace(input, goal); //performs algorithm and stores results in result
        
        //displays result to user
        cout << "Results using A* Search and the Out-Of-Place heuristic: " << endl;
        cout << "Solution: " << result.solution << endl;
        cout << "Number of states expanded: " << result.numExpanded << endl;
        cout << endl;
    }
    else if (mode == "01"){
        result = AStarManhattanDistance(input, goal);
    
        cout << "Results using A* Search and the Manhattan distance heuristic: " << endl;
        cout << "Solution: " << result.solution << endl;
        cout << "Number of states expanded: " << result.numExpanded << endl;
        cout << endl;
    }
    else if (mode == "10"){
        result = AStarIterativeDeepeningOutOfPlace(input, goal);
        
        cout << "Results using IDA* Search and the Out-Of-Place heuristic: " << endl;
        cout << "Solution: " << result.solution << endl;
        cout << "Number of states expanded: " << result.numExpanded << endl;
        cout << endl;
    }
    else if (mode == "11"){
        result = AStarIterativeDeepeningManhattanDistance(input, goal);
        
        cout << "Results using A* Search and the Manhattan distance heuristic: " << endl;
        cout << "Solution: " << result.solution << endl;
        cout << "Number of states expanded: " << result.numExpanded << endl;
        cout << endl;
    }
    
    //testing the resulting moves to see if they create the goal state using the input state.
    string test = testingAlgoritm(input, result.solution);
    
    cout << "Testing..." << endl << endl;
    
    cout << test << " should equal " << goal << endl;
    

    return 0;
}


