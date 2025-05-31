require('catppuccin').setup({
	flavour = "auto", -- latte, frappe, macchiato, mocha
	transparent_background = false,
	term_colors = true,
	dim_inactive = {
		enabled = true,
		shade = "dark",
		percentage = 0.15,
	},
	integrations = {
		neotree = true,
		markdown = true,
		aerial = true,
		alpha = true,
		gitsigns = true,
		treesitter = true,
		-- headlines = true,
		-- cmp = true,
		-- mini = true,
		telescope = true,
		lsp_trouble = true,
		which_key = true,
		treesitter_context = true,
		rainbow_delimiters = true,
		indent_blankline = {
			enabled = true,
			colored_indent_levels = true,
		},
		native_lsp = {
			enabled = true,
			virtual_text = {
				errors = { "italic" },
				hints = { "italic" },
				warnings = { "italic" },
				information = { "italic" },
				ok = { "italic" },
			},
			underlines = {
				errors = { "underline" },
				hints = { "underline" },
				warnings = { "underline" },
				information = { "underline" },
				ok = { "underline" },
			},
			inlay_hints = {
				background = true,
			}
		},
	},
})
