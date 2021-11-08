# stag - manage file tagging using symlinks

There are many ways to manage files and tags. However, many of them are strongly coupled to a single tool, hard to manually manipulate, or don't work with existing tools.

stag (sometimes stylized as STag) is an alternative that uses a very simple directory structure to provide this functionality. It is slower in many cases, due to not having indexing, but prioritizes simplicity and can always be manipulated manually by the user without breaking the databases's self-consistency. In cases where mistakes are likely to occur, tools are provided to help resolve them.

The S in STag can stand for many things, such as "simple", "symlink", or "straightforward".

# Building

```sh
make release # or debug
./dist/Release/stag [options] # or Debug
```

# Usage

Usage is specified in other files. Consult the table for the locations.

|(Format)|Usage|File formats|
|-:|-|-|
|Markdown|`stag.1.ronn`|`stag.5.ronn`|
|Roff (after running `make`)|`dist/stag.1`|`dist/stag.5`|

(Roff is the format used for man pages, and can be viewed with `man -l file`)
