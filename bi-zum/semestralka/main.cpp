#include <iostream>
#include <vector>
#include <map>

#define DEBUG 0

// 0 == going cell by cell and filling it
// 1 == going by domains and filling according cells
#define SOLVE_BY_DOMAIN 1

#define MINIMUM_REMAINING_VALUES 1


using namespace std;

// all squares are filled
bool valid_solution(vector<vector<int>> sudoku)
{
    for(auto & i : sudoku)
    {
        for(int j : i)
        {
            if(j == 0 || j > 9)
                return false;
        }
    }
    return true;
}


vector<vector<int>> initialize_sudoku() {
    return {
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
}

class CSPSudoku
{
    map<pair<int, int>, vector<int>> m_domain;

public:
    CSPSudoku(vector<vector<int>> initial_sudoku) : m_sudoku(initial_sudoku)
    {
        if (DEBUG != 1)
            check_correct_input();

        for(int i = 0; i < m_sudoku.size(); i++)
        {
            for(int j = 0; j < m_sudoku[i].size(); j++)
            {
                if(m_sudoku[i][j] == 0)
                    m_domain[{i, j}] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            }
        }

        for (int i = 0; i < m_sudoku.size(); ++i)
        {
            for (int j = 0; j < m_sudoku[i].size(); ++j)
            {
                if (m_sudoku[i][j] != 0)
                    update_domain(i, j, m_sudoku[i][j], m_domain);
            }
        }

        cout << "sudoku initialized" << endl;
    }

    void check_correct_input()
    {
        if(m_sudoku.size() != 9)
            throw invalid_argument("The input sudoku must have 9 rows");

        for(const auto & i : m_sudoku)
        {
            if(i.size() != 9)
                throw invalid_argument("The input sudoku must have 9 columns");
        }

        for(auto & i : m_sudoku)
        {
            for(int j : i)
            {
                if(j < 0 || j > 9)
                    throw invalid_argument("The input sudoku must have values between 0 and 9");
            }
        }
    }

    // updates all constraints and removes domain when empty
    void update_domain(int i, int j, int value, map<pair<int, int>, vector<int>>& domain)
    {
        for(int k = 0; k < m_sudoku.size(); k++)
        {
            if(domain.count(make_pair(k, j)) > 0) 
            {
                auto it = lower_bound(domain[make_pair(k, j)].begin(), domain[make_pair(k, j)].end(), value);
                if(it != domain[make_pair(k, j)].end() && *it == value)
                    domain[make_pair(k, j)].erase(it);

                if(domain[make_pair(k, j)].empty())
                    domain.erase(make_pair(k, j));
                
            }

            if(domain.count(make_pair(i, k)) > 0)
            {
                auto it = lower_bound(domain[make_pair(i, k)].begin(), domain[make_pair(i, k)].end(), value);
                if(it != domain[make_pair(i, k)].end() && *it == value)
                    domain[make_pair(i, k)].erase(it);
                
                if(domain[make_pair(i, k)].empty())
                    domain.erase(make_pair(i, k));
            }
        }

        int row = i / 3;
        int col = j / 3;

        for(int k = row * 3; k < row * 3 + 3; k++)
        {
            for(int l = col * 3; l < col * 3 + 3; l++)
            {
                if(domain.count(make_pair(k, l)) > 0)
                {
                    auto it = lower_bound(domain[make_pair(k, l)].begin(), domain[make_pair(k, l)].end(), value);
                    if(it != domain[make_pair(k, l)].end() && *it == value)
                        domain[make_pair(k, l)].erase(it);

                    if(domain[make_pair(k, l)].empty())
                        domain.erase(make_pair(k, l));
                }
            }
        }
    }

    void solve_sudoku()
    {
        bool solution_found = false;
        solution_recursive(m_sudoku, m_domain, solution_found);
        if(!solution_found)
            cout << "No solution found" << endl;
        else
            cout << "Solution found" << endl;
    }


#if SOLVE_BY_DOMAIN != 1
    // going by the real sudoku and the first 0 found gets filled
    void solution_recursive(vector<vector<int>> sudoku, map<pair<int, int>, vector<int>> domain, bool &solution_found)
    {
        if(solution_found)
            return;

        bool filled = true;

        for(int row = 0; row < sudoku.size(); row++)
        {
            for(int cell = 0; cell < sudoku[row].size(); cell++)
            {
                if(sudoku[row][cell] == 0)
                {
                    for(auto value: domain[make_pair(row, cell)])
                    {
                        vector<vector<int>> new_sudoku = sudoku;
                        map<pair<int, int>, vector<int>> new_domain = domain;
                        new_domain.erase(make_pair(row, cell));

                        new_sudoku[row][cell] = value;
                        update_domain(row, value, value, new_domain);

                        solution_recursive(new_sudoku, new_domain, solution_found);

                    }

                    filled = false;
                    break;
                }
            }
        }

        if(filled)
        {
            solution_found = valid_solution(sudoku);
            if(solution_found)
                m_sudoku = sudoku;


            return;
        }
    }

#else
    //
    void solution_recursive(vector<vector<int>> sudoku, map<pair<int, int>, vector<int>> domain, bool &solution_found)
    {
        if(solution_found)
            return;

        if (DEBUG == 1) {
            cout << "\033[2J\033[H";
            m_sudoku = sudoku;
            print();
            cout << endl;
        }

        if(domain.empty())
        {
            solution_found = valid_solution(sudoku);
            if(solution_found)
                m_sudoku = sudoku;

            if(DEBUG == 1)
                cout << "domain empty\n" << endl;

            return;
        }

        // minimum-remaining-values heuristic
        auto smallestDomainIt = domain.begin();

#if MINIMUM_REMAINING_VALUES == 1
        for(auto it = domain.begin(); it != domain.end(); it++)
        {
            if(it->second.size() < smallestDomainIt->second.size())
                smallestDomainIt = it;
        }
#endif
        for(int value : smallestDomainIt->second)
        {
            vector<vector<int>> new_sudoku = sudoku;

            // REMOVE ALREADY FILLED DOMAIN
            map<pair<int, int>, vector<int>> new_domain = domain;
            new_domain.erase(smallestDomainIt->first);

            new_sudoku[smallestDomainIt->first.first][smallestDomainIt->first.second] = value;
            update_domain(smallestDomainIt->first.first, smallestDomainIt->first.second, value, new_domain);

            solution_recursive(new_sudoku, new_domain, solution_found);
        }

    }
#endif
    void print()
    {
        for(auto & i : m_sudoku)
        {
            for(int j : i)
                cout << j << " ";
            cout << endl;
        }
    }

    vector<vector<int>> m_sudoku;
};


int main()
{
    vector<vector<int>> initial_sudoku = initialize_sudoku();

    CSPSudoku sudoku(initial_sudoku);
    sudoku.solve_sudoku();

    sudoku.print();
    return 0;
}
