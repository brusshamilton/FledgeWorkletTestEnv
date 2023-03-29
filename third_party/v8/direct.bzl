load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

def load_v8():
    http_archive(
        name = "v8",
        url = "https://github.com/v8/v8/archive/refs/heads/11.4.16.zip",
        strip_prefix = "v8-11.4.16",
        sha256 = "29463aa92cb6049863bd1e9618d3adf5ff86b7a85ee4f624646f3763dd7d548b",
    )

    # build deps
    http_archive(
        name = "bazel_skylib",
        sha256 = "1c531376ac7e5a180e0237938a2536de0c54d93f5c278634818e0efc952dd56c",
        urls = [
            "https://github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
            "https://mirror.bazel.build/github.com/bazelbuild/bazel-skylib/releases/download/1.0.3/bazel-skylib-1.0.3.tar.gz",
        ],
    )

    http_archive(
        name = "rules_python",
        sha256 = "a30abdfc7126d497a7698c29c46ea9901c6392d6ed315171a6df5ce433aa4502",
        strip_prefix = "rules_python-0.6.0",
        url = "https://github.com/bazelbuild/rules_python/archive/0.6.0.tar.gz",
    )

