# MyCustomshell

A small Unix shell written in C. It reads commands, runs programs in child
processes (foreground or background), and has a handful of built-in commands
and a history feature. Built with CMake and clang, using raw system calls
(`fork`, `execvp`, `waitpid`, `read`, `chdir`, `sigaction`) rather than
library shortcuts.

## Features

- Runs external programs in the foreground, or in the background with `&`
- Prompt shows the current working directory
- Built-ins: `cd` (supports `~` and `-`), `pwd`, `help`, `history`, `exit`
- History of the last 10 commands, with `!!` and `!n` to re-run one
- Shell-style quoting: `"double"`, `'single'`, and `\` escapes
- Ctrl-C does not kill the shell; it prints the built-in help instead
- Background children are reaped so they do not become zombies
- Correct handling of piped or multi-line input (one command per line)

## Build and run

Requires clang and CMake.

```bash
cmake -S . -B build
cmake --build build
./build/shell
```

## Example

```
/home/me$ echo "Hello   World"
Hello   World
/home/me$ sleep 5 &
/home/me$ history
2	history
1	sleep 5 &
0	echo "Hello   World"
/home/me$ !0
echo "Hello   World"
Hello   World
```

## Project layout

```
include/   header files, one per module
src/
  main.c       main loop: prompt, read, history expansion, dispatch
  read.c       buffered line reader and quote-aware tokenizer
  execute.c    fork/exec, foreground wait, zombie reaping
  internal.c   dispatch for built-in commands
  cd.c         cd, with ~ and - support
  help.c       help command and the SIGINT help output
  history.c    ring buffer of the last 10 commands
  util.c       error printing helper
```

Each module has a small interface in its header and keeps its state private
(for example, the input buffer in `read.c` and the history ring in
`history.c` are `static`).

## Design notes

- **Line reader:** `read()` can return partial lines or several lines at
  once, so `read_command` keeps a persistent buffer and hands back exactly
  one line per call.
- **Tokenizer:** a single in-place pass with a tiny state machine
  (unquoted / single-quoted / double-quoted). Removing quotes only shortens
  text, so no allocation is needed.
- **Signals:** the SIGINT handler uses only async-signal-safe calls
  (`write`), and the main loop treats `EINTR` from `read()` as "redraw the
  prompt".

## Roadmap

- [ ] Input/output redirection (`<`, `>`, `>>`)
- [ ] Pipes
- [ ] `;`, `&&`, `||`
- [ ] Environment variables and `$?`
- [ ] Job control (`jobs`, `fg`, `bg`, Ctrl-Z)
- [ ] Line editing and persistent history
- [ ] Automated tests and CI (ASan/UBSan)

## Known limitations

- No pipes or redirection yet
- Unterminated quotes run to the end of the line instead of erroring
- Lines longer than the buffer are truncated# myshell

A small Unix shell written in C. It reads commands, runs programs in child
processes (foreground or background), and has a handful of built-in commands
and a history feature. Built with CMake and clang, using raw system calls
(`fork`, `execvp`, `waitpid`, `read`, `chdir`, `sigaction`) rather than
library shortcuts.

## Features

- Runs external programs in the foreground, or in the background with `&`
- Prompt shows the current working directory
- Built-ins: `cd` (supports `~` and `-`), `pwd`, `help`, `history`, `exit`
- History of the last 10 commands, with `!!` and `!n` to re-run one
- Shell-style quoting: `"double"`, `'single'`, and `\` escapes
- Ctrl-C does not kill the shell; it prints the built-in help instead
- Background children are reaped so they do not become zombies
- Correct handling of piped or multi-line input (one command per line)

## Build and run

Requires clang and CMake.

```bash
cmake -S . -B build
cmake --build build
./build/shell
```

## Example

```
/home/me$ echo "Hello   World"
Hello   World
/home/me$ sleep 5 &
/home/me$ history
2	history
1	sleep 5 &
0	echo "Hello   World"
/home/me$ !0
echo "Hello   World"
Hello   World
```

## Project layout

```
include/   header files, one per module
src/
  main.c       main loop: prompt, read, history expansion, dispatch
  read.c       buffered line reader and quote-aware tokenizer
  execute.c    fork/exec, foreground wait, zombie reaping
  internal.c   dispatch for built-in commands
  cd.c         cd, with ~ and - support
  help.c       help command and the SIGINT help output
  history.c    ring buffer of the last 10 commands
  util.c       error printing helper
```

Each module has a small interface in its header and keeps its state private
(for example, the input buffer in `read.c` and the history ring in
`history.c` are `static`).

## Design notes

- **Line reader:** `read()` can return partial lines or several lines at
  once, so `read_command` keeps a persistent buffer and hands back exactly
  one line per call.
- **Tokenizer:** a single in-place pass with a tiny state machine
  (unquoted / single-quoted / double-quoted). Removing quotes only shortens
  text, so no allocation is needed.
- **Signals:** the SIGINT handler uses only async-signal-safe calls
  (`write`), and the main loop treats `EINTR` from `read()` as "redraw the
  prompt".

## Roadmap

- [ ] Input/output redirection (`<`, `>`, `>>`)
- [ ] Pipes
- [ ] `;`, `&&`, `||`
- [ ] Environment variables and `$?`
- [ ] Job control (`jobs`, `fg`, `bg`, Ctrl-Z)
- [ ] Line editing and persistent history
- [ ] Automated tests and CI (ASan/UBSan)

## Known limitations

- No pipes or redirection yet
- Unterminated quotes run to the end of the line instead of erroring
- Lines longer than the buffer are truncated
