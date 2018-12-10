/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/* Note: this file must be run at root directory of the project. Otherwise the
   Sys.command calls below happen in the wrong directory */
/* File's in ocaml for now, because our `ocaml tests/test.ml` call doesn't allow
   us to pass the `-pp refmt` option to parse reason code. Would be nice if `ocaml`
   had that like `ocamlc` */
let folders = [
  /* (directory, number of tests) */
  /* first one is special. See the actual tests loop below */
  ("specialTests", 4, [], []),
  /* `textTests` are simply recorded raw texts ran through berror. */
  ("textTests", 1, [], []),
  ("noError", 1, [], []),
  ("prettyPrint", 2, [], []),
  ("1_bad_file_name", 1, [], []),
  ("bad-file-name-2", 1, [], []),
  ("file_IllegalCharacter", 1, [], []),
  ("file_SyntaxError", 7, [], [7]),
  ("type_FunctionWrongLabel", 5, [], []),
  ("type_AppliedTooMany", 3, [], []),
  ("type_SignatureItemMismatch", 12, [11, 12], []),
  ("type_AppliedWithoutLabel", 1, [], []),
  ("type_IncompatibleType", 10, [], []),
  ("type_MismatchTypeArguments", 1, [], []),
  ("type_NotAFunction", 1, [], []),
  ("type_RecordFieldNotBelong", 3, [], []),
  ("type_RecordFieldsUndefined", 1, [], []),
  ("type_UnboundModule", 2, [], []),
  ("type_UnboundRecordField", 2, [], []),
  ("type_UnboundTypeConstructor", 2, [], []),
  ("type_UnboundValue", 4, [], []),
  ("warning_OptionalArgumentNotErased", 2, [], []),
  ("warning_PatternNotExhaustive", 2, [], []),
  ("warning_PatternUnused", 1, [], []),
];

exception Not_equal(string);

let beginsWithPrefix = (str, prefix) => {
  let prefixLength = String.length(prefix);
  if (String.length(str) < prefixLength) {
    false;
  } else {
    let strPrefix = String.sub(str, 0, prefixLength);
    strPrefix == prefix;
  };
};

let readFile = filePath => {
  let lines = {contents: []};
  let chan = open_in(filePath);
  try (
    {
      while (true) {
        let line = input_line(chan);
        /* Rather hacky, however the reason test case was outputting stuff like
         * Command line: refmt 'tests/file_SyntaxError/file_SyntaxError_7.re' > /var/folders/ty/3n7_qfdn767dqdzjs76k9mm1brfmx_/T/ocamlpp138f80
         * which caused a different temporary file to be generated each time (and is somewhat OS dependent). As these tests run from a shell script
         * and use ocaml directly, we don't have access to a bunch of libraries, so this seemed like the simplest way to get the tests running smoothly
         */
        if (!beginsWithPrefix(line, "Command line:")) {
          lines.contents = [line, ...lines.contents];
        };
      };
      "this will never be reached";
    }
  ) {
  | End_of_file =>
    close_in(chan);
    List.rev(lines.contents) |> String.concat("\n");
  };
};

/* these generate ocaml errors that points to nonexistant files. Handle them
   specially here */
let specialTestsCommands = [
  "ocamlc -open NonExistantModule foo.ml",
  "ocamlc nonexistentFile.cmo",
  "ocamlc nonexistentFile.ml",
  /* this file was generated by running `echo "let a:string = 1" | utop -stdin &> stderrTestInput.txt` */
  switch (Sys.os_type) {
  | "Win32" => "type tests\\specialTests\\stderrTestInput.txt"
  | _ => "cat tests/specialTests/stderrTestInput.txt"
  },
];

let changeCommands = {contents: []};

