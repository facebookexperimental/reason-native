/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type token =
  | Starts(string)
  | Stops(string)
  | Text(string);

module Make = (M: {
                 let startRegex: Re.re;
                 let stopRegex: Re.re;
               }) => {
  open M;
  let tokenize = (s: string): list(token) => {
    let parseNonStarts = nonStarts =>
      Re.split_full(stopRegex, nonStarts)
      |> List.map(nonStartSplitResult =>
           switch (nonStartSplitResult) {
           | `Delim(g) => Stops(Re.Group.get(g, 0))
           | `Text(text) => Text(text)
           }
         );
    s
    |> Re.split_full(startRegex)
    |> List.fold_left(
         (tokens, splitResult) =>
           switch (splitResult) {
           | `Delim(g) => tokens @ [Starts(Re.Group.get(g, 0))]
           | `Text(nonStarts) => tokens @ parseNonStarts(nonStarts)
           },
         [],
       );
  };

  let unformattedText = s => {
    List.fold_left(
      (acc, token) =>
        switch (token) {
        | Starts(_)
        | Stops(_) => acc
        | Text(t) => acc @ [t]
        },
      [],
      tokenize(s),
    )
    |> String.concat("");
  };

  let length = s => {
    List.fold_left(
      (acc, part) =>
        switch (part) {
        | Starts(_)
        | Stops(_) => acc
        | Text(t) => acc + String.length(t)
        },
      0,
      tokenize(s),
    );
  };

  let printTokens = tokens => {
    let tokenStr =
      List.map(
        token =>
          switch (token) {
          | Starts(starts) => "Starts(" ++ String.escaped(starts) ++ ")"
          | Stops(stops) => "Stops(" ++ String.escaped(stops) ++ ")"
          | Text(text) => "Text(" ++ String.escaped(text) ++ ")"
          },
        tokens,
      )
      |> String.concat(", ");

    print_endline("tokens: [" ++ tokenStr ++ "]");
  };

  let tokensToStr = tokens =>
    tokens
    |> List.map(token =>
         switch (token) {
         | Starts(s)
         | Stops(s)
         | Text(s) => s
         }
       )
    |> String.concat("");

  type partitionAcc = {
    activeIndex: int,
    remainingIndices: list(int),
    nonTextTokensSeen: list(token),
    parsedText: list(list(token)),
    charsSeen: int,
    activeSection: list(token),
    queue: list(token),
  };

  let partition2 = (indices, s) => {
    let tokens = tokenize(s);

    switch (indices) {
    | [] => [s]
    | [h, ...t] =>
      let initialState = {
        activeIndex: h,
        remainingIndices: t,
        nonTextTokensSeen: [],
        parsedText: [],
        charsSeen: 0,
        activeSection: [],
        queue: tokens,
      };

      let step = state =>
        switch (state.queue) {
        | [] => state
        | [token, ...remainingQueue] =>
          switch (token) {
          | Starts(_)
          | Stops(_) => {
              ...state,
              nonTextTokensSeen: [token, ...state.nonTextTokensSeen],
              parsedText:
                List.map(tokens => [token, ...tokens], state.parsedText),
              activeSection: [token, ... state.activeSection],
              queue: remainingQueue,
            }
          | Text(text) =>
            if (state.charsSeen < state.activeIndex) {
              let textLength = String.length(text);
              if (state.charsSeen + textLength < state.activeIndex) {
                {
                  ...state,
                  charsSeen: state.charsSeen + textLength,
                  activeSection: [token, ...state.activeSection],
                  queue: remainingQueue,
                };
              } else {
                let numToTake = state.activeIndex - state.charsSeen;
                let remaining = String.length(text) - numToTake;
                let remainingText = String.sub(text, numToTake, remaining);
                let firstPart = Text(String.sub(text, 0, numToTake));

                switch (state.remainingIndices) {
                | [] => {
                    ...state,
                    charsSeen: state.charsSeen + textLength,
                    activeSection: [
                      Text(remainingText),
                      ...state.nonTextTokensSeen,
                    ],
                    parsedText: [
                      [firstPart, ...state.activeSection],
                      ...state.parsedText,
                    ],
                    queue: remainingQueue,
                  }
                | [h, ...t] => {
                    ...state,
                    charsSeen: state.charsSeen + numToTake,
                    activeSection: state.nonTextTokensSeen,
                    parsedText: [
                      [firstPart, ...state.activeSection],
                      ...state.parsedText,
                    ],
                    queue: [Text(remainingText), ...remainingQueue],
                    activeIndex: h,
                    remainingIndices: t,
                  }
                };
              };
            } else {
              {
                ...state,
                activeSection: [token, ...state.activeSection],
                queue: remainingQueue,
              };
            }
          }
        };

      let state = ref(initialState);
      while (state^.queue !== []) {
        state := step(state^);
      };
      let parsedText = [state^.activeSection, ...state^.parsedText]
      List.map(
        tokens => tokensToStr(List.rev(tokens)),
        List.rev(parsedText),
      );
    };
  };

  /** everything before index is one string, everything else is the other */
  let partition = (index, s) => {
    let tokens = tokenize(s);
    let firstHalfANSI = ref([]);
    let secondHalfANSI = ref([]);
    let partition1 = ref([]);
    let partition2 = ref([]);
    let charsSeen = ref(0);
    ();
    List.iter(
      token =>
        if (charsSeen^ < index) {
          switch (token) {
          | Starts(_)
          | Stops(_) =>
            firstHalfANSI := [token, ...firstHalfANSI^];
            partition1 := [token, ...partition1^];
          | Text(t) =>
            let textLength = String.length(t);
            if (charsSeen^ + textLength < index) {
              partition1 := [token, ...partition1^];
            } else {
              let numToTake = index - charsSeen^;
              let firstPart = String.sub(t, 0, numToTake);
              let secondPart =
                String.sub(
                  t,
                  index - charsSeen^,
                  String.length(t) - numToTake,
                );
              partition1 := [Text(firstPart), ...partition1^];
              partition2 := [Text(secondPart), ...firstHalfANSI^];
            };
            charsSeen := charsSeen^ + textLength;
          };
        } else {
          switch (token) {
          | Starts(_)
          | Stops(_) =>
            secondHalfANSI := [token, ...secondHalfANSI^];
            partition2 := [token, ...partition2^];
          | Text(_) => partition2 := [token, ...partition2^]
          };
        },
      tokens,
    );

    partition1 := secondHalfANSI^ @ partition1^;

    (
      tokensToStr(List.rev(partition1^)),
      tokensToStr(List.rev(partition2^)),
    );
  };
};
