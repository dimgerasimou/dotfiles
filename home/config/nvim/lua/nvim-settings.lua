-- Compitability - filetypes:
vim.opt.encoding = 'UTF-8'
vim.bo.filetype = 'on'
vim.opt.undofile = true
vim.opt.clipboard = 'unnamedplus'
vim.opt.list = true
vim.opt.listchars = {
        space = "∙",
        -- eol = "↲",
        tab = "➞ ",
        trail = "•",
        extends = "❯",
        precedes = "❮",
        nbsp = "",
}

-- Line Control:
vim.opt.number = true
vim.opt.scrolloff = 10
vim.opt.relativenumber = true

-- Tabs:
vim.opt.tabstop = 8
vim.opt.shiftwidth = 8

-- Scrolling:
vim.o.mouse = 'a'
vim.opt.wrap = false
vim.opt.sidescroll = 1

-- Splits:
vim.opt.splitbelow = true
vim.opt.splitright = true
