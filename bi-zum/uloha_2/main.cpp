#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <unordered_set>

#define N_QUEENS 12
#define POPULATION_SIZE 10
#define MUTATION_PROBABILITY 0.01
#define TOURNAMENT_PLAYERS 4


using namespace std;

class CSolution
{
public:
    vector<int> layout;
    int fitness = 0;

    int countFitness()
    {
        int score = 0;

        // it is enough to check only diagonals
        for(int i = 0; i < layout.size(); i++)
        {
            bool inScope = false;

            int increment = 1;
            for(int left = (i - 1); left >= 0 && !inScope; left--)
            {
                if(layout[left] == layout[i] + increment || layout[left] == layout[i] - increment)
                    inScope = true;

                increment++;
            }
            increment = 1;
            for(int right = i + 1; right < layout.size() && !inScope; right++)
            {
                if(layout[right] == layout[i] + increment || layout[right] == layout[i] - increment)
                    inScope = true;
                increment++;
            }

            if(!inScope)
                score++;
        }

        fitness = score;
        return fitness;
    }

    friend std::ostream& operator<<(std::ostream& os, const CSolution& solution) {
        os << "[ ";
        for (int value : solution.layout) {
            os << value << " ";
        }
        os << "]";
        return os;
    }

    bool operator<(const CSolution& other) const {
        return fitness < other.fitness;
    }

    void mutationSwap()
    {
        random_device rd;
        mt19937 gen(rd());

        uniform_real_distribution<> dis(0.0, 1.0);

        for(uint i = 0; i < layout.size(); i++)
        {
            double randomNumber = dis(gen);
            if(randomNumber < MUTATION_PROBABILITY)
            {
                uint start = rand() % (layout.size());
                auto tmp = layout[i];
                layout[i] = layout[start];
                layout[start] = tmp;
            }
        }
    }

    bool checkSize()
    {
        if(layout.size() != N_QUEENS)
            throw invalid_argument("nemam nervy");
        return true;
    }

    bool hasDuplicates() {
        unordered_set<int> seen;
        for (int num : layout) {
            if (seen.count(num) > 0) {
                throw invalid_argument("tvojemama");
            }
            seen.insert(num);
        }
        return false;
    }


    void generateRandom()
    {
        for(int i = 0; i < N_QUEENS; i++)
        {
            layout.push_back(i);
            shuffle(layout.begin(), layout.end(), std::mt19937(std::random_device()()));
        }
    }
};

bool isNotInVector(const std::vector<int>& vec, int value) {
    for (int elem : vec) {
        if (elem == value)
            return false;
    }
    return true;
}

class CNQueens
{
public:
    vector<CSolution> population;

    CNQueens()
    {
        for(uint i =0; i < POPULATION_SIZE; i++) {
            CSolution a;
            a.generateRandom();
            a.countFitness();
            population.push_back(a);
        }
    }

    CSolution genomCycle()
    {
        // 1 cycle is one generation
        while(true)
        {
            // selection
            auto betterHalf = selection();

            vector<CSolution> heir;

            // cross
            while(!betterHalf.empty())
            {
                uint first = rand() % (betterHalf.size());
                uint second = rand() % (population.size());

                //auto tmp = OX(betterHalf[first], population[second]);
                auto tmp = PMX(betterHalf[first], population[second]);

                // mutation
                tmp.first.mutationSwap();
                tmp.first.countFitness();
                tmp.second.mutationSwap();
                tmp.second.countFitness();

                betterHalf.erase(betterHalf.cbegin() + first);

                heir.push_back(tmp.first);
                heir.push_back(tmp.second);

                if (tmp.first.fitness == N_QUEENS)
                    return tmp.first;
                if (tmp.second.fitness == N_QUEENS)
                    return tmp.second;
            }

            // exchange
            population = heir;
        }
    }

    static CSolution OX_oneWay(const CSolution &first, const CSolution &second, uint length, uint startPosition)
    {
        int remaining = second.layout.size() - (length + startPosition);

        vector<int> copy(first.layout.begin() + startPosition, first.layout.begin() + startPosition + length);
        CSolution result;
        vector<int> remnant;

        uint index = length + startPosition;
        while(remnant.size() != remaining)
        {
            index = index % first.layout.size();

            if(isNotInVector(copy, second.layout[index]))
                remnant.push_back(second.layout[index]);

            index++;
        }

        vector<int> beginning;
        while(beginning.size() != startPosition)
        {
            index = index % first.layout.size();

            if(isNotInVector(copy, second.layout[index]))
                beginning.push_back(second.layout[index]);

            index++;
        }
        result.layout.insert(end(result.layout), begin(beginning), end(beginning));
        result.layout.insert(end(result.layout), begin(copy), end(copy));
        result.layout.insert(end(result.layout), begin(remnant), end(remnant));
        return  result;
    }

