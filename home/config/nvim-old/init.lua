--  ____   ____
-- |  _ \ / ___|    Dimitris Gerasimou (dimgerasimou)
-- | | | | |  _     <https://github.com/dimgerasimou>
-- | |_| | |_| |
-- |____/ \____|

require('plugins')
require('pluginsconfig')
require('utils')

local set = vim.opt

-- Compitability - filetypes:

set.encoding = 'UTF-8'
vim.bo.filetype = true
set.undofile = true
set.clipboard = 'unnamedplus'

-- Line Control:

set.number = true
set.scrolloff = 10
set.relativenumber = true

-- Tabs:

set.tabstop = 8
set.shiftwidth = 8

-- Scrolling:

vim.o.mouse = 'a'

---- For scrolling:

set.wrap = false
set.sidescroll = 1

---- For wraping:
--[[ 
set.breakindent = true
set.breakindentopt = 'sbr'
set.showbreak = '↪>\\'
 ]]

-- Splits:

set.splitbelow = true
set.splitright = true

-- Line:

set.showmode = false
set.guifont = 'Fira Code Nerd Font 11'
set.ruler = false

-- Colors:

set.background = dark
vim.cmd.colorscheme 'catppuccin'
