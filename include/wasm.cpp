#include <chrono>
#include <thread>
#include "nonogram.hpp"

using js_v_func_t = void (*)(void);
using js_viii_func_t = void (*)(Nonogram::ng_size_t, Nonogram::ng_size_t, bool);

class MySolver : public Nonogram::Solver {
public:
    Nonogram::ng_size_t input_[Nonogram::MAX_SIZE * Nonogram::MAX_SIZE];
    Nonogram::ng_size_t output_[Nonogram::MAX_SIZE * Nonogram::MAX_SIZE];
    bool do_inspection_;
    uint64_t interval_;
    js_v_func_t js_wrap_;
    js_viii_func_t js_inspect_;
    js_v_func_t js_fail_;

public:
    bool wrap(void) {
        for (int row{0}; row < input_[0]; row++) {
            for (int col{0}; col < input_[1]; col++) {
                output_[row * input_[1] + col] = get(row, col);
            }
        }
        js_wrap_();

        return false;
    }
    bool inspect(const bool vec, const Nonogram::ng_size_t vec_i, const bool inference) {
        if (do_inspection_) {
            if (inference) {
                for (int row{0}; row < input_[0]; row++) {
                    for (int col{0}; col < input_[1]; col++) {
                        output_[row * input_[1] + col] = get(row, col);
                    }
                }
            } else {
                for (int i{0}; i < input_[!vec]; i++) {
                    output_[(vec ? i : vec_i) * input_[1] + (vec ? vec_i : i)] = get(vec, vec_i, i);
                }
            }
            js_inspect_(vec, vec_i, inference);
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_));
        }

        return true;
    }
    void fail(void) {
        js_fail_();
    }
} solver;

extern "C" {
    uint16_t *getInputPtr(void) {
        return solver.input_;
    }
    uint16_t *getOutputPtr(void) {
        return solver.output_;
    }
    void setWrap(uint16_t js_wrap_int) {
        solver.js_wrap_ = (js_v_func_t)js_wrap_int;
    }
    void setInspect(uint16_t js_inspect_int) {
        solver.js_inspect_ = (js_viii_func_t)js_inspect_int;
    }
    void setFail(uint16_t js_fail_int) {
        solver.js_fail_ = (js_v_func_t)js_fail_int;
    }
    Nonogram::ng_sq_size_t getRemain(void) {
        return solver.getRemain();
    }
    Nonogram::ng_size_t getCount(void) {
        return solver.getCount();
    }
    double getTime(void) {
        return solver.getTime();
    }
    void solve(bool inspection, uint64_t js_interval) {
        solver.do_inspection_ = inspection;
        solver.interval_ = js_interval;
        if (solver.solve(solver.input_)) {
            solver.wrap();
        } else {
            solver.fail();
        }
    }
};
