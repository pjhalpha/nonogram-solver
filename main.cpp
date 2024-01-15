#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdexcept>
#include "nonogram.hpp"

using namespace std;
using namespace Nonogram;

class NGException : public runtime_error {
public:
    NGException(string message) : runtime_error{message} {}
};

Solver solver;
ng_size_t input_arr[MAX_SIZE * MAX_SIZE];
ng_size_t output_table[MAX_SIZE][MAX_SIZE];
fstream ifs, ofs;
uint64_t mx_cnt{1};

char printBinBit(const ng_size_t v) {
    return v == 0b10 ? '#' : v == 0b01 ? '.' : v == 0b00 ? '?' : '!';
}
bool wrap(void) {
    for (int r{}; r < input_arr[0]; ++r) {
        for (int c{}; c < input_arr[1]; ++c) ofs << printBinBit(solver.get(r, c)) << ' ';
        ofs << '\n';
    }
    ofs << '\n';

    return --mx_cnt;
}
void inspect(const ng_size_t vec, const ng_size_t vi, const bool is_inference) {
    if (is_inference) {
        for (int r{}; r < input_arr[0]; ++r) for (int c{}; c < input_arr[1]; ++c) output_table[r][c] = solver.get(r, c);
    } else {
        if (vec) for (int i{}; i < input_arr[0]; ++i) output_table[i][vi] = solver.get(vec, vi, i);
        else for (int i{}; i < input_arr[1]; ++i) output_table[vi][i] = solver.get(vec, vi, i);
    }
    for (int r{}; r < input_arr[0]; ++r) {
        for (int c{}; c < input_arr[1]; ++c) cout << printBinBit(output_table[r][c]) << ' ';
        if (!vec && r == vi) cout << '<';
        cout << '\n';
    }
    for (int c{}; c < input_arr[1]; ++c) cout << (vec && c == vi ? '^' : ' ') << ' ';
    cout << '\n';
}

int main(int argc, char const *argv[]) {
    string ifp, ofp, buf;
    stringstream ls;

    try {
        if (argc < 2) throw NGException{"Usage: "s + argv[0] + " <-i <input file>> [-o <output file>] [-m <max count>]\n\n\t-i <input file>\n\t\tSpecify the path of the input file.\n\n\t-o <output file>\n\t\tSpecify the path of the output file.\n\t\tOtherwise, \".out\" is appended after the input path.\n\n\t-m <maximum count>\n\t\tSpecify the maximum count of solutions to be found.\n\t\tSet this option as negative value to find all solutions.\n\t\tOtherwise, only one solution could be found.\n"s};

        for (int i{1}; i < argc; ++i) {
            if (argv[i][0] == '-')  {
                if (argv[i] == "-i"s) {
                    if (i + 1 < argc) {
                        ifp = argv[++i];
                    } else throw NGException{"Error: Missing the input file path after -i option."s};
                } else if (argv[i] == "-o"s) {
                    if (i + 1 < argc) {
                        ofp = argv[++i];
                    } else throw NGException{"Error: Missing the output file path after -o option."s};
                } else if (argv[i] == "-m"s) {
                    if (i + 1 < argc) {
                        mx_cnt = stoi(argv[++i]);
                    } else throw NGException{"Error: Missing the maximum count of solutions after -m option."s};
                } else throw NGException{"Error: Unknown option: "s + argv[i]};
            } else throw NGException{"Error: Unknown option: "s + argv[i]};
        }
        
        ifs.open(ifp, ios::in);

        if (ifs.fail()) throw NGException{"Error: Unable to open the input file."s};

        ofs.open(ofp.empty() ? ofp = ifp + ".out" : ofp, ios::out);

        if (ofs.fail()) throw NGException{"Error: Unable to open the output file."s};

        getline(ifs, buf);
        ls = stringstream{buf};
        for (int i{}; i < 2; ++i) ls >> input_arr[i];

        for (int i{2}, vec{}; vec < 2; ++vec) for (int vi{}; vi < input_arr[vec]; ++vi) {
            if (i >= MAX_SIZE * MAX_SIZE) throw NGException{"Error: The clue is invalid or unsolvable."s};
            
            ng_size_t &line_clue_size{input_arr[i++]};

            getline(ifs, buf);
            ls = stringstream{buf};
            for (line_clue_size = 0; i < MAX_SIZE * MAX_SIZE && ls >> input_arr[i]; ++line_clue_size, ++i);
        }

        solver.setWrap(wrap);

        if (!solver.solve(input_arr)) throw NGException{"Error: The clue is invalid or unsolvable."s};

        cout << solver.getCount() << " solution(s) found in " << fixed << showpoint << solver.getTime() << " seconds" << endl;

        ifs.close();
        ofs.close();

        return 0;
    } catch (NGException &e) {
        cerr << e.what() << endl;
    } catch (...) {
        cerr << "Error: A fatal error has occurred."s << endl;
    }
    
    ifs.close();
    ofs.close();

    return 1;
}
