# Compile Test Reference

## Ideas

- [ ] implement an output reference format to combine both the output and error streams into single file

## Format

```
// [condition (',' condition)*]? command ':' args?
```

where

- `<condition` is one of conditions given to the test tool
- `<command>` is one of the predefined commands
- `<args>` is a list of arguments to the command

## Commands

- `same-line` - matches a line in the output
- `next-line` - matches a next line in the output

## Example

```
// [x86_64] same-line: "Hello, world!"
println("Hello, world!");
```
