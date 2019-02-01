---
id: introduction
title: Reason Native
sidebar_label: Introduction
---

Testing, printing, coloring, and other tools to effectively write native Reason code.


## Packages

- [`@reason-native/rely`](./rely/index.md): Native Jest style API.
- [`@reason-native/console`](./console/index.md): `Console.log(anything)`. At Runtime. Natively.
- [`@reason-native/pastel`](./pastel/index.md): Terminal highlighting with support for nesting and style propagation.
- [`@reason-native/refmterr`](./refmterr/index.md): Utility for extracting structure from unstructured ocaml compiler errors, and displaying them.
- [`@reason-native/file-context-printer`](./file-context-printer/index.md): Utility for displaying snippets of files on the command line.
- [`@reason-native/pastel-console`](./pastel/console.md): To be used in `devDependencies`. Enhances `console` with colors.

## Status

The packages included in this repo are not yet released to npm/opam. They are being made public
as a way to gather feedback and share progress on native command line tooling.

Inside of this repo, there are several packages inside the `src` directory that are to be individually
released when their APIs are sufficiently stable.

## Contributing

* install [esy](https://esy.sh/)
* run `esy install` and `esy build`
* test executables are currently defined in the respective .json files for each repository, to run them run `esy x ExecutableName.exe`

## License

Reason Native is MIT licensed, as found in the LICENSE file.
