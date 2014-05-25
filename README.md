#cbvike

VIM plugin for Code::Blocks.

It's based on <https://code.google.com/p/cbvike/> and improved in my own way. So It still retains the name of `cbvike`.

Currently build and tested on stable version 12.11

## Basic Function
It can emulate the basic functions of VIM. Tested functions are as follow:

Movement

    h, j, k, l
    w            -- goto next word
    b            -- goto previous word
    e            -- goto end of the word
    gg           -- goto start of file
    <num>G       -- goto line(num)
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

Insert

    ESC
    i            -- insert at current position
    I            -- insert at line start
    a            -- append at current position
    A            -- append at line end
    o            -- new line after current line
    O            -- new line above current line

Editing
    
    (num)cw      -- change (num) word(s)
    (num)cc      -- change (num) line(s)
    (num)c$      -- change to line end
    (num)C       -- the same as c$
    r<character> -- replace current position by character
    u            -- undo
    Ctrl-r       -- redo
    
Cut and paste

    x            -- delete
    X            -- backspace
    (num)yw      -- yank (num) word(s)
    (num)yy      -- yank (num) line(s)
    (num)y$      -- yank to line end
    (num)Y       -- the same as yy
    (num)dw      -- delete (num) word(s)
    (num)dd      -- delete (num) line(s)
    (num)d$      -- delete to line end
    (num)D       -- the same as d$
    p            -- paste below
    P            -- paste above

Bracket related

    ci<bracket>  -- change in bracket -- such as ci(, ci), ci{, ci}, ci<, ci>, ci[, ci], ci", ci'

Commands
    
    /<something to find>
    :nohl        -- no highlight


## Known Issues

* When the key shortcuts are defined in the `keybinder` plugin like `Ctrl-r`, it won't take effect. So if you want to used such shortcuts, ensure it is not defined in `keybinder`. Maybe I'll provide a script to undefine such shortcuts in `keybinder` later. 
* It can't handle split window edit. If you enter split edit mode, the plugin won't take any effect until the editor returns to un-splitted. 
* **NOTE:** I've commit a patch to Code::Blocks and as soon as the patch takes effect, this plugin will provide full support to split edit. Just rebuild it with the new SDK.

## Installation

Currently It has been built under Windows 7 64 bit with Code::Blocks 12.11 (build 8629) and MinGW including GCC 4.8.1 

1. You can build the plugin with or without building Code::Blocks and wxWidgets. But any way, you need to download source code for Code::Blocks and wxWidgets, at least for the headers.

2. You need to set following variables in Code::Blocks before compiling

    * `wx` -- the source directory for wxWidgets
    * `cb_path`     -- the source directory for Code::Blocks
    * `cb_dll_path` -- the binary directory for Code::Blocks including cb and wx libraries. This can be either the directory where you build source yourself or the directory you installed Code::Blocks from a pre-built binary.

3. Build target `All` or `release` for users and `debug` for developers.

If you have ideas or need help, feel free to contact me by <Arthur.zmj@gmail.com>



