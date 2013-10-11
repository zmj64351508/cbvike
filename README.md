#cbvike

VIM plugin for Code::Blocks.

It's based on <https://code.google.com/p/cbvike/> and improved in my own way. So It still retains the name of `cbvike` but I change the license file.

Currently build and tested on stable version 12.11

## Basic Function
It can emulate the basic functions of VIM. Tested functions are as follow:

Movement

	h, j, k, l
	w, b
	gg, (num)G, 0
	^, $
	f,F

Insert

	i, I, a, A, o, O, ESC

Editing

	r(work properly with ASCII)
	(num)cw, (num)cc, c$ 
	u, Ctrl-r

Cut and paste

	x
	(num)yw, (num)yy, y$ 
	(num)dw, (num)dd, d$, D
	p, P 


## Known Issues

* It can't handle split window edit. 

## Installation

Currently It has been built under Windows 7 64 bit with Code::Blocks 12.11 (build 8629) and MinGW including GCC 4.8.1 

----
If you are using stable version 12.11, you may install from `build/vike.cbplugin` . 
Or you can 

  copy `build/bin/vike.dll` to `<directory of Code::Blocks installed>/share/CodeBlocks/plugins`

  copy `build/bin/vike.zip` to `<directory of Code::Blocks installed>/share/CodeBlocks`

---
Under other situations, you may need to build it yourself. The project is managed by Code::Blocks. When you open the project you need to set the include and lib searching path yourself in the `Project - Build Options`.

Before you build the plugin you need to build wxWidget 2.8.x(for me is 2.8.12) and Code::Blocks(for me is build 8629) from source. You can get help from <http://wiki.codeblocks.org/index.php?title=Installing_Code::Blocks_from_source_on_Windows>

If you have ideas or need help, feel free to contact me by <Arthur.zmj@gmail.com>



