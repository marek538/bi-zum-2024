#pragma clang diagnostic push
#pragma ide diagnostic ignored "performance-unnecessary-value-param"
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <queue>
#include <cstdlib>
#include <stack>
#include <unistd.h>
#include <cmath>

#define RED_TEXT "\033[41m"
#define BLUE_TEXT "\033[44m"
#define GREEN_TEXT "\033[42m"
#define BLACK_TEXT "\033[40m"
#define RESET_TEXT "\033[0m"
#define PROGRESS_CONSTANT 5

using namespace std;

void printProgress(vector<string> &map)
{
    cout << "\033[2J\033[H";
    usleep(500000);

    for(auto &str : map)
    {
        for(char ch : str)
        {
            if(ch == 'P')
                cout << RED_TEXT << ch << RESET_TEXT;
            else if(ch == 'T')
                cout << BLUE_TEXT << ch << RESET_TEXT;
            else if(ch == 'S')
                cout << GREEN_TEXT << ch << RESET_TEXT;
            else if(ch == 'X')
                cout << BLACK_TEXT << ch << RESET_TEXT;
            else
                cout << ch;
        }
        cout << endl;
    }

}


void printMap(vector<string> &input)
{
    for (auto &str: input)
        cout << str << endl;
}

int getRandomIndex(size_t length) {
    return rand() % length; // Generate random number between 0 and length-1
}

pair<int, int> extractNumber(string input) //00, 11
{
    uint i = 0, left = 0, right = 0;

    for (;i < input.length(); i++)
    {
        if (isdigit(input[i]))
            left = left * 10 + (input[i] - 48);
        else
            break;
    }

    for (;i < input.length(); i++)
        if(isdigit(input[i]))
            right = right * 10 + (input[i] - 48);

    return make_pair(left, right);
}

class CHeuristicNode
{
public:

    pair<int, int> coordinates;
    double heur_value = 0;
    int dijkstraLen = 0;

    CHeuristicNode(int x, int y, double val, int dLen) : coordinates(make_pair(x, y)), heur_value(val), dijkstraLen(dLen) {}

    bool operator<(const CHeuristicNode &other) const {
        // Compare based on heuristic value
        return this->heur_value < other.heur_value;
    }


};

class CMap
{
public:
    vector<string> map;
    pair<int, int> start = {0, 0};
    pair<int, int> end = {0, 0};

    CMap(string location)
    {
        string tmp_text;
        ifstream ReadFile(location);

        if (!ReadFile.good())
            throw invalid_argument( "file not found" );

        while(getline(ReadFile, tmp_text))
        {
            if (!ReadFile.good())
                throw invalid_argument( "file not found" );
            if(tmp_text[0] == 's') // read start
                start = extractNumber(tmp_text.substr(6));

            else if(tmp_text[0] == 'e') // read end
                end = extractNumber(tmp_text.substr(4));
            else
                map.push_back(tmp_text);
        }
        ReadFile.close();

        validateInput();
        // !!!!! debug
        switchCoordPos();

        map[start.first][start.second] = 'S';
        map[end.first][end.second] = 'P';
    }

    void switchCoordPos()
    {
        auto tmp = start.second;
        start.second = start.first;
        start.first = tmp;
        tmp = end.second;
        end.second = end.first;
        end.first = tmp;
    };

    void print()
    {
        for(auto &str : map)
            cout << str << endl;

    }

    bool sameLengthsandChars()
    {

        uint l = map[0].length();
        for(auto &line : map)
        {
            if (line.length() != l)
                return false;

            for (auto ch : line)
                if(ch != ' ' && ch != 'X')
                    return false;
        }

        for(uint i = 0; i < map[0].length(); i++)
        {
            if(map[0][i] != 'X' || map[map.size() - 1][i] != 'X')
                return false;
        }
        return true;
    }

    void validateInput() // throw error if not correct
    {
        if(start.first == 0
            || start.second == 0
            || end.first == 0
            || end.second == 0
            || uint(start.first) >= map[0].length()
            || uint(start.second) >= map.size()
            || uint(end.first) >= map[0].length()
            || uint(end.second) >= map.size()
            || !sameLengthsandChars())
            //|| map[start.first][start.second] != ' '
            //|| map[end.first][end.second] != ' '
            throw invalid_argument( "wrong input" );
    }

};

