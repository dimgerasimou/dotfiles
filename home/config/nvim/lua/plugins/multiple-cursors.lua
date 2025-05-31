require('multiple-cursors').setup({
	pre_hook = function()
		-- vim.g.minipairs_disable = true
		vim.b.completion = false
	end,
	post_hook = function()
		-- vim.g.minipairs_disable = false
		vim.b.completion = false
	end,
})
