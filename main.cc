#include <vector>
#include <iterator>
#include <iostream>
#include <ncurses.h>
#include "generate.hh"
#include "stats.hh"
#include "probability_matrix.hh"

std::string typed = "";
int row, col;
int y = -1, x = -1;
int mid_y, mid_x;
int ch;

//int main(int argc, char *argv[])
int main()
{
    std::string lowercase = "qwertyuiopasdfghjklzxcvbnm";
    std::string uppercase = "QWERTYUIOPASDFGHJKLZXCVBNM";
    std::string symbols = "`~!@#$%^&*()-_=+{[]};:'\"\\|,<.>/?";
    std::string numbers = "0123456789";

    std::string short_chars = "asdfghjkl";
    std::string sentence = generate(short_chars, 40);
    ProbabilityMatrix m(short_chars);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, 1);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_RED);

    printw(m.to_string().c_str());

    getmaxyx(stdscr, row, col);
    mid_x = (col - std::size(sentence))/2;
    mid_y = row/2;

    mvprintw(mid_y, mid_x, "%s", sentence.c_str());
    move(mid_y, mid_x);

    int score{};

    while(typed.size() != std::size(sentence)){
        getyx(stdscr, y, x);
        ch = getch();

        if (ch == KEY_BACKSPACE || ch == '\b' || ch == 127 || ch == KEY_DC) {
            if (x != mid_x){
                move(y, --x);
                addch(sentence[x - mid_x]);
                move(y, x);
                typed.pop_back();
            }
        } else if (sentence[x - mid_x] == ch) {
            addch(ch | COLOR_PAIR(2));
            typed.push_back(ch);
        } else {
            if (ch == ' ') 
                addch(ch | COLOR_PAIR(3));
            else 
                addch(ch | COLOR_PAIR(1));
            typed.push_back(ch);
        }

        // prob matrix update
        if (typed.length() > 1){ // update only if we have a predecessor
            bool correct = last_char_correct(typed, sentence);
            // ignore space for now
            int pos = typed.length() - 1;
            char current = typed[pos];
            char last = sentence[pos - 1];
            m.update_element(last, current, correct);
        }

        getyx(stdscr, y, x);
        score = missed_characters(typed, sentence);
        move(LINES - 4, 2);
        clrtoeol();
        printw("Errors: %i", score);
        move(LINES - 3, 2);
        clrtoeol();
        printw("Typed: %s", typed.c_str());
        mvprintw(0, 0, m.to_string().c_str());
        move(y, x);
    }
    //refresh();
    while(getch() != KEY_F(1)){ }
    endwin();
    return 0;
}
