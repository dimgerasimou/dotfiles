require('multiple-cursors').setup({
	pre_hook = function()
		vim.g.minipairs_disable = true
		require("cmp").setup({enabled=false})
	end,
	post_hook = function()
		vim.g.minipairs_disable = false
		require("cmp").setup({enabled=true})
	end,
})