vector<string> routeRealization(bool found, map<pair<int, int>, pair<int, int>>& visited, CMap &game, int expanded)
{
    int length = 0;
    if(!found)
        return game.map;

    auto tmp = game.map;
    auto coord = game.end;
    while(coord != make_pair(0, 0))
    {
        length++;
        tmp[coord.first][coord.second] = 'P';
        coord = visited[coord];
    }


    cout << "delka cesty = " << length << endl;
    cout << "otevreno uzlu = " << expanded << endl;

    return tmp;
}

vector<pair<int, int>> initialization()
{
    vector<pair<int, int>> shift;
    shift.emplace_back(0, 1);
    shift.emplace_back(0, -1);
    shift.emplace_back(1, 0);
    shift.emplace_back(-1, 0);

    return shift;
}

vector<string> DFSRoute(CMap game)
{
    int progress = 5;
    int expanded = 1;

    if(game.start == game.end)
        return game.map;

    map<pair<int, int>, pair<int, int>> visited; // visited, predecessor
    stack<pair<int, int>> toVisit; // change in DFS and in RndS
    bool found = false;

    visited[game.start] = make_pair(0, 0);
    toVisit.push(game.start);

    auto shift = initialization();

    while(!toVisit.empty() && !found)
    {
        bool last_node = true;
        auto current = toVisit.top();
        toVisit.pop();

        for(auto p : shift)
        {
            if(visited.count(make_pair(current.first + p.first, current.second + p.second)) == 0
               && (game.map[current.first + p.first][current.second + p.second] == ' '
               ||game.map[current.first + p.first][current.second + p.second] == 'P'))
            {
                visited[make_pair(current.first + p.first, current.second + p.second)] = current;
                toVisit.emplace(current.first + p.first, current.second + p.second);
                game.map[current.first + p.first][current.second + p.second] = 'T';
                last_node = false;
                expanded++;
                if (make_pair(current.first + p.first, current.second + p.second) == game.end)
                    found = true;
            }
        }

        progress = progress % PROGRESS_CONSTANT;
        if(last_node || progress == 0)
            printProgress(game.map);
        progress++;
    }

    return routeRealization(found, visited, game, expanded);
}

CHeuristicNode getNext(vector<CHeuristicNode> &toVisit, int algorithm) {
    int index;
    if(algorithm == 0)
        index = getRandomIndex(toVisit.size());
    else
        index = 0;

    auto current = toVisit[index];
    toVisit.erase(toVisit.cbegin() + index);
    return current;
}

double manhattan(pair<int, int> start, pair<int, int> current)
{
    return (start.first - current.first) + (start.second - current.second);
}

double distance(pair<int, int> start, pair<int, int> current)
{
    int left = (start.first - current.first) * (start.first - current.first);
    int right = (start.second - current.second) * (start.second - current.second);

    return sqrt(left + right);
}

double heuristicVal(pair<int, int> start, pair<int, int> end, pair<int, int> current, int algorithm, int dijkstraLen) {
    if(algorithm == 0)
        return 0;
    else if(algorithm == 1)
    {
        return distance(current, end);
    }
    else
    {
        return (distance(current, end) + dijkstraLen);
    }
}

// sort toVisit
void addElement(vector<CHeuristicNode> &toVisit, CHeuristicNode node, int algorithm) {
    if (algorithm == 0) {
        toVisit.push_back(node);
        return;
    }

    auto it = lower_bound(toVisit.begin(), toVisit.end(), node);
    toVisit.insert(it, node);
}

