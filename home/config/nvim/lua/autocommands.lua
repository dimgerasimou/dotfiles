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
