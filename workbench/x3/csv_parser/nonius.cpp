#include <nonius/benchmark.h++>
#include "input.hpp"
#include BOOST_PP_STRINGIZE(BENCH_INC)

NONIUS_BENCHMARK(BOOST_PP_STRINGIZE(BENCH_INC), [] {
    return bench(bench_parser);
});

#define NONIUS_RUNNER
#include <nonius/main.h++>

