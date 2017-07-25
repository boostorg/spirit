#include "bench_run.hpp"
#include <nonius/benchmark.h++>

NONIUS_BENCHMARK(BOOST_PP_STRINGIZE(HOW_INC), [] {
    return bench_run();
});

#define NONIUS_RUNNER
#include <nonius/main.h++>