let forEachTest =
    (i, (dirname, fileCount, indicesWithInterfaces, indicesWithReason)) =>
  for (j in 1 to fileCount) {
    let testsDirname = Filename.concat("tests", dirname);
    /* text test */
    let isReason = List.exists(q => q == j, indicesWithReason);
    let filename =
      i === 1 ?
        Filename.concat(
          testsDirname,
          Printf.sprintf("%s_%d.txt", dirname, j),
        ) :
        Filename.concat(
          testsDirname,
          isReason ?
            Printf.sprintf("%s_%d.re", dirname, j) :
            Printf.sprintf("%s_%d.ml", dirname, j),
        );
    let interfaceFilename =
      i === 1 ?
        Filename.concat(
          testsDirname,
          Printf.sprintf("%s_%d.txt", dirname, j),
        ) :
        Filename.concat(
          testsDirname,
          Printf.sprintf("%s_%d.mli", dirname, j),
        );
    let expectedOutputName =
      Filename.concat(
        testsDirname,
        Printf.sprintf("%s_%d_expected.txt", dirname, j),
      );
    let actualOutputName =
      Filename.concat(
        testsDirname,
        Printf.sprintf("%s_%d_actual.txt", dirname, j),
      );
    /* special handling of the first item, specialTests */
    let cmd =
      if (i === 0) {
        List.nth(specialTestsCommands, j - 1);
      } else if (i === 1) {
        (
          switch (Sys.os_type) {
          | "Win32" => "type "
          | _ => "cat "
          }
        )
        ++ filename;
      } else if (List.exists(q => q == j, indicesWithInterfaces)) {
        "ocamlc -w +40 "
        ++ interfaceFilename
        ++ " &&  ocamlc -I "
        ++ Filename.dirname(filename)
        ++ " -w +40 "
        ++ filename;
      } else if (isReason) {
        "ocamlc -pp refmt -w +40 -impl " ++ filename;
      } else {
        "ocamlc -w +40 " ++ filename;
      };

    /* expecting compiling errors in stderr; pipe to a file */
    let windowsCompatibilityPipe =
      switch (Sys.os_type) {
      | "Win32" => "| sed -E s/([A-Za-z0-9])\\\\([A-Za-z0-9])/\\1\\/\\2/g | sed -E s/\\r//g"
      | _ => ""
      };
    ignore(
      Sys.command(
        Printf.sprintf(
          "(%s) 2>&1 | berror.exe --path-to-refmttype refmttype %s > %s",
          cmd,
          windowsCompatibilityPipe,
          actualOutputName,
        ),
      ),
    );
    /* open the produced error output */
    let expected = readFile(expectedOutputName);
    let actual = readFile(actualOutputName);
    /* swap-comment below two lines if you want to generate new expected
       from the new actual */
    /* ignore @@ Sys.command @@ Printf.sprintf("cp %s %s", actualOutputName, expectedOutputName); */
    /* TODO: show the differences */
    if (actual != expected) {
      print_endline("The test output does not match what was expected.");
      print_newline();
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_endline("Actual:" ++ actualOutputName);
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_endline(actual);
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_newline();
      print_newline();
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_endline("Expected:" ++ expectedOutputName);
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_endline(expected);
      print_endline(
        "----------------------------------------------------------------------",
      );
      print_newline();
      print_newline();
      changeCommands.contents = [
        "cp " ++ actualOutputName ++ " " ++ expectedOutputName,
        ...changeCommands.contents,
      ];
      print_newline();
      /* raise(Not_equal(filename)); */
    };
  };

let rmCommand =
  switch (Sys.os_type) {
  | "Win32" => "cd tests && del /Q /F /S \"*.cmi\" && del /Q /F /S \"*.cmo\""
  | _ => "rm -rf ./tests/**/*.{cmi,cmo}"
  };

try (
  {
    List.iteri(forEachTest, folders);

    ignore(Sys.command(rmCommand));

    if (List.length(changeCommands.contents) > 0) {
      print_newline();
      print_endline("To accept the changes run:");
      print_endline(String.concat("\n", changeCommands.contents));
      exit(1);
    } else {
      print_newline();
      print_endline("ALL GOOD!");
    };
  }
) {
/* trust me I'm not evil */
/* the leftover cmi and cmo files from some partially failed ocamlc above
   cause the next `make` build to fail out of refusal to compile with these
   leftover artifact, so we remove them */
| a =>
  ignore(Sys.command(rmCommand));
  raise(a);
};
