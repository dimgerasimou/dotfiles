""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
"      #                                                                                   "
"      #  #                                                  #                             "
"      #                                                                                   "
"  ### #  #  # ##   ##     ###    ###   # ##   ### #   ###   #  # ##   ##     ###   #    # "
" #   ##  #  ##  # #  #   #   #  #   #  ##    #   ##  #      #  ##  # #  #   #   #  #    # "
" #    #  #  #    #    #  #   #  ####   #     #   ##   ###   #  #    #    #  #   #  #    # "
" #   ##  #  #    #    #  #   #  #      #     #   ##      #  #  #    #    #  #   #  #    # "
"  ### #  #  #    #    #   ####   ###   #      ### #   ###   #  #    #    #   ###    ####  "
"                             #                                                            "
"                         #   #                                                            "
"                          ###                                                             "
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

".VIMRC, VIM configuration.

"Scripts ---------------------------------------------------------------- {{{


"COMPITABILITY AND FILETYPES -----------------------------


"Disable compatibility with vi.
set nocompatible

"Enable type file detection.
filetype on

"Enable plugins and load plugin for the detected file type.
filetype plugin on

"Load an indent file for the detected file type.
filetype indent on


"HIGHLIGHTING -------------------------------------------


"Turn syntax highlighting on.
syntax on

"Highlight cursor line underneath the cursor horizontally.
"set cursorline

"Highlight cursor line underneath the cursor vertically.
"set cursorcolumn


"LINE CONTROL ------------------------------------------


"Add numbers to each line.
set number

"Do not let cursor scroll below or above N number of lines when scrolling.
set scrolloff=10

"Do not wrap lines. Allow long lines to extend as far as the line goes.
set nowrap


"SEARCH ------------------------------------------------


"While searching though a file incrementally highlight matching characters as you type.
set incsearch

"Ignore capital letters during search.
set ignorecase

"Override the ignorecase option if searching for capital letters.
"This will allow you to search specifically for capital letters.
set smartcase

"Show matching words during a search.
set showmatch

"Use highlighting when doing a search.
set hlsearch


"COMMANDS ----------------------------------------------


"Show partial command you type in the last line of the screen.
set showcmd

"Set the commands to save in history default number is 20.
set history=1000

"Enable auto completion menu after pressing TAB.
set wildmenu

"Make wildmenu behave like similar to Bash completion.
set wildmode=list:longest

"There are certain files that we would never want to edit with Vim.
"Wildmenu will ignore files with these extensions.
set wildignore=*.docx,*.jpg,*.png,*.gif,*.pdf,*.pyc,*.exe,*.flv,*.img,*.xlsx


"TABS -----------------------------------------------------


"Expand tabs to spaces.
"set expandtab

set tabstop=4
set shiftwidth=4

"}}}

"PLUGINS ---------------------------------------------------------------- {{{

"VIM-PLUG CONFIGURATION

let g:lightline = {
    \ 'colorshceme': 'deus',
    \ }

let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
  autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif

call plug#begin('~/.vim/plugged')

Plug 'airblade/vim-gitgutter'
Plug 'itchyny/lightline.vim'
Plug 'dense-analysis/ale'
Plug 'preservim/nerdtree'

call plug#end()

"Lightline configuration.
set laststatus=2
set noshowmode

"}}}

"MAPPINGS --------------------------------------------------------------- {{{

"}}}

"VIMSCRPIT -------------------------------------------------------------- {{{

"Code folding
augroup filetype_vim
    autocmd!
    autocmd FileType vim setlocal foldmethod=marker
augroup END
"}}}

"STATUS LINE ------------------------------------------------------------ {{{

"}}}
