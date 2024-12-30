#include "line_clue.hpp"

namespace Nonogram {
    ng_size_t LineClue::getLineSize(void) const {
        return line_size_;
    }
    ng_size_t LineClue::getSize(void) const {
        return size_;
    }
    ng_size_t LineClue::getOffset(const ng_size_t cele_i) const {
        return offset_[cele_i];
    }
    ng_size_t LineClue::getMargin(void) const {
        return margin_;
    }
    bool LineClue::init(const ng_size_t line_size, const ng_size_t size, const ng_size_t* const clue_arr) {
        line_size_ = line_size;
        size_ = size;

        if (line_size_ == 0 || line_size_ >= MAX_CLUE_SIZE || size_ > line_size_ + 1 >> 1) {
            return false;
        }

        offset_[0] = 0;
        std::memcpy(offset_ + 1, clue_arr, sizeof(ng_size_t) * size_);
        for (ng_size_t cele_i{0}; cele_i < size_; ++cele_i) {
            offset_[cele_i + 1] += offset_[cele_i] + 1;
        }
        margin_ = line_size_ - offset_[size_] + 1;
    
        if (margin_ > line_size_) {
            return false;
        }
        
        return true;
    }
};
