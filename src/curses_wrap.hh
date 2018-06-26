#ifndef CURSES_WRAP_HH
#define CURSES_WRAP_HH

#include <string>

namespace curses {
        

    // TODO Colors could be more generalized like this but I'm not sure if its
    // worth it.
    enum Colors {
        // COLOR_PAIR from ncurses source translates to this:
        // COLOR_PAIR(color_number) == static_cast<unsigned>(color_number) << 8 & A_COLOR
        // where A_COLOR == static_cast<unsigned>(((1U << 8) - 1U)) << 8
        RedBlack =   1u << 8 & 0b1111111100000000,
        GreenBlack = 2u << 8 & 0b1111111100000000,
        RedRed =     3u << 8 & 0b1111111100000000,
    };

    void initialize();

    bool is_backspace(const int ch);
    bool is_enter(const int ch);

    std::pair<int, int> get_mid(int y_offset, int x_offset);

    // print somewhere in the screen and return the cursor to the original
    // position print_no_move
    void printnm(const int y, const int x, const std::string_view& str);

    // print and go to position from which you started printing
    void print_begin(const int y, const int x, const std::string_view& str);

} /* curses */ 

#endif /* ifndef CURSES_WRAP_HH */