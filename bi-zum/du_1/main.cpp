#include <iostream>
#include <utility>
#include "vector"

using namespace std;

class Route
{
public:
    string name;
    Route * predecesor = nullptr;

    Route(string geh) : name(std::move(geh)){}
};

int getTime(char c)
{
    switch (c) {
        case 'a':
            return 1;
        case 'b':
            return 3;
        case 'c':
            return 5;
        case 'd':
            return 8;
        case 'e':
            return 12;
        default:
            return 100000;
    }
}


vector<char> trimVector(vector<char> org, char left, char right)
{
    for (int i = 0; i < org.size(); i++)
    {
        if(org[i] == left || org[i] == right)
        {
              org.erase(org.cbegin() + i);
              i--;
        }
    }

    return org;
}

int recursion(vector<char> left, vector<char> right, bool left_battery, Route * folower)
{
    if(left.empty())
        return 0;

    int min = 1000000;

    if(left_battery) {
        for (int i = 0; i < left.size(); i++) {
            for (int j = 0; j < left.size(); j++) {
                if (i == j) {}
                else {
                    string t{left[i], left[j]};

                    Route * this_route = new Route(t);

                    auto tmp_r = right;
                    auto tmp_l = left;

                    tmp_r.push_back(left[i]);
                    tmp_r.push_back(left[j]);

                    tmp_l = trimVector(tmp_l, left[i], left[j]);
                    int temp = recursion(tmp_l, tmp_r, false, this_route);
                    temp += max(getTime(left[i]), getTime(left[j]));
                    if(temp < min) {
                        folower->predecesor = this_route;
                        min = temp;
                    }

                }
            }
        }
    }
    else
    {
        for(int i = 0; i < right.size(); i++)
        {
            string t{right[i]};
            Route * this_route = new Route(t);

            auto tmp_r = right;
            tmp_r = trimVector(tmp_r, right[i], right[i]);
            auto tmp_l = left;
            tmp_l.push_back(right[i]);
            int temp = recursion(tmp_l, tmp_r, true, this_route);
            temp += getTime(right[i]);
            if(temp < min) {
                folower->predecesor = this_route;
                min = temp;
            }

        }
    }

    return min;
}


int main() {

    vector<char> to_go;
    vector<char> house;

    to_go.push_back('a');//1
    to_go.push_back('b');//3
    to_go.push_back('c');//5
    to_go.push_back('d');//8
    to_go.push_back('e');//12

    Route * head = new Route("000");

    cout << recursion(to_go, house, true, head) << endl;

    while(true)
    {
        if(head == nullptr)
            return 0;

        cout << head->name << endl;
        head = head->predecesor;
    }



}
