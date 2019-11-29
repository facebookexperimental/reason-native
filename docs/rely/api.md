---
id: api
title: Basic API
---

> Prefer reading code? Check out [RelyAPI.rei](https://github.com/facebookexperimental/reason-native/blob/master/src/rely/RelyAPI.rei)

## Basic Configuration

```re
/* TestFramework.re */
include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir: "path/to/test/lib/__snapshots__", /* required */
      projectDir: "path/to/your/project" /* required */
    });
});
```

**Running with default CLI config:**

```re
/* MyLibTest.re */
TestFramework.cli(); /* default config */
```

## Testing

> For a full list of available matchers, visit the [expect documentation](expect.md)

```re
/* MyFirstTest.re */
open TestFramework;

describe(
  "my first test suite",
  ({test, testSkip, testOnly, describe, describeSkip, describeOnly}) => {

  /* test suite */
  test("basic matchers", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });

  test("mock functions", ({expect}) => {
    let double = x => x * 2;
    /*there are constructors for up to 7 args */
    let mock = Mock.mock1(double);

    let list = List.map(Mock.fn(mock), [1, 2, 3, 4]);

    expect.mock(mock).toBeCalledTimes(4);
    expect.mock(mock).toBeCalledWith(2);
    expect.mock(mock).lastCalledWith(4);
    expect.mock(mock).nthCalledWith(3, 3);
    expect.mock(mock).toReturnTimes(4);
    expect.mock(mock).toReturn(6);
    expect.mock(mock).lastReturnedWith(8);
    expect.mock(mock).nthReturnedWith(3, 6);
  });

  /* test skip causes this test to be skipped (not run) */
  testSkip("incorrect test", ({expect}) => {
    expect.int(1 + 1).toBe(3);
  });

  /* describe skip skips everything inside the describe */
  describeSkip("a nested test suite", ({test}) => {
    test("snapshots", ({expect}) => {
      expect.string("I 💖 Rely").toMatchSnapshot();
    });
  });

  /* testOnly will cause the test to always be run as long as it's
   * not inside a skip (since 2.1.0) */
  testOnly("only test", ({expect}) => {
    expect.int(1 + 1).toBe(2);
  });

  /* Everything inside a describe only will always be run as long as it's not
  inside a skip (since 2.1.0) */
  describeOnly("a nested test suite", ({test}) => {
    test("trivial", ({expect}) => {
      expect.bool(true).toBeTrue();
    });
  });
});
```

## Custom Matchers

Custom matchers can also be created as below, the API is currently identical to the internal one, so additional examples can be found by looking at the [code for the built in matchers](https://github.com/facebookexperimental/reason-native/tree/master/src/rely/matchers).

```re
/*IntExtensions.re*/
type intExtensions = {toDivide: int => unit};

let intExtensions = (actual, {createMatcher}) => {
  let pass = (() => "", true);
  let createDividesMatcher = createMatcher(
      ({formatReceived, formatExpected}, actualThunk, expectedThunk) => {
      let actual = actualThunk();
      let expected = expectedThunk();
      let actualDividesExpected = expected mod actual == 0;

      if (!actualDividesExpected) {
        let failureMessage =
          String.concat(
            "",
            [
              "Expected actual to divide ",
              formatExpected(string_of_int(expected)),
              "\n",
              "Received: ",
              formatReceived(string_of_int(actual))
            ],
          );
        (() => failureMessage, false);
      } else {
        pass;
      };
    });
  {
    toDivide: (expected) => createDividesMatcher(() => actual, () => expected),
  };
};
```

```re
/* DividesTest.re*/
open TestFramework;
open IntExtensions;

type customMatchers = {
  int: int => intExtensions
};

let customMatchers = createMatcher => {
  int: i => intExtensions(i, createMatcher)
}

let { describe, describeOnly, describeSkip } =
  describeConfig |> withCustomMatchers(customMatchers) |> build;

describe("divides matchers example", ({test}) => {
  test("should divide", ({expect}) => {
    expect.ext.int(42).toDivide(84);
  });
  test("this test fails", ({expect}) => {
    expect.ext.int(43).toDivide(84);
  });
});
```
