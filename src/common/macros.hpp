#define let const auto
#define mutant auto
#define def auto

#define VA_NUM_ARGS(...)                                                       \
  VA_NUM_ARGS_IMPL(__VA_ARGS__, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,    \
                   14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define VA_NUM_ARGS_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12,    \
                         _13, _14, _15, _16, _17, _18, _19, _20, _21, _22,     \
                         _23, _24, _25, N, ...) N
#define macro_dispatcher(func, ...)                                            \
  macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs) macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func##nargs

#define to_tuplestr(...) macro_dispatcher(to_tuplestr, __VA_ARGS__)(__VA_ARGS__)
#define to_tuplestr1(a) std::tuple{#a, a}
#define to_tuplestr2(a, b) std::tuple{#a, a}, to_tuplestr1(b)
#define to_tuplestr3(a, ...) to_tuplestr1(a), to_tuplestr2(__VA_ARGS__)
#define to_tuplestr4(a, ...) to_tuplestr1(a), to_tuplestr3(__VA_ARGS__)
#define to_tuplestr5(a, ...) to_tuplestr1(a), to_tuplestr4(__VA_ARGS__)
#define to_tuplestr6(a, ...) to_tuplestr1(a), to_tuplestr5(__VA_ARGS__)
#define to_tuplestr7(a, ...) to_tuplestr1(a), to_tuplestr6(__VA_ARGS__)
#define to_tuplestr8(a, ...) to_tuplestr1(a), to_tuplestr7(__VA_ARGS__)
#define to_tuplestr9(a, ...) to_tuplestr1(a), to_tuplestr8(__VA_ARGS__)
#define to_tuplestr10(a, ...) to_tuplestr1(a), to_tuplestr9(__VA_ARGS__)
#define to_tuplestr11(a, ...) to_tuplestr1(a), to_tuplestr10(__VA_ARGS__)
#define to_tuplestr12(a, ...) to_tuplestr1(a), to_tuplestr11(__VA_ARGS__)
#define to_tuplestr13(a, ...) to_tuplestr1(a), to_tuplestr12(__VA_ARGS__)
#define to_tuplestr14(a, ...) to_tuplestr1(a), to_tuplestr13(__VA_ARGS__)
#define to_tuplestr15(a, ...) to_tuplestr1(a), to_tuplestr14(__VA_ARGS__)
#define to_tuplestr16(a, ...) to_tuplestr1(a), to_tuplestr15(__VA_ARGS__)
#define to_tuplestr17(a, ...) to_tuplestr1(a), to_tuplestr16(__VA_ARGS__)
#define to_tuplestr18(a, ...) to_tuplestr1(a), to_tuplestr17(__VA_ARGS__)
#define to_tuplestr19(a, ...) to_tuplestr1(a), to_tuplestr18(__VA_ARGS__)
#define to_tuplestr20(a, ...) to_tuplestr1(a), to_tuplestr19(__VA_ARGS__)
#define to_tuplestr21(a, ...) to_tuplestr1(a), to_tuplestr20(__VA_ARGS__)
#define to_tuplestr22(a, ...) to_tuplestr1(a), to_tuplestr21(__VA_ARGS__)
#define to_tuplestr23(a, ...) to_tuplestr1(a), to_tuplestr22(__VA_ARGS__)
#define to_tuplestr24(a, ...) to_tuplestr1(a), to_tuplestr23(__VA_ARGS__)
#define to_tuplestr25(a, ...) to_tuplestr1(a), to_tuplestr24(__VA_ARGS__)
