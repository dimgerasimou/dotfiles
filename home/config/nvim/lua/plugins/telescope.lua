require('telescope').setup {
	extensions = {
		persisted = {
			layout_config = { width = 0.55, height = 0.55 }
		},
		fzf = {
			fuzzy = true,
			override_generic_sorter = true,
			case_mode = "smart_case",
		},
		aerial = {
			show_nesting = {
				["_"] = false,
				json = true,
				yaml = true,
			},
		},
	}
}

require('telescope').load_extension('persisted')
require('telescope').load_extension('fzf')
require('telescope').load_extension('aerial')
