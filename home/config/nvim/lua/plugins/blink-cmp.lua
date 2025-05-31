local is_enabled = function()
	local disabled_ft = {
		"TelescopePrompt",
		"grug-far",
	}
	return not vim.tbl_contains(disabled_ft, vim.bo.filetype)
	       and vim.b.completion ~= false
	       and vim.bo.buftype ~= "prompt"
end

require("blink-cmp").setup{
	enabled = is_enabled,
	snippets = { preset = 'luasnip' },
	fuzzy = { implementation = "prefer_rust_with_warning" },
	cmdline = { completion = { menu = { auto_show = true } } },
	completion = {
		menu = {
			scrollbar = false,
			auto_show = is_enabled,
			border = {
				"╭",
				"─",
				"╮",
				"│",
				"╯",
				"─",
				"╰",
				"│",
			},
		},
		list = {
			selection = { preselect = false, auto_insert = false },
		},
	},
	sources = {
		default = { 'lsp', 'path', 'snippets', 'buffer' },
	},
	keymap = {
		preset = 'default',
		['<CR>'] = { 'accept', 'fallback' },
	},
}
