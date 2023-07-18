require('catppuccin').setup({
	flavour = "mocha", -- latte, frappe, macchiato, mocha
	background = {
		light = "latte",
		dark = "mocha",
	},
	transparent_background = true,
	show_end_of_buffer = false,
	term_colors = true,
	dim_inactive = {
		enabled = false,
		shade = "dark",
		percentage = 0.15,
	},
	no_italic = false,
	no_bold = false,
	no_underline = false,
	styles = {
		comments = { "italic" },
		conditionals = { "italic" },
		loops = {},
		functions = {},
		keywords = {},
		strings = {},
		variables = {},
		numbers = {},
		booleans = {},
		properties = {},
		types = {},
		operators = {},
	},
	color_overrides = {},
	custom_highlights = {},
	integrations = {
		cmp = true,
		gitsigns = true,
		neotree = true,
		treesitter = true,
		notify = false,
		indent_blankline = {
    			enabled = true,
    			colored_indent_levels = false,
		},
		mini=true,
		native_lsp = {
    			enabled = true,
    			virtual_text = {
        			errors = { "italic" },
        			hints = { "italic" },
        			warnings = { "italic" },
        			information = { "italic" },
			},
			underlines = {
				errors = { "underline" },
				hints = { "underline" },
				warnings = { "underline" },
				information = { "underline" },
			},
			inlay_hints = {
        			background = true,
    			},
		},
		-- treesiter_context=true,
		-- telescope = {
			--enabled = true,
		-- },
		which_key=true,
		-- trouble=true,
		-- ts_rainbow2 = true
	},
})

