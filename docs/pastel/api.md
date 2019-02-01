---
id: api
title: Pastel API
sidebar_label: API
---

> Prefer reading code? Check out [Pastel.rei](https://github.com/facebookexperimental/reason-native/blob/master/src/pastel/Pastel.rei)

All text formatting is accomplished through the `<Pastel [props] />` tag.

## Available Props

### `color=Color`

The `color` prop specifies the color of the contained text. It can be any of the 16 [4-bit ANSI colors](https://en.wikipedia.org/wiki/ANSI_escape_code#3/4_bit):

```re
color =
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BlackBright
  | RedBright
  | GreenBright
  | YellowBright
  | BlueBright
  | MagentaBright
  | CyanBright
  | WhiteBright;
```

**Example:**
```re
print_endline(
  Pastel.(
    <Pastel color=Blue> "Hello World" </Pastel>
  )
);
```
```sh-stacked
[34mHello World[39m
```
---

### `backgroundColor=Color`

The `backgroundColor` prop specifies the color of the contained text. It can be any of the 16 [4-bit ANSI colors](https://en.wikipedia.org/wiki/ANSI_escape_code#3/4_bit):

```re
backgroundColor =
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BlackBright
  | RedBright
  | GreenBright
  | YellowBright
  | BlueBright
  | MagentaBright
  | CyanBright
  | WhiteBright;
```

**Example:**
```re
print_endline(
  Pastel.(
    <Pastel backgroundColor=Blue> "Hello World" </Pastel>
  )
);
```
```sh-stacked
[44mHello World[49m
```
---
### `bold=Boolean`

The `bold` prop specifies if Pastel should bold the text:

```re
bold =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel bold=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [1mWorld[22m
```
---
### `dim=Boolean`

The `dim` prop specifies if Pastel should dim the text:

```re
dim =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel dim=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [90mWorld[39m
```
---
### `italic=Boolean`

The `italic` prop specifies if Pastel should italicize the text:

```re
italic =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel italic=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [3mWorld[23m
```
---
### `underline=Boolean`

The `underline` prop specifies if Pastel should underline the text:

```re
underline =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel underline=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [4mWorld[24m
```
---
### `inverse=Boolean`

The `inverse` prop specifies if Pastel should invert the text:

```re
inverse =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel inverse=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [30m[107mWorld[49m[39m
```
---
### `hidden=Boolean`

The `hidden` prop specifies if Pastel should hide the text:

```re
hidden =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel hidden=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [8mWorld[28m
```

---
### `strikethrough=Boolean`

The `strikethrough` prop specifies if Pastel should strike-through the text:

```re
strikethrough =
  | true
  | false;
```

**Example:**
```re
print_endline(
  "Hello " ++
  Pastel.(
    <Pastel strikethrough=true> "World" </Pastel>
  )
);
```
```sh-stacked
Hello [9mWorld[29m
```
