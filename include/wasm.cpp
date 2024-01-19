#include <chrono>
#include <thread>
#include "nonogram.hpp"

using namespace std;
using namespace Nonogram;
typedef void (*js_v_func_type)(void);
typedef void (*js_viii_func_type)(ng_size_t, ng_size_t, bool);

Solver solver;
js_v_func_type js_wrap, js_fail;
js_viii_func_type js_inspect;
ng_size_t input[MAX_SIZE * MAX_SIZE], output[MAX_SIZE * MAX_SIZE];
uint64_t interval;

void sleep(uint64_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
bool wrap(void) {
    for (int r{0}; r < input[0]; r++) for (int c{0}; c < input[1]; c++) output[r * input[1] + c] = solver.get(r, c);
    js_wrap();

    return 0;
}
void inspect(const ng_size_t vec, const ng_size_t vi, const bool inference) {
    if (inference) for (int r{0}; r < input[0]; r++) for (int c{0}; c < input[1]; c++) output[r * input[1] + c] = solver.get(r, c);
    else for (int i{0}; i < input[!vec]; i++) output[(vec ? i : vi) * input[1] + (vec ? vi : i)] = solver.get(vec, vi, i);
    js_inspect(vec, vi, inference);
    sleep(interval);
}
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
        memset(output, 0, sizeof output);
        solver.setWrap(wrap);
        solver.setInspect(inspection ? inspect : nullptr);
        interval = js_interval;
        if (solver.solve(input)) wrap();
        else fail();
    }
};
