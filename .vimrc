"  ____   ____ 
" |  _ \ / ___|    Dimitris Gerasimou (dimgerasimou)
" | | | | |  _     <https://github.com/dimgerasimou>
" | |_| | |_| |    
" |____/ \____|
"               
" Pretty thick vim configuration.

"-----------------------------------------------------------
"   => Compitability and Filetypes
"-----------------------------------------------------------

set nocompatible                "Disable compatibility with vi.
filetype on                     "Enable type file detection.
filetype plugin on              "Enable plugins and load plugin for the detected file type.
filetype indent on              "Load an indent file for the detected file type.

"-----------------------------------------------------------
"   => Highlighting
"-----------------------------------------------------------

syntax on                       "Turn syntax highlighting on.
"set cursorline                 "Highlight cursor line underneath the cursor horizontally.
"set cursorcolumn               "Highlight cursor line underneath the cursor vertically.

"-----------------------------------------------------------
"   => Line Control
"-----------------------------------------------------------

set number                      "Add numbers to each line.
set relativenumber              "Show numbers relative to the active line.
set scrolloff=10                "Do not let cursor scroll below or above N number of lines when scrolling.
set nowrap                      "Do not wrap lines. Allow long lines to extend as far as the line goes.

"-----------------------------------------------------------
"   => Search
"-----------------------------------------------------------

set incsearch                    "While searching though a file incrementally highlight matching characters as you type.
set ignorecase                   "Ignore capital letters during search.
set smartcase                    "Override the ignorecase option if searching for capital letters.
                                 "This will allow you to search specifically for capital letters.
set showmatch                    "Show matching words during a search.
set hlsearch                     "Use highlighting when doing a search.

"-----------------------------------------------------------
"   => Commands
"-----------------------------------------------------------

set showcmd                       "Show partial command you type in the last line of the screen.
set history=1000                  "Set the commands to save in history default number is 20.
set wildmenu                      "Enable auto completion menu after pressing TAB.
set wildmode=list:longest         "Make wildmenu behave like similar to Bash completion.
set wildignore=*.docx,*.jpg,*.png,*.gif,*.pdf,*.pyc,*.exe,*.flv,*.img,*.xlsx 
                                  "There are certain files that we would never want to edit with Vim.
                                  "Wildmenu will ignore files with these extensions.

"-----------------------------------------------------------
"   => Tabs
"-----------------------------------------------------------

set tabstop=4                     "Number of whitespaces a tab occupies.
set shiftwidth=4                  "Number of whitespaces a level of indentation occupies.

"-----------------------------------------------------------
"   => Line configuration
"-----------------------------------------------------------

set laststatus=2                  "Always display the status line.
set noshowmode                    "Do not show vim integrated status line.

let g:lightline = {
	\ 'colorscheme': 'powerline',
	\ 'active': {
	\   'left': [ [ 'mode', 'paste' ],
	\             [ 'gitbranch', 'readonly', 'filename', 'modified' ] ]
	\ },
	\ 'component_function': {
	\   'gitbranch': 'gitbranch#name'
	\ },
	\ }

"-----------------------------------------------------------
"   => Pluggins
"-----------------------------------------------------------

" Script to autoinstall vim-plugged

let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
  autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif

call plug#begin('~/.vim/plugged')

Plug 'dense-analysis/ale'        "Code correction
Plug 'itchyny/lightline.vim'     "Status line
Plug 'itchyny/vim-gitbranch'     "Git branch support for lightline
Plug 'preservim/nerdtree'        "Nerdtree expantion

call plug#end()

