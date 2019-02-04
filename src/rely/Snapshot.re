/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open SnapshotIO;
open Common.Collections;
open TestResult.AggregatedResult;

type state = {
  unusedSnapshotSet: MStringSet.t,
  updatedSnapshotSet: MStringSet.t,
  removedSnapshotCount: ref(int),
  updateSnapshots: bool,
  startingSnapshotSet: StringSet.t,
  snapshotDir: string,
};

module Make = (IO: SnapshotIO) => {
  exception InvalidSnapshotFileName(string);

  let markSnapshotUsed = (snapshot, state) => {
    let _ = MStringSet.delete(snapshot, state.unusedSnapshotSet);
    state;
  };
  let markSnapshotUpdated = (snapshot, state) => {
    let _ = MStringSet.add(snapshot, state.updatedSnapshotSet);
    /* Updated also implies used. */
    let _ = MStringSet.delete(snapshot, state.unusedSnapshotSet);
    state;
  };

  let initializeState = (~snapshotDir, ~updateSnapshots) => {
    let unusedSnapshotSet = MStringSet.empty();
    if (IO.existsFile(snapshotDir)) {
      let _ =
        IO.readSnapshotNames(snapshotDir)
        |> List.map(filePath => MStringSet.add(filePath, unusedSnapshotSet));
      ();
    };
    let updatedSnapshotSet = MStringSet.empty();
    let removedSnapshotCount = ref(0);
    let startingSnapshotSet =
      unusedSnapshotSet |> MStringSet.toList |> StringSet.fromList;

    {
      unusedSnapshotSet,
      updatedSnapshotSet,
      updateSnapshots,
      removedSnapshotCount,
      startingSnapshotSet,
      snapshotDir,
    };
  };

  let fileToTestName = entry => {
    let snapshot = Filename.basename(entry);
    let parts = snapshot |> Str.split(Str.regexp("[.]")) |> Array.of_list;
    if (Array.length(parts) < 4) {
      raise(InvalidSnapshotFileName("invalid snapshot"));
    } else {
      parts[1];
    };
  };

  let markSnapshotsAsCheckedForTest = (testName, state) => {
    MStringSet.filter(
      entry => {
        let snapshotTestName = fileToTestName(entry);
        snapshotTestName != testName;
      },
      state.unusedSnapshotSet,
    );
    state;
  };

  let getSnapshotStatus = state => {
    let {removedSnapshotCount} = state;
    let updatedSnapshotCount = ref(0);
    let createdSnapshotCount = ref(0);
    let _ =
      MStringSet.forEach(
        snapshot =>
          if (StringSet.has(snapshot, state.startingSnapshotSet)) {
            incr(updatedSnapshotCount);
          } else {
            incr(createdSnapshotCount);
          },
        state.updatedSnapshotSet,
      );
    {
      numCreatedSnapshots: createdSnapshotCount^,
      numRemovedSnapshots: removedSnapshotCount^,
      numUpdatedSnapshots: updatedSnapshotCount^,
    };
  };

  let removeUnusedSnapshots = state =>
    List.iter(
      snapshot =>
        if (IO.existsFile(snapshot)) {
          IO.removeFile(snapshot);
          state.removedSnapshotCount := state.removedSnapshotCount^ + 1;
        },
      MStringSet.values(state.unusedSnapshotSet),
    );
};
