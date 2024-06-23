require("neorg").setup({
	load = {
		["core.defaults"] = {},
		["core.concealer"] = {config = {icon_preset = "basic"}},
		["core.completion"] = {
			config = {
				engine = "nvim-cmp"
			}
		},
		["core.integrations.nvim-cmp"] = {},
		-- ["core.integrations.image"] = {},
		-- ["core.latex.renderer"] = {},
	},
})

vim.wo.foldlevel = 99
vim.wo.conceallevel = 2
