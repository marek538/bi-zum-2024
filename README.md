# 1) Sudoku Solver (CSP Optimization)

## Project Description
A high-performance Sudoku solver implemented in C++ using Constraint Satisfaction Problem (CSP) techniques. The project focuses on optimizing brute-force search algorithms through intelligent heuristics.

## Core Features
* **CSP Approach:** Models Sudoku as a set of variables, domains, and constraints (no duplicates in rows, columns, or 3x3 grids).
* **MRV Heuristic:** Implements the "Minimum Remaining Values" strategy—prioritizing cells with the fewest valid options to prune the search tree early.
* **Efficient Pruning:** Dynamically updates domains of related cells after each move, drastically reducing the search space compared to standard backtracking.


# 2) Maze Solver & Pathfinding Visualizer

## Project Description
A C++ application designed to solve complex 2D mazes using various pathfinding algorithms. The project features a console-based visualizer that displays the search process in real-time.

## Implemented Algorithms
* **BFS (Breadth-First Search):** Guarantees the shortest path by exploring all possible branches level-by-level.
* **DFS (Depth-First Search):** Efficiently explores deep branches, suitable for specific maze structures.
* **A* (A-Star):** An optimized pathfinding approach using heuristics to significantly reduce the search space.
* **RNDS (Randomized Search):** Demonstrates non-deterministic pathfinding behavior.

# 3) N-Queens Problem: Genetic Algorithm Solver

## Project Description
A C++ implementation of a genetic blackbox algorithm designed to solve the classic N-Queens puzzle. The project demonstrates evolutionary computation techniques to find optimal configurations where no two queens threaten each other on an $N \times N$ chessboard.

## Key Features
* **Fitness Evaluation:** Custom heuristic function that evaluates the board state by checking diagonal conflicts.
* **Crossover Operators:** Implements `OX` (Order Crossover) and `PMX` (Partially Mapped Crossover) to effectively combine solutions.
* **Evolutionary Strategy:** Uses population-based search with selection, mutation, and crossover to navigate the search
