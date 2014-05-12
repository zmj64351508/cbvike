#cbvike

VIM plugin for Code::Blocks.

It's based on <https://code.google.com/p/cbvike/> and improved in my own way. So It still retains the name of `cbvike`.

Currently build and tested on stable version 12.11

## Basic Function
It can emulate the basic functions of VIM. Tested functions are as follow:

Movement

	h, j, k, l
	w, b
	gg, (num)G, 0
	^, $
    f, F
	n, N

Insert

	i, I, a, A, o, O, ESC

Editing

	r(work properly with ASCII)
	(num)cw, (num)cc, (num)c$ 
	u, Ctrl-r

Cut and paste

	x, X
	(num)yw, (num)yy, (num)y$
	(num)dw, (num)dd, (num)d$, (num)D
	p, P

Commands
	
	/<something to find>
	:nohl


## Known Issues

* When the key shortcuts are defined in the `keybinder` plugin like `Ctrl-r`, it won't take effect. So if you want to used such shortcuts, ensure it is not defined in `keybinder`. Maybe I'll provide a script to undefine such shortcuts in `keybinder` later. 
* It can't handle split window edit. 

## Installation

Currently It has been built under Windows 7 64 bit with Code::Blocks 12.11 (build 8629) and MinGW including GCC 4.8.1 

You need to build it yourself. The project is managed by Code::Blocks. When you open the project you need to set the include and lib searching path yourself in the `Project - Build Options`.

Before you build the plugin you need to build wxWidget 2.8.x(for me is 2.8.12) and Code::Blocks(for me is build 8629) from source. You can get help from <http://wiki.codeblocks.org/index.php?title=Installing_Code::Blocks_from_source_on_Windows>

If you have ideas or need help, feel free to contact me by <Arthur.zmj@gmail.com>



