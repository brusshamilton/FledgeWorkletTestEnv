
load("@bazel_skylib//:workspace.bzl", "bazel_skylib_workspace")
load("@rules_python//python:pip.bzl", "pip_install")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

def load_v8_transitive_dependencies():
    bazel_skylib_workspace()
    pip_install(
        name = "v8_python_deps",
        extra_pip_args = ["--require-hashes"],
        requirements = "@v8//:bazel/requirements.txt",
    )

    new_git_repository(
        name = "com_googlesource_chromium_icu",
        commit = "1e49ac26ddc712b1ab702f69023cbc57e9ae6628",
        remote = "https://chromium.googlesource.com/chromium/deps/icu.git",
        build_file = "@v8//:bazel/BUILD.icu",
    )

    native.bind(
        name = "icu",
        actual = "@com_googlesource_chromium_icu//:icu",
    )

    new_git_repository(
        name = "com_googlesource_chromium_base_trace_event_common",
        commit = "147f65333c38ddd1ebf554e89965c243c8ce50b3",
        remote = "https://chromium.googlesource.com/chromium/src/base/trace_event/common.git",
        build_file = "@v8//:bazel/BUILD.trace_event_common",
    )

    native.bind(
        name = "base_trace_event_common",
        actual = "@com_googlesource_chromium_base_trace_event_common//:trace_event_common",
    )

