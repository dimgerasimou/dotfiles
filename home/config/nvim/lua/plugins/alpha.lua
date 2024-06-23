require("alpha").setup(require('plugins.alpha-theme').config)

-- Hide status line
vim.api.nvim_create_autocmd("User", {
	-- group = group,
	pattern = "AlphaReady",
	callback = function()
		vim.opt.laststatus = 0
		vim.opt.showcmd = false
		vim.opt.ruler = false
	end,
})

vim.api.nvim_create_autocmd("BufUnload", {
	-- group = group,
	pattern = "<buffer>",
	callback = function()
		vim.opt.laststatus = 3
		vim.opt.showcmd = true
		vim.opt.ruler = true
	end,
})
