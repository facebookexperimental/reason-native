/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type describe =
  | Terminal(string)
  | Nested(string, describe);
type test = (string, describe);

type t =
  | Describe(describe)
  | Test(test);

let ancestrySeparator = " › ";
let ancestryRegex = Str.regexp_string(ancestrySeparator);

let rec toString = path =>
  switch (path) {
  | Describe(Terminal(name)) => name
  | Describe(Nested(name, describe))
  | Test((name, describe)) =>
    String.concat(ancestrySeparator, [toString(Describe(describe)), name])
  };

let describeToString = d => Describe(d) |> toString;
let testToString = t => Test(t) |> toString;

let hash = (path, attempt) => {
  let testTitle = toString(path);
  let testHash =
    String.sub(
      Common.Strs.Crypto.md5(
        Str.global_replace(ancestryRegex, "", testTitle)
        ++ string_of_int(attempt),
      ),
      0,
      8,
    );
  testHash;
};
