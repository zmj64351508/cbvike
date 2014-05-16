#cbvike

VIM plugin for Code::Blocks.

It's based on <https://code.google.com/p/cbvike/> and improved in my own way. So It still retains the name of `cbvike`.

Currently build and tested on stable version 12.11

## Basic Function
It can emulate the basic functions of VIM. Tested functions are as follow:

Movement

	h, j, k, l
	w	         -- goto next word
    b            -- goto previous word
	gg           -- goto start of file
	<num>G       -- goto line(num)
	G            -- goto end of file
	0            -- visiable line start
	^            -- line start
	$            -- line end
    f<character> -- find next character
	F<character> -- find previous character
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
	(num)cc,     -- change (num) line(s)
	(num)c$      -- change to line end
	r<character> -- replace current position by character
	u            -- undo
	Ctrl-r       -- redo
	
Cut and paste

	x -- delete
	X -- backspace
	(num)yw      -- yank (num) word(s)
	(num)yy      -- yank (num) line(s)
	(num)y$      -- yank to line end
	(num)dw      -- delete (num) word(s)
	(num)dd      -- delete (num) line(s)
	(num)d$      -- delete to line end
	(num)D       -- the same as d$
	p            -- paste below
	P            -- paste above

Bracket related

	ci<bracket>  -- change in bracket -- such as ci(, ci), ci{, ci}, ci<, ci>, ci[, ci]

Commands
	
	/<something to find>
	:nohl        -- no hightlight


## Known Issues

* When the key shortcuts are defined in the `keybinder` plugin like `Ctrl-r`, it won't take effect. So if you want to used such shortcuts, ensure it is not defined in `keybinder`. Maybe I'll provide a script to undefine such shortcuts in `keybinder` later. 
* It can't handle split window edit. 

## Installation

Currently It has been built under Windows 7 64 bit with Code::Blocks 12.11 (build 8629) and MinGW including GCC 4.8.1 

You need to build it yourself. The project is managed by Code::Blocks. When you open the project you need to set the include and lib searching path yourself in the `Project - Build Options`.

Before you build the plugin you need to build wxWidget 2.8.x(for me is 2.8.12) and Code::Blocks(for me is build 8629) from source. You can get help from <http://wiki.codeblocks.org/index.php?title=Installing_Code::Blocks_from_source_on_Windows>

If you have ideas or need help, feel free to contact me by <Arthur.zmj@gmail.com>



