#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "nonogram.hpp"

using namespace std::literals;

class MySolver : public Nonogram::Solver {
public:
    MySolver(void) = default;
    MySolver(const MySolver&) = delete;
    MySolver &operator=(const MySolver&) = delete;
    ~MySolver(void) {
        ifs_.close();
        ofs_.close();
    }
    
    void run(const std::string &ifp, const std::string &ofp, std::uint64_t max_count) {
        max_count_ = max_count;
        ifs_.open(ifp, std::ios::in);

        if (ifs_.fail()) throw std::runtime_error{"Error: Unable to open the input file."s};

        ofs_.open(ofp, std::ios::out);

        if (ofs_.fail()) throw std::runtime_error{"Error: Unable to open the output file."s};

        std::getline(ifs_, buf_);
        ls_ = std::stringstream{buf_};
        for (int i{0}; i < 2; ++i) {
            ls_ >> input_arr_[i];
        }

        for (int i{2}, vec{0}; vec < 2; ++vec) {
            for (int vec_i{0}; vec_i < input_arr_[vec]; ++vec_i) {
                if (i >= Nonogram::MAX_SIZE * Nonogram::MAX_SIZE) throw std::runtime_error{"Error: The clue is invalid or unsolvable."s};
                
                Nonogram::ng_size_t &line_clue_size{input_arr_[i++]};

                std::getline(ifs_, buf_);
                ls_ = std::stringstream{buf_};
                for (line_clue_size = 0; i < Nonogram::MAX_SIZE * Nonogram::MAX_SIZE && ls_ >> input_arr_[i]; ++line_clue_size, ++i);
            }
        }

        if (!clue_.init(input_arr_)) throw std::runtime_error{"Error: The clue is invalid."s};
        if (!solve(clue_)) throw std::runtime_error{"Error: The clue is unsolvable."s};

        std::cout << getCount() << " solution(s) found in " << std::fixed << std::showpoint << getTime() << " seconds" << std::endl;
        
        ifs_.close();
        ofs_.close();
    }

private:
    std::fstream ifs_, ofs_;
    std::stringstream ls_;
    std::string buf_;
    Nonogram::ng_size_t input_arr_[Nonogram::MAX_SIZE * Nonogram::MAX_SIZE], output_table_[Nonogram::MAX_SIZE][Nonogram::MAX_SIZE];
    uint64_t max_count_;
    Nonogram::Clue clue_;

    char printBinBit(const Nonogram::ng_size_t val) {
        return (val == 0b10) ? '#' : (val == 0b01) ? '.' : (val == 0b00) ? '?' : '!';
    }
    bool wrap(void) {
        for (int row{0}; row < input_arr_[0]; ++row) {
            for (int col{0}; col < input_arr_[1]; ++col) {
                ofs_ << printBinBit(get(row, col)) << ' ';
            }
            ofs_ << '\n';
        }
        ofs_ << '\n';

        return --max_count_;
    }
    bool inspect(const bool vec, const Nonogram::ng_size_t vec_i, const bool is_inference) {
        // if (is_inference) {
        //     for (int row{0}; row < input_arr_[0]; ++row) {
        //         for (int col{0}; col < input_arr_[1]; ++col) {
        //             output_table_[row][col] = get(row, col);
        //         }
        //     }
        // } else {
        //     if (vec) {
        //         for (int i{0}; i < input_arr_[0]; ++i) {
        //             output_table_[i][vec_i] = get(vec, vec_i, i);
        //         }
        //     } else {
        //         for (int i{0}; i < input_arr_[1]; ++i) {
        //             output_table_[vec_i][i] = get(vec, vec_i, i);
        //         }
        //     }
        // }
        // for (int row{0}; row < input_arr_[0]; ++row) {
        //     for (int col{0}; col < input_arr_[1]; ++col) {
        //         std::cout << printBinBit(output_table_[row][col]) << ' ';
        //     }
        //     if (!vec && row == vec_i) {
        //         std::cout << '<';
        //     }
        //     std::cout << '\n';
        // }
        // for (int col{0}; col < input_arr_[1]; ++col) {
        //     std::cout << ((vec && col == vec_i) ? '^' : ' ') << ' ';
        // }
        // std::cout << '\n';

        return true;
    }
};

static MySolver solver;

int main(int argc, char const *argv[]) {
    std::string ifp{"input.txt"}, ofp{"output.txt"};
    uint64_t max_count{};
    
    try {
        if (argc < 2) throw std::runtime_error{"Usage: "s + argv[0] + " <-i <input file>> [-o <output file>] [-m <max count>]\n\n\t-i <input file>\n\t\tSpecify the path of the input file.\n\n\t-o <output file>\n\t\tSpecify the path of the output file.\n\t\tOtherwise, \".out\" is appended after the input path.\n\n\t-m <maximum count>\n\t\tSpecify the maximum count of solutions to be found.\n\t\tSet this option as negative value to find all solutions.\n\t\tOtherwise, only one solution could be found.\n"s};

        for (int arg_i{1}; arg_i < argc; ++arg_i) {
            if (argv[arg_i][0] == '-')  {
                if (argv[arg_i] == "-i"s) {
                    if (arg_i + 1 < argc) {
                        ifp = argv[++arg_i];
                    } else throw std::runtime_error{"Error: Missing the input file path after -i option."s};
                } else if (argv[arg_i] == "-o"s) {
                    if (arg_i + 1 < argc) {
                        ofp = argv[++arg_i];
                    } else throw std::runtime_error{"Error: Missing the output file path after -o option."s};
                } else if (argv[arg_i] == "-m"s) {
                    if (arg_i + 1 < argc) {
                        max_count = std::stoi(argv[++arg_i]);
                    } else throw std::runtime_error{"Error: Missing the maximum count of solutions after -m option."s};
                } else throw std::runtime_error{"Error: Unknown option: "s + argv[arg_i]};
            } else throw std::runtime_error{"Error: Unknown option: "s + argv[arg_i]};
        }

        solver.run(ifp, ofp.empty() ? ofp = ifp + ".out" : ofp, max_count);

        return 0;
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Error: A fatal error has occurred."s << std::endl;
    }

    return 1;
}
