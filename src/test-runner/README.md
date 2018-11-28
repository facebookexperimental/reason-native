# test-runner


[![CircleCI](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master.svg?style=svg)](https://circleci.com/gh/yourgithubhandle/test-runner/tree/master)

## Getting Started

Install test-runner using [`esy`](https://esy.sh/)

```bash
esy add @reason-native/test-runner
```

Or manually add it as a dev dependency to your package.json and run
```bash
esy install
```

Let's start by creating a library for our tests inside a directory called test and create a dune file to handle building our code (if you wish to use another build system, the important thing here is to pass the -linkall flag to the compiler)
```
│
├─test/
│   lib/
|       dune
│
```

#### dune
```
(library
   (name MyLibTest)
   (public_name my-lib.test)
   ; the linkall flag ensures that all of our tests are compiled and the -g flag emits debugging information
   (ocamlopt_flags -linkall -g)
   ; you will want to depend on the library you are testing as well,
   ; however for the purposes of this example we are only depending on the test runner itself
   (libraries test-runner.lib )
)
```

Now let's create a file to initialize the test framework. Here we are specifying where snapshots should be stored as well as the root directory of your project for the formatting of terminal output.
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
```

#### TestFramework.re
```reason
include TestRunner.Make({
  let config =
    TestRunner.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__"
      projectDir: "path/to/your/project"
    });
});
```

Now we can finally write our first test!
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
|       MyFirstTest.re
```

```reason
open TestFramework;

describe("my first test suite", ({test}) => {
  test("1 + 1 should equal 2", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });
});
```

From here let's create an executable to actually run our tests.
```
│
├─test/
│   lib/
|       dune
|       TestFramework.re
|       MyFirstTest.re
|   exe/
|       dune
|       MyLibTest.re
```

#### dune
```
(executable
   (name MyLibTest)
   (public_name MyLibTest.exe)
   (libraries  my-lib.test )
)
```

#### MyLibTest.re
```reason
MyLibTest.TestFramework.cli()
```

Finally we can run `esy build && esy x MyLibTest.exe` to build and run our tests.


**Contains the following libraries and executables:**

```
test-runner@0.0.0
│
├─test/
│   name:    TestTestRunner.exe
│   main:    TestTestRunner
│   require: test-runner.lib
│
├─library/
│   library name: test-runner.lib
│   namespace:    TestRunner
│   require:
│
└─executable/
    name:    TestRunnerApp.exe
    main:    TestRunnerApp
    require: test-runner.lib
```

## Developing:

```
npm install -g esy
git clone <this-repo>
esy install
esy build
```

## Running Binary:

After building the project, you can run the main binary that is produced.

```
esy x TestRunnerApp.exe
```

## Running Tests:

```
# Runs the "test" command in `package.json`.
esy test
```
