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

"Expand tabs to spaces.

"set expandtab
set tabstop=4
set shiftwidth=4

"-----------------------------------------------------------
"   => Pluggins
"-----------------------------------------------------------

" lightline theme

let g:lightline = {
    \ 'colorshceme': 'deus',
    \ }

" Script to autoinstall vim-plugged

let data_dir = has('nvim') ? stdpath('data') . '/site' : '~/.vim'
if empty(glob(data_dir . '/autoload/plug.vim'))
  silent execute '!curl -fLo '.data_dir.'/autoload/plug.vim --create-dirs  https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
  autocmd VimEnter * PlugInstall --sync | source $MYVIMRC
endif

call plug#begin('~/.vim/plugged')

"{{ Git Utillities }}
Plug 'airblade/vim-gitgutter'

"{{ Code correction }}
Plug 'dense-analysis/ale'

"{{ Status line }}
Plug 'itchyny/lightline.vim'

"{{ Extra }}
Plug 'preservim/nerdtree'

call plug#end()

"Lightline configuration.
set laststatus=2
set noshowmode
