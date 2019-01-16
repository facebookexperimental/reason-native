/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

type singleSuiteInput = {
  name: string,
  numPassingTests: int,
  numFailingTests: int,
  numSkippedTests: int,
};

type multipleSuiteInput = {
  name: string,
  testSuites: list(TestSuite.t),
};

type singleSuiteExpectedOutput = {
  numPassedTests: int,
  numFailedTests: int,
  numSkippedTests: int,
  numPassedTestSuites: int,
  numPendingTestSuites: int,
  numTotalTests: int,
  numTotalTestSuites: int,
  numSkippedTestSuites: int,
  numFailedTestSuites: int,
};

describe("Rely AggregateResult", ({describe, test}) => {
  describe("Single suite cases", ({test}) => {
    let singleSuiteInput = (passing, failing, skipped) => {
      let name =
        String.concat(
          ", ",
          [
            string_of_int(passing) ++ " passing tests",
            string_of_int(failing) ++ " failing tests",
            string_of_int(skipped) ++ " skipped tests",
          ],
        );
      {
        name,
        numPassingTests: passing,
        numFailingTests: failing,
        numSkippedTests: skipped,
      };
    };
    let singleSuiteTestCases = [
      (
        {
          name: "single skipped test",
          numPassingTests: 0,
          numFailingTests: 0,
          numSkippedTests: 1,
        },
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 0,
          numPassedTests: 0,
          numFailedTests: 0,
          numSkippedTests: 1,
        },
      ),
      (
        {
          name: "single passing test",
          numPassingTests: 1,
          numFailingTests: 0,
          numSkippedTests: 0,
        },
        {
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
          numPassedTests: 1,
          numFailedTests: 0,
          numSkippedTests: 0,
        },
      ),
      (
        {
          name: "single failing test",
          numPassingTests: 0,
          numFailingTests: 1,
          numSkippedTests: 0,
        },
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 0,
          numFailedTests: 1,
          numSkippedTests: 0,
        },
      ),
      (
        singleSuiteInput(1, 10, 100),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 1,
          numFailedTests: 10,
          numSkippedTests: 100,
        },
      ),
      (
        singleSuiteInput(1, 100, 10),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 1,
          numFailedTests: 100,
          numSkippedTests: 10,
        },
      ),
      (
        singleSuiteInput(10, 1, 100),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 10,
          numFailedTests: 1,
          numSkippedTests: 100,
        },
      ),
      (
        singleSuiteInput(10, 100, 1),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 10,
          numFailedTests: 100,
          numSkippedTests: 1,
        },
      ),
      (
        singleSuiteInput(100, 1, 10),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 100,
          numFailedTests: 1,
          numSkippedTests: 10,
        },
      ),
      (
        singleSuiteInput(100, 10, 1),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 100,
          numFailedTests: 10,
          numSkippedTests: 1,
        },
      ),
      (
        singleSuiteInput(100, 0, 1),
        {
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 101,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
          numPassedTests: 100,
          numFailedTests: 0,
          numSkippedTests: 1,
        },
      ),
    ];
    let testSingleSuite = ((input: singleSuiteInput, expectedOutput)) =>
      test(
        input.name,
        ({expect}) => {
          open Rely.TestResult.AggregatedResult;
          let aggregatedResult =
            ref({
              numFailedTests: 0,
              numFailedTestSuites: 0,
              numPassedTests: 0,
              numPassedTestSuites: 0,
              numPendingTestSuites: 0,
              numSkippedTests: 0,
              numSkippedTestSuites: 0,
              numTotalTests: 0,
              numTotalTestSuites: 0,
              snapshotSummary: None,
              testSuiteResults: [],
            });

          let testSuites = [
            TestSuite.(
              init(input.name)
              |> withSkippedTests(input.numSkippedTests)
              |> withPassingTests(input.numPassingTests)
              |> withFailingTests(input.numFailingTests)
            ),
          ];
          module Reporter =
            TestReporter.Make({});
          Reporter.onRunComplete(res => aggregatedResult := res);

          TestSuiteRunner.run(testSuites, Reporter.reporter);
          let actualOutput = {
            numPassedTestSuites: aggregatedResult^.numPassedTestSuites,
            numPendingTestSuites: aggregatedResult^.numPendingTestSuites,
            numTotalTests: aggregatedResult^.numTotalTests,
            numTotalTestSuites: aggregatedResult^.numTotalTestSuites,
            numSkippedTestSuites: aggregatedResult^.numSkippedTestSuites,
            numFailedTestSuites: aggregatedResult^.numFailedTestSuites,
            numPassedTests: aggregatedResult^.numPassedTests,
            numFailedTests: aggregatedResult^.numFailedTests,
            numSkippedTests: aggregatedResult^.numSkippedTests,
          };

          expect.bool(actualOutput == expectedOutput).toBeTrue();
        },
      );
    singleSuiteTestCases |> List.iter(testSingleSuite);
  });

  describe("Multiple suite cases", ({test}) => {
    let singletonPassingTestSuite =
      TestSuite.(init("passing") |> withPassingTests(1));
    let singletonFailingTestSuite =
      TestSuite.(init("failing") |> withFailingTests(1));
    let singletonSkippedTestSuite =
      TestSuite.(init("skipped") |> withSkippedTests(1));

    let repeatHelper = (el, n, l) =>
      switch (n) {
      | n when n <= 0 => l
      | n => l @ [el]
      };
    let repeat = (el, n) => repeatHelper(el, n, []);

    let singletonSuiteTestCase = (passing, failing, skipped) => {
      let name =
        String.concat(
          ", ",
          [
            string_of_int(passing) ++ " passing test suites",
            string_of_int(failing) ++ " failing test suites",
            string_of_int(skipped) ++ " skipped test suites",
          ],
        );
      let testSuites =
        repeat(singletonPassingTestSuite, passing)
        @ repeat(singletonFailingTestSuite, failing)
        @ repeat(singletonSkippedTestSuite, skipped);
      {name, testSuites};
    };

    let testCases = [
      (
        {testSuites: [], name: "no test suites"},
        {
          numPassedTests: 0,
          numFailedTests: 0,
          numSkippedTests: 0,
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 0,
          numTotalTestSuites: 0,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
        },
      ),
      (
        singletonSuiteTestCase(1, 0, 1),
        {
          numPassedTests: 1,
          numFailedTests: 0,
          numSkippedTests: 1,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 0,
        },
      ),
      (
        singletonSuiteTestCase(1, 1, 0),
        {
          numPassedTests: 1,
          numFailedTests: 1,
          numSkippedTests: 0,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
        },
      ),
      (
        singletonSuiteTestCase(0, 1, 1),
        {
          numPassedTests: 0,
          numFailedTests: 1,
          numSkippedTests: 1,
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 1,
        },
      ),
      (
        singletonSuiteTestCase(1, 1, 1),
        {
          numPassedTests: 1,
          numFailedTests: 1,
          numSkippedTests: 1,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 3,
          numTotalTestSuites: 3,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 1,
        },
      ),
    ];

    let runTestCases = ((input, expectedOutput)) =>
      test(
        input.name,
        ({expect}) => {
          open Rely.TestResult.AggregatedResult;
          let aggregatedResult =
            ref({
              numFailedTests: 0,
              numFailedTestSuites: 0,
              numPassedTests: 0,
              numPassedTestSuites: 0,
              numPendingTestSuites: 0,
              numSkippedTests: 0,
              numSkippedTestSuites: 0,
              numTotalTests: 0,
              numTotalTestSuites: 0,
              snapshotSummary: None,
              testSuiteResults: [],
            });

          module Reporter =
            TestReporter.Make({});
          Reporter.onRunComplete(res => aggregatedResult := res);

          TestSuiteRunner.run(input.testSuites, Reporter.reporter);
          let actualOutput = {
            numPassedTestSuites: aggregatedResult^.numPassedTestSuites,
            numPendingTestSuites: aggregatedResult^.numPendingTestSuites,
            numTotalTests: aggregatedResult^.numTotalTests,
            numTotalTestSuites: aggregatedResult^.numTotalTestSuites,
            numSkippedTestSuites: aggregatedResult^.numSkippedTestSuites,
            numFailedTestSuites: aggregatedResult^.numFailedTestSuites,
            numPassedTests: aggregatedResult^.numPassedTests,
            numFailedTests: aggregatedResult^.numFailedTests,
            numSkippedTests: aggregatedResult^.numSkippedTests,
          };

          expect.bool(actualOutput == expectedOutput).toBeTrue();
        },
      );
    testCases |> List.iter(runTestCases);
  });
});
