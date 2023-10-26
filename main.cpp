#include <fstream>
#include <string>
#include <sstream>
#include "nonogram.hpp"

using namespace std;
using namespace Nonogram;

uint16_t input_arr[MAX_SIZE * MAX_SIZE];
ofstream *gofs;
clock_t inspectation_interval;

char printBinBit(const uint16_t v) {
    return v == 0b10 ? '#' : v == 0b01 ? '.' : v == 0b00 ? '?' : '!';
}
bool print(const Solver &o) {
    cout << o.getTime() << " seconds taken\n";
    for (int r{0}; r < input_arr[0]; r++) {
        for (int c{0}; c < input_arr[1]; c++) *gofs << printBinBit(o.get(r, c)) << ' ';
        *gofs << '\n';
    }
    *gofs << '\n';

    return 0;
}
void inspect(const Solver &o, const uint16_t vec, const uint16_t vi, const bool inference) {
    /* cout << (vol - o.getRemain()) * 100 / vol << '%' << '\r';
    if (inference) {
        for (int r{0}; r < input_arr[0]; r++) for (int c{0}; c < input_arr[1]; c++) output_table[r][c] = o.get(r, c);
    } else {
        if (vec) for (int i{}; i < input_arr[0]; i++) output_table[i][vi] = o.get(vec, vi, i);
        else for (int i{}; i < input_arr[1]; i++) output_table[vi][i] = o.get(vec, vi, i);
    }
    for (int r{0}; r < input_arr[0]; r++) {
        for (int c{0}; c < input_arr[1]; c++) cout << printBinBit(output_table[r][c]) << ' ';
        if (!vec && r == vi) cout << '<';
        cout << '\n';
    }
    for (int c{0}; c < input_arr[1]; c++) cout << (vec && c == vi ? '^' : ' ') << ' ';
    cout << '\n'; */
}

int main(int argc, char const *argv[]) {
    try {
        if (argc < 2) {
            throw runtime_error{"Usage: "s + string{argv[0]} + " <-i <input file>> [-o <output file>] [-ins <time intervals>]\n\n\t-i <input file>\t\tSpecify the path of the input file.\n\t-o <output file>\tSpecify the path of the output file, otherwise \".out\" is appended after the input path.\n\t-ins <time intervals>\tPrint the process of solving the nonogram at specified time intervals. (Unused option)\n"s};
        }

        string ifp, ofp;
        bool inspectation{};

        for (int i{1}; i < argc; i++) {
            if (argv[i][0] == '-')  {
                if (argv[i] == "-i"s) {
                    if (i + 1 < argc) {
                        if (ifp.empty()) {
                            ifp = argv[++i];
                        } else {
                            throw runtime_error{"Error: Multiple input files specified."s};
                        }
                    } else {
                        throw runtime_error{"Error: Missing input file path after -i option."s};
                    }
                } else if (argv[i] == "-o"s) {
                    if (i + 1 < argc) {
                        if (ofp.empty()) {
                            ofp = argv[++i];
                        } else {
                            throw runtime_error{"Error: Multiple output files specified."s};
                        }
                    } else {
                        throw runtime_error{"Error: Missing output file path after -o option."s};
                    }
                } else if (argv[i] == "-ins"s) {
                    if (i + 1 < argc) {
                        inspectation = 1;
                        inspectation_interval = stoi(argv[++i]);
                    } else {
                        throw runtime_error{"Error: Missing time intervals after -ins option."s};
                    }
                } else {
                    throw runtime_error{"Error: Unknown option: "s + argv[i]};
                }
            } else {
                throw runtime_error{"Error: Unknown option: "s + argv[i]};
            }
        }

        ifstream ifs{ifp};

        if (ifs.fail()) {
            throw runtime_error{"Error: Unable to open the input file."s};
        }

        ofstream ofs{ofp.empty() ? ofp = ifp + ".out" : ofp};

        if (ofs.fail()) {
            ifs.close();
            
            throw runtime_error{"Error: Unable to open the output file."s};
        }

        gofs = &ofs;

        try {
            int ii{};
            string buf;
            stringstream ls;

            getline(ifs, buf);
            ls = stringstream{buf};
            for (; ii < 2; ii++) ls >> input_arr[ii];

            for (int vec{0}; vec < 2; vec++) for (int vi{0}; vi < input_arr[vec]; vi++) {
                uint16_t &line_clue_size{input_arr[ii++]};

                getline(ifs, buf);
                ls = stringstream{buf};
                for (line_clue_size = 0; ls >> input_arr[ii]; line_clue_size++, ii++);
            }

            if (!Solver::solve(input_arr, print, inspectation ? inspect : nullptr)) {
                throw runtime_error{"Error: The clue is invalid or unsolvable."s};
            }
        } catch (exception &e) {
            ifs.close();
            ofs.close();
            
            throw e;
        }

        ifs.close();
        ofs.close();

        return 0;
    } catch (exception &e) {
        cerr << e.what() << endl;
    }

    return 1;
}
