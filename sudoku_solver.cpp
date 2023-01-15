#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <unordered_set>
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/algorithm/string.hpp>

constexpr int SUDOKU_SIZE = 9;

class SudokuCell {
    private:
        std::unordered_set<int> possibleValues;
    public:
        SudokuCell(std::unordered_set<int> possibleValuesStart) {
            possibleValues = possibleValuesStart;
        }

        bool is_known() {
            return possibleValues.size() == 1;
        }

        bool is_impossible() {
            return possibleValues.empty();
        }

        std::unordered_set<int> get_possibilities() {
            return possibleValues;
        }

        void remove_possibility(int value) {
            possibleValues.erase(value);
        }

        void set_possibilities(std::unordered_set<int> new_possibilities) {
            possibleValues = new_possibilities;
        }
};

// rows, columns or boxes
class SudokuContainer {
    private:
        std::vector<SudokuCell*> cells;
    public:
        SudokuContainer(std::vector<SudokuCell*> container_cells) {
            cells = container_cells;
        } 

        void updateCellPossibilities() {
            // update possibilities of every cell by removing digits that are already solved in another cell of the container
            std::vector<int> solved_digits {};
            for (auto cell: cells) {
                if (cell->is_known()) {
                    solved_digits.push_back(*cell->get_possibilities().begin());
                }
            }
            for (auto cell: cells) {
                if (!cell->is_known()) {
                    for (int digit: solved_digits) {
                        cell->remove_possibility(digit);
                    }
                }
            }

            // detect if there is only one place a digit can be in
            for (int digit=1; digit<SUDOKU_SIZE+1; digit++) {
                if (std::find(solved_digits.begin(), solved_digits.end(), digit) != solved_digits.end()) {
                    continue;
                }
                std::vector<SudokuCell*> cells_with_digit {};
                for (auto cell: cells) {
                    if (cell->get_possibilities().find(digit) != cell->get_possibilities().end()) {
                        cells_with_digit.push_back(cell);
                    }
                }
                if (cells_with_digit.size() == 1) {
                    std::unordered_set<int> single_digit_set {};
                    single_digit_set.insert(digit);
                    cells_with_digit[0]->set_possibilities(single_digit_set);
                }
            }
        }
};

class Sudoku {
    private:
        std::vector<std::vector<SudokuCell>> grid;

        std::vector<SudokuContainer> containers;
    public:
        Sudoku(std::string input) {
            std::vector<std::string> lines;
            boost::split(lines, input, [](char c){return c == '\n';});
            for (std::string line: lines) {
                std::vector<SudokuCell> row {};
                for (char c: line) {
                    std::unordered_set<int> cell_possibilities {};
                    if (std::isdigit(c)) {
                        cell_possibilities.insert(c - '0');
                    } else {
                        for (int i=1; i<SUDOKU_SIZE+1; i++) {
                            cell_possibilities.insert(i);
                        }
                    }
                    row.push_back(SudokuCell {cell_possibilities});
                }
                grid.push_back(row);
            }

            // columns
            for (int x=0; x<SUDOKU_SIZE; x++) {
                std::vector<SudokuCell*> column {};
                for (int y=0; y<SUDOKU_SIZE; y++) {
                    column.push_back(&grid[y][x]);
                }
                containers.push_back(SudokuContainer {column});
            }

            // rows
            for (int y=0; y<SUDOKU_SIZE; y++) {
                std::vector<SudokuCell*> row {};
                for (int x=0; x<SUDOKU_SIZE; x++) {
                    row.push_back(&grid[y][x]);
                }
                containers.push_back(SudokuContainer {row});
            }

            // boxes
            for (int bx = 0; bx < (SUDOKU_SIZE/3); bx++) {
                for (int by = 0; by < (SUDOKU_SIZE/3); by++) {
                    std::vector<SudokuCell*> box {};
                    for (int x=0; x < 3; x++) {
                        for (int y=0; y < 3; y++) {
                            box.push_back(&grid[by*3+y][bx*3+x]);
                        }
                    }
                    containers.push_back(SudokuContainer {box});
                }
            }
        }

        void solve()  {
            while (true) {
                for (SudokuContainer container: containers) {
                    container.updateCellPossibilities();
                }

                bool isSolved = true;
                for (auto row: grid) {
                    for (SudokuCell cell: row) {
                        if (!cell.is_known()) {
                            isSolved = false;
                        }
                    }
                }

                if (isSolved) {
                    break;
                }
            }

            print_result();
        }

        void print_result() {
            std::string output_string {};
            for (auto row: grid) {
                for (SudokuCell cell: row) {
                    std::cout << cell.get_possibilities().size() << std::endl;
                    output_string += (char) (*cell.get_possibilities().begin() + '0');
                }
                output_string += "\n";
            }
            std::cout << output_string << std::endl;
        }
};

int main() {
    std::ifstream t("sudoku_input.txt");
    std::string input((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
    Sudoku sudoku {input};
    sudoku.solve();
}