    static int getMapping(int element, const vector<int>& original, const vector<int>& withDuplicate)
    {
        uint position = withDuplicate.size();
        for(uint i = 0; i < withDuplicate.size(); i++)
        {
            if(withDuplicate[i] == element) {
                position = i;
                break;
            }
        }

        if(position == withDuplicate.size())
            return element;


        return getMapping(original[position], original, withDuplicate);
    }

    //  random substring (ideally half of NQUEENS) - remove content from second and fill the rest
    //  random substring (ideally half of NQUEENS) - remove content from second and fill the rest
    static pair<CSolution, CSolution> OX(const CSolution &first, const CSolution &second)
    {
        int length = N_QUEENS / 2;
        uint maxStart = first.layout.size() - length;
        uint startPosition = rand() % (maxStart);

//        cout << "start of switch : " << startPosition << endl;
        return make_pair(OX_oneWay(first, second, length, startPosition), OX_oneWay(second, first, length, startPosition));
    }

    static pair<CSolution, CSolution> PMX(const CSolution &first, const CSolution &second)
    {
        uint start = rand() % (first.layout.size());
        uint end = rand() % (first.layout.size());

        while(start == end)
            end = rand() % (first.layout.size());

        if(end < start)
        {
            auto tmp = end;
            end = start;
            start = tmp;
        }

        vector<int> copyFirst(first.layout.begin() + start, first.layout.begin() + end);
        vector<int> copySecond(second.layout.begin() + start, second.layout.begin() + end);

        CSolution sol1;
        CSolution sol2;

        // now create mapping rules
        for(uint i = 0; i < start; i++)
        {
            sol1.layout.push_back(getMapping(second.layout[i], copySecond, copyFirst));

            sol2.layout.push_back(getMapping(first.layout[i], copyFirst, copySecond));

        }

        sol1.layout.insert(sol1.layout.end(), copyFirst.begin(), copyFirst.end());
        sol2.layout.insert(sol2.layout.end(), copySecond.begin(), copySecond.end());

        for(uint i = end; i < first.layout.size(); i++)
        {
            sol1.layout.push_back(getMapping(second.layout[i], copySecond, copyFirst));

            sol2.layout.push_back(getMapping(first.layout[i], copyFirst, copySecond));

        }

        return make_pair(sol1, sol2);
    }

    vector<CSolution> selection()
    {
        sort(population.begin(), population.end());
        uint halfSize = population.size() / 2;
        vector<CSolution> first_half(population.begin(), population.begin() + halfSize);
        return first_half;
    }

};

void test()
{
    auto tmp = CNQueens();
    CSolution left;
    left.generateRandom();
    CSolution right;
    right.generateRandom();

    auto res = CNQueens::OX(left, right);
    cout << "left = " << left << endl;
    cout << "right = " << right << endl;
    cout << "leftRes = " <<  res.first << endl;
    cout << "rightRes = " <<  res.second << endl;
    cout << endl;

//    res = CNQueens::PMX(left, right);
//    cout << "left = " << left << endl;
//    cout << "right = " << right << endl;
//    cout << "leftRes = " <<  res.first << endl;
//    cout << "rightRes = " <<  res.second << endl;

//    res = CNQueens::PMX(left, left);
//    cout << "left = " << left << endl;
//    cout << "leftRes = " <<  res.first << endl;

    left.layout =  {3, 5, 1, 0, 4, 2, 7, 6};
    right.layout = {0, 7, 4, 3, 6, 1, 5, 2};
    auto meh = CNQueens::PMX(left, right);
    cout << meh.first << endl;
}

int main() {
    // test();

    auto tmp = CNQueens();
    auto result = tmp.genomCycle();
    cout << result << endl;

    return 0;
}

// 12 queens - took some time but result : [ 3 1 8 5 11 0 10 6 9 2 4 7 ]