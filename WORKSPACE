load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

http_archive(
    name = "gtest",
    url = "https://github.com/google/googletest/archive/release-1.8.0.zip",
    sha256 = "f3ed3b58511efd272eb074a3a6d6fb79d7c2e6a0e374323d1e6bcbcc1ef141bf",
    build_file = "@//:gtest.BUILD",
    strip_prefix = "googletest-release-1.8.0",
)

git_repository(
    name = "fcpp",
    remote = "https://github.com/fcpp/fcpp.git",
    strip_prefix = "src",
    tag = "v0.6.15",
)
