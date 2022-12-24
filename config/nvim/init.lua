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

-- Line Control:

set.number = true
set.scrolloff = 10

-- Tabs:

set.tabstop = 8
set.shiftwidth = 8

-- Scrolling:

vim.cmd( 'set mouse=a' )

-- Splits:

set.splitbelow = true
set.splitright = true

-- Line:

set.showmode = false
set.guifont = 'FiraCode Nerd Font 11'

-- Colors:

set.background = dark
vim.cmd('colorscheme rose-pine')
