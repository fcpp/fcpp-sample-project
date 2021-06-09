load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/f5e592d8ee5ffb1d9af5be7f715ce3576b8bf9c4.zip",
    sha256 = "e61e3889bd5cc3e6bc1084d2108ecda2f110c0387ba88b394ffd16043a1d5709",
    build_file = "@//:gtest.BUILD",
    strip_prefix = "googletest-f5e592d8ee5ffb1d9af5be7f715ce3576b8bf9c4",
)

git_repository(
    name = "fcpp",
    remote = "https://github.com/fcpp/fcpp.git",
    strip_prefix = "src",
    commit = "131c09a4699311b2ec0f140ea884d8bb46c5c030",
)
