#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <string_view>
#include <unordered_set>

#include <QString>

// from: http://en.cppreference.com/w/cpp/types/numeric_limits/epsilon
template <class T>
typename std::enable_if_t<!std::numeric_limits<T>::is_integer, bool>
almost_equal(T x, T y, int ulp) {
    // the machine epsilon has to be scaled to the magnitude of the values used
    // and multiplied by the desired precision in ULPs (units in the last place)
    return std::abs(x - y)
                   <= std::numeric_limits<T>::epsilon() * std::abs(x + y) * ulp
           // unless the result is subnormal
           || std::abs(x - y) < std::numeric_limits<T>::min();
}

template <typename Seq> auto choice(Seq &sequence) {
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, std::size(sequence) - 1);

    auto it = std::begin(sequence);
    std::advance(it, dis(gen));
    return it;
}

template <typename Seq, typename W> auto choice(Seq &sequence, W &weights) {

    static_assert(std::is_floating_point_v<typename W::value_type>);
    assert(std::size(sequence) == std::size(weights));
    assert(std::size(sequence) != 0);

    std::mt19937 gen(std::random_device{}());
    auto idx = std::discrete_distribution<>{std::begin(weights),
                                            std::end(weights)}(gen);

    auto it = std::begin(sequence);
    std::advance(it, idx);
    return it;
}

template <typename F, typename S> auto zip(const F &lhs, const S &rhs) {
    // TODO This should be implemented without coping i.e. only over
    // iterators...
    std::vector<std::pair<typename F::value_type, typename S::value_type>>
            zipped;
    zipped.reserve(std::size(lhs));

    auto lhs_iter = std::begin(lhs);
    auto rhs_iter = std::begin(rhs);
    while (lhs_iter != std::end(lhs) || rhs_iter != std::end(rhs))
        zipped.emplace_back(*lhs_iter++, *rhs_iter++);

    return zipped;
}

template <typename Seq, typename Number>
void invert_values(Seq &seq, Number max_value) {
    static_assert(std::is_arithmetic_v<Number>);
    static_assert(std::is_arithmetic_v<typename Seq::value_type>);
    for (auto &el : seq)
        el = max_value - el;
}

template <typename T, typename E> bool is_in(const T &container, const E &el) {
    return std::end(container)
           != std::find(std::begin(container), std::end(container), el);
}

// possibly use this to define different resolution but it is unneeded atm
// template <class Units>
class Timer {
    using resolution_t = std::chrono::milliseconds;
    using clock = std::chrono::high_resolution_clock;
    clock::time_point t;

public:
    auto start() { t = clock::now(); }
    auto duration() {
        auto end = clock::now();
        return std::chrono::duration_cast<resolution_t>(end - t).count();
    }
};

// std::set does this...
template <typename T> auto sort_uniq(T &container) {
    // sortable, erasable concept would be nice
    std::sort(container.begin(), container.end());
    auto last = std::unique(container.begin(), container.end());
    container.erase(last, container.end());
}

inline auto sort_uniq(QString &container) {
    std::sort(container.begin(), container.end());
    auto last = std::unique(container.begin(), container.end());
    container.chop(std::distance(last, container.end()));
}

// only ascii...
inline auto count_chars(std::string_view file_path) {

    std::string_view ascii_str =
            R"(qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM`~!@#$%^&*()-_=+{[]};:'"\|,<.>/?0123456789)";
    std::unordered_set<char> ascii{ascii_str.begin(), ascii_str.end()};

    std::ifstream file{file_path.data()};
    // perhaps this should not break the whole program?
    if (!file.is_open()) {
        std::clog << "Unable to open the file: " << file_path;
        exit(1);
    }

    // TODO horrible solution to get frequencies between 0 and 1 but I'm sleepy
    std::map<char, size_t> counter;
    size_t tot{};
    char ch;
    while (file >> ch) {
        if (ascii.count(ch)) {
            ++tot;
            ++counter[ch];
        }
    }
    std::map<char, double> freq;
    double total = tot;
    for (auto &&[ch, val] : counter)
        freq[ch] = val / total;

    return freq;
}

#endif /* UTILS_H */
