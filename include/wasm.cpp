#include <chrono>
#include <thread>
#include "nonogram.hpp"

using namespace std;
using namespace Nonogram;
typedef void (*js_v_func_type)(void);
typedef void (*js_viii_func_type)(ng_size_t, ng_size_t, bool);

js_v_func_type js_wrap, js_fail;
js_viii_func_type js_inspect;
ng_size_t input[MAX_SIZE * MAX_SIZE], output[MAX_SIZE * MAX_SIZE];
bool do_inspection;
uint64_t interval;
class MySolver : public Solver {
public:
    bool wrap(void) {
        for (int r{0}; r < input[0]; r++) for (int c{0}; c < input[1]; c++) output[r * input[1] + c] = get(r, c);
        js_wrap();

        return false;
    }
    bool inspect(const ng_size_t vec, const ng_size_t vi, const bool inference) {
        if (do_inspection) {
            if (inference) for (int r{0}; r < input[0]; r++) for (int c{0}; c < input[1]; c++) output[r * input[1] + c] = get(r, c);
            else for (int i{0}; i < input[!vec]; i++) output[(vec ? i : vi) * input[1] + (vec ? vi : i)] = get(vec, vi, i);
            js_inspect(vec, vi, inference);
            this_thread::sleep_for(chrono::milliseconds(interval));
        }

        return true;
    }
} solver;
void fail(void) {
    js_fail();
}

extern "C" {
    uint16_t *getInputPtr(void) {
        return input;
    }
    uint16_t *getOutputPtr(void) {
        return output;
    }
    void setWrap(uint16_t js_wrap_int) {
        js_wrap = (js_v_func_type)js_wrap_int;
    }
    void setInspect(uint16_t js_inspect_int) {
        js_inspect = (js_viii_func_type)js_inspect_int;
    }
    void setFail(uint16_t js_fail_int) {
        js_fail = (js_v_func_type)js_fail_int;
    }
    ng_sq_size_t getRemain(void) {
        return solver.getRemain();
    }
    ng_size_t getCount(void) {
        return solver.getCount();
    }
    double getTime(void) {
        return solver.getTime();
    }
    void solve(bool inspection, uint64_t js_interval) {
        do_inspection = inspection;
        interval = js_interval;
        if (solver.solve(input)) solver.wrap();
        else fail();
    }
};
