cc_test(
    name = "tester",
    srcs = ["tester.cpp"],
    deps = [
        "@gtest//:main",
        "@fcpp//lib:fcpp",
        "@fcpp//test:test_net",
        "//lib:collection_compare",
    ],
    copts = ['-Iexternal/gtest/googletest/include/'],
    args = ['--gtest_color=yes'],
    timeout = 'short',
)
