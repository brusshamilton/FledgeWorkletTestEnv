# FledgeWorkletTestEnv
A simple JavaScript environment for testing FLEDGE worklets

There is only a single binary, which accepts a single argument -- a path to a JavaScript file to use as a bidding script. 

## Building

First you will need to install the [bazel build system](https://bazel.build/). This has been verified to work with version 6.1.1, though this doesn't use any of the fancier features so most versions should work equally well.

Build the executable with clang by running
```sh
bazel  build --config=clang :check_script
```
You will need to have a recent version of `clang` installed and in your path.

If you want to try with gcc, then you can try to build with 
```sh
bazel build :check_script
```
I have not tested with gcc.

## Running

After building the binary, you can test your bidding script by running
```sh
./bazel-bin/check_script <path_to_script>
```

There is a sample bidding script provided in the repository in `examples/bid.js`.

## Known limitations

* We do not provide all of the additional functions available in the actual FLEDGE worklet environment.
* We do not provide all of the fields of all of the arguments to `generateBid()`.
* We do not validate all of the components of the return value of `generateBid()`.
