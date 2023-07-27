-- Keybinds
local vk = vim.keymap

-- map leader to <Space>
vk.set('n', ' ', '<Nop>', { silent = true, remap = false })
vim.g.mapleader = ' '

-- General
vk.set('n', '<leader>n', '<cmd>Neotree<CR>', {silent = true, noremap = true})

-- Markdown Preview
vk.set('n', '<leader>mm', '<cmd>MarkdownPreview<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>ms', '<cmd>MarkdownPreviewStop<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>mt', '<cmd>MarkdownPreviewStart<CR>', {silent = true, noremap = true})

-- Vimtex
vk.set('n', '<leader>tl', '<cmd>VimtexLog<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tv', '<cmd>VimtexView<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tc', '<cmd>VimtexClean<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tr', '<cmd>VimtexReload<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tt', '<cmd>VimtexStart<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>ts', '<cmd>VimtexStop<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>ta', '<cmd>VimtexStopAll<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tg', '<cmd>VimtexStatus<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>ti', '<cmd>VimtexInfo<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tca', '<cmd>VimtexCompileSS<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tcs', '<cmd>VimtexCompileSelected<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tcc', '<cmd>VimtexCompile<CR>', {silent = true, noremap = true})
vk.set('n', '<leader>tco', '<cmd>VimtexCompileOutput<CR>', {silent = true, noremap = true})

-- Nabla
vk.set('n', '<leader>p', require('nabla').popup)

-- Smart Splits
vk.set('n', '<A-h>',             require('smart-splits').resize_left)
vk.set('n', '<A-j>',             require('smart-splits').resize_down)
vk.set('n', '<A-k>',             require('smart-splits').resize_up)
vk.set('n', '<A-l>',             require('smart-splits').resize_right)
vk.set('n', '<C-h>',             require('smart-splits').move_cursor_left)
vk.set('n', '<C-j>',             require('smart-splits').move_cursor_down)
vk.set('n', '<C-k>',             require('smart-splits').move_cursor_up)
vk.set('n', '<C-l>',             require('smart-splits').move_cursor_right)
vk.set('n', '<leader><leader>h', require('smart-splits').swap_buf_left)
vk.set('n', '<leader><leader>j', require('smart-splits').swap_buf_down)
vk.set('n', '<leader><leader>k', require('smart-splits').swap_buf_up)
vk.set('n', '<leader><leader>l', require('smart-splits').swap_buf_right)

-- Aerial
vk.set('n', '<leader>a', '<cmd>AerialToggle!<CR>')

-- Trouble
vk.set("n", "<leader>xx", "<cmd>TroubleToggle<cr>",                       {silent = true, noremap = true})
vk.set("n", "<leader>xw", "<cmd>TroubleToggle workspace_diagnostics<cr>", {silent = true, noremap = true})
vk.set("n", "<leader>xd", "<cmd>TroubleToggle document_diagnostics<cr>",  {silent = true, noremap = true})
vk.set("n", "<leader>xl", "<cmd>TroubleToggle loclist<cr>",               {silent = true, noremap = true})
vk.set("n", "<leader>xq", "<cmd>TroubleToggle quickfix<cr>",              {silent = true, noremap = true})
vk.set("n", "gR",         "<cmd>TroubleToggle lsp_references<cr>",        {silent = true, noremap = true})

-- Autocommands

-- Session_Manager
vim.api.nvim_create_autocmd({ 'BufWritePost' }, {
  group = config_group,
  callback = function ()
    if vim.bo.filetype ~= 'git'
      and not vim.bo.filetype ~= 'gitcommit'
      and not vim.bo.filetype ~= 'gitrebase'
      then require('session_manager').autosave_session() end
  end
})

-- Alpha
vim.api.nvim_create_autocmd("User", {
	group = group,
	pattern = "AlphaReady",
	callback = function()
		vim.opt.laststatus = 0
		vim.opt.showcmd = false
		vim.opt.ruler = false
	end,
})

vim.api.nvim_create_autocmd("BufUnload", {
	group = group,
	pattern = "<buffer>",
	callback = function()
		vim.opt.laststatus = 3
		vim.opt.showcmd = true
		vim.opt.ruler = true
	end,
})

-- Highlight on Yank
vim.api.nvim_exec(
	[[
		augroup YankHighlight
		autocmd!
		autocmd TextYankPost * silent! lua vim.highlight.on_yank()
		augroup enable_diagnostics
	]],
	false
)
