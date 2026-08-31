" syntax/benjit.vim
if exists("b:current_syntax")
  finish
endif

" Keywords
syn keyword benjitKeyword algorithm is if else then end variable for do return break continue
syn keyword benjitBoolean true false

" Built-in functions
syn keyword benjitBuiltin print

" Comments
syn match benjitComment "#.*$"

" Operators
syn match benjitOperator "\(:=\|==\|<=\|>=\|<\|>\|+\|-\|\*\|//\)"

" Numbers
syn match benjitNumber "\<\d\+\>"

" Function/algorithm names — declaration site and call sites
syn match benjitFunction "\<\h\w*\>\s*("me=e-1
syn keyword benjitKeyword algorithm nextgroup=benjitFunctionDef skipwhite
syn match benjitFunctionDef "\h\w*" contained

" Link to standard highlight groups
hi def link benjitKeyword     Keyword
hi def link benjitBoolean     Boolean
hi def link benjitBuiltin     Function
hi def link benjitComment     Comment
hi def link benjitOperator    Operator
hi def link benjitNumber      Number
hi def link benjitFunction    Function
hi def link benjitFunctionDef Function

let b:current_syntax = "benjit"