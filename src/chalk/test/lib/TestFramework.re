/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let projectDir =
  Filename.(
    Sys.executable_name
    |> dirname
    |> dirname
    |> dirname
    |> dirname
    |> dirname
    |> dirname
    |> dirname
    |> (dir => Filename.concat(dir, "src"))
    |> (dir => Filename.concat(dir, "chalk"))
  );

include Rely.Make({
  let config =
  Rely.TestFrameworkConfig.initialize({
      snapshotDir:
        Filename.(
          projectDir
          |> (dir => Filename.concat(dir, "test"))
          |> (dir => Filename.concat(dir, "lib"))
          |> (dir => Filename.concat(dir, "__snapshots__"))
        ),
      projectDir,
    });
});
