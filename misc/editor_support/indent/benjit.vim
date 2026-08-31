" indent/benjit.vim
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=BenjitIndent()
setlocal indentkeys+=0=end
setlocal autoindent

if exists("*BenjitIndent")
  finish
endif

function! BenjitIndent()
  let lnum = prevnonblank(v:lnum - 1)
  if lnum == 0
    return 0
  endif

  let prevline = getline(lnum)
  let curline = getline(v:lnum)
  let ind = indent(lnum)

  " Strip comments and trailing whitespace for keyword checks
  let prevstripped = substitute(prevline, '#.*$', '', '')
  let prevstripped = substitute(prevstripped, '\s\+$', '', '')

  " Increase indent after lines ending a block opener:
  " 'algorithm ... is', 'if ... then', 'for ... do'
  if prevstripped =~ '\<is\s*$' || prevstripped =~ '\<then\s*$' || prevstripped =~ '\<do\s*$'
    let ind += &shiftwidth
  endif

  " Decrease indent if the current line is 'end'
  if curline =~ '^\s*end\>'
    let ind -= &shiftwidth
  endif

  return ind < 0 ? 0 : ind
endfunction