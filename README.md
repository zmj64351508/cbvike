#cbvike

VIM plugin for Code::Blocks.

It's based on <https://code.google.com/p/cbvike/> and improved in my own way. So It still retains the name of `cbvike`.

Currently build and tested on stable version 12.11, 13.12 and svn trunk

## Basic Function
It can emulate the basic functions of VIM. Tested functions are as follow:

###NOTES
- Most of the commands can start with a number which means the times of the command to execute repeatedly.

###Motion Commands

`Motion Commands` can move the caret.

    h, j, k, l
    w            -- goto next word
    b            -- goto previous word
    e            -- goto end of the word
    gg           -- goto start of file
    <num>G       -- goto line <num>
    G            -- goto end of file
    0            -- visiable line start
    ^            -- line start
    $            -- line end
    f<character> -- find next character
    F<character> -- find previous character
    t<character> -- till next character
    T<character> -- till back character
    n            -- next match case
    N            -- previous match case

###Operator Commands

`Operator Commands` are the ones that can be follow by `Motion Commands`. There are 3 `Operator Commands`:

    d -- delete
    c -- cut
    y -- yank(copy)

All the Operator Commands can used like `<Operator> <Motion>`

eg.

    2dw: delete 2 words
    c2j: change current line and 2 lines below
    2yfa: yank from the caret to the second occurence of 'a'
    2d3l: equal to 6yl or y6l, delete 6 characters start from caret in current line
    d/abc: delete text to next searching result of /abc

###Other Commands
Mode Swap

    ESC
    i            -- insert at current position
    I            -- insert at line start
    a            -- append at current position
    A            -- append at line end
    o            -- new line after current line
    O            -- new line above current line

Editing
    
    r<character> -- replace current position by character
    u            -- undo
    Ctrl-r       -- redo
    
Cut and paste

    x            -- delete
    X            -- backspace
    cc           -- change (num) line(s)
    C            -- the same as c$
    yy           -- yank (num) line(s)
    Y            -- the same as yy
    dd           -- delete (num) line(s)
    D            -- the same as d$
    p            -- paste below
    P            -- paste above

Scroll
    
    zz           -- scroll line to centre of the screen
    zt           -- scroll line to top of the screen

Bracket related

    ci<bracket>  -- change in bracket -- such as ci(, ci), ci{, ci}, ci<, ci>, ci[, ci], ci", ci'

###Extra Commands
    
    /<something to find>
    :nohl        -- no highlight searching


## Known Issues

* When the key shortcuts are defined in the `keybinder` plugin like `Ctrl-r`, it won't take effect. So if you want to used such shortcuts, ensure it is not defined in `keybinder`. Maybe I'll provide a script to undefine such shortcuts in `keybinder` later. 
* Split and unsplit is fully supported after Code::Blocks svn trunk [9835](http://sourceforge.net/p/codeblocks/code/9835/) and is not supported before it.

## Installation

Currently It has been built under Windows 8 64 bit with Code::Blocks svn trunk and TDM-MinGW including GCC 4.7.1 

1. You can build the plugin with or without building Code::Blocks and wxWidgets. But any way, you need to download source code for Code::Blocks and wxWidgets, at least for the headers.

2. You need to set following variables in Code::Blocks before compiling
    - `wx` -- the source directory for wxWidgets
    - `cb_path`     -- the source directory for Code::Blocks
    - `cb_dll_path` -- the binary directory for Code::Blocks including cb and wx libraries. This can be either the directory where you build source yourself or the directory you installed Code::Blocks from a pre-built binary.

3. Build target `All` or `release` for users and `debug` for developers.

If you have ideas or need help, feel free to contact me by <Arthur.zmj@gmail.com>