vector<string> SearchKernel(CMap game, int algorithm)
{
    int progress = 5;
    int expanded = 1;

    if(game.start == game.end)
        return game.map;

    map<pair<int, int>, pair<int, int>> visited; // visited, predecessor
    vector<CHeuristicNode> toVisit;
    bool found = false;

    visited[game.start] = make_pair(0, 0);
    toVisit.emplace_back(game.start.first, game.start.second, heuristicVal(game.start, game.end, game.start,algorithm, 0), 0);

    auto shift = initialization();

    while(!toVisit.empty() && !found)
    {
        CHeuristicNode current = getNext(toVisit, algorithm);

        for(auto p : shift)
        {
            if(visited.count(make_pair(current.coordinates.first + p.first, current.coordinates.second + p.second)) == 0
               && (game.map[current.coordinates.first + p.first][current.coordinates.second + p.second] == ' '
               ||game.map[current.coordinates.first + p.first][current.coordinates.second + p.second] == 'P'))
            {
                visited[make_pair(current.coordinates.first + p.first, current.coordinates.second + p.second)] = current.coordinates;
                double herVal = heuristicVal(game.start,
                                                                game.end,
                                                                make_pair(current.coordinates.first + p.first, current.coordinates.second + p.second),
                                                                algorithm, 
                                                                (current.dijkstraLen+1));
                addElement(toVisit, CHeuristicNode(current.coordinates.first + p.first, current.coordinates.second + p.second, herVal, current.dijkstraLen+1),
                                                                algorithm);

                expanded++;
                game.map[current.coordinates.first + p.first][current.coordinates.second + p.second] = 'T';
                if (make_pair(current.coordinates.first + p.first, current.coordinates.second + p.second) == game.end)
                    found = true;
            }
        }

        progress = progress % PROGRESS_CONSTANT;
        if(progress == 0)
            printProgress(game.map);
        progress++;
    }

    return routeRealization(found, visited, game, expanded);
}

vector<string> BFSRoute(CMap game)
{
    if(game.start == game.end)
        return game.map;

    int expanded = 1;

    map<pair<int, int>, pair<int, int>> visited; // visited, predecessor
    queue<pair<pair<int, int>, int>> toVisit; //coordinates + distance
    bool found = false;

    visited[game.start] = make_pair(0, 0);
    toVisit.emplace(game.start, 0);

    auto shift = initialization();

    int layer = 0;
    while(!toVisit.empty() && !found)
    {
        auto current =  toVisit.front();

        if(layer < current.second) {
            printProgress(game.map);
            layer ++;
        }

        auto currentCoord = current.first;
        toVisit.pop();

        for(auto p : shift)
        {
            if(visited.count(make_pair(currentCoord.first + p.first, currentCoord.second + p.second)) == 0
            && (game.map[currentCoord.first + p.first][currentCoord.second + p.second] == ' '
            || game.map[currentCoord.first + p.first][currentCoord.second + p.second] == 'P'))
            {
                visited[make_pair(currentCoord.first + p.first, currentCoord.second + p.second)] = currentCoord;
                toVisit.emplace(make_pair(currentCoord.first + p.first, currentCoord.second + p.second), current.second + 1);
                game.map[currentCoord.first + p.first][currentCoord.second + p.second] = 'T';

                expanded++;
                if (make_pair(currentCoord.first + p.first, currentCoord.second + p.second) == game.end)
                    found = true;
            }
        }
    }

    return routeRealization(found, visited, game, expanded);
}

vector<string> AStarRoute(CMap game){

    return SearchKernel(game, 2);
}

vector<string> HeuristicRoute(CMap game){

    return SearchKernel(game, 1);
}

vector<string> RNDRoute(CMap game){
    return SearchKernel(game, 0);
}

 string chooseMaze()
{
    cout << "name of maze" << endl;
    string location;
    cin >> location;
    string dat = "./dataset/";
    location = dat + location;
    cout << location << endl;

    return location;
}

int chooseAlgorithm()
{
    cout << "type BFS/DFS/RNDS/ASTAR/HEUR" << endl;
    string input;
    cin >> input;

    if(input == "BFS")
        return 0;
    else if(input == "DFS")
        return 1;
    else if(input == "RNDS")
        return 2;
    else if (input == "ASTAR")
        return 3;
    else if(input == "HEUR")
        return 4;
    else
        throw invalid_argument("incorrect input");
}

int main()
{
    try {
        CMap game = CMap(chooseMaze());
        int algorithm = chooseAlgorithm();

        vector<string> output;
        switch (algorithm) {
            case 0:
                output = BFSRoute(game);
                break;
            case 1:
                output = DFSRoute(game);
                break;
            case 2:
                output = RNDRoute(game);
                break;
            case 3:
                output = AStarRoute(game);
                break;
            case 4:
                output = HeuristicRoute(game);
                break;

        }
        printProgress(output);
    }
    catch (exception &e)
    {
        cout << e.what();
    }

    return 0;
}

#pragma clang diagnostic pop