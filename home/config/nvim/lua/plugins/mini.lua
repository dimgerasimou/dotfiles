require('mini.animate').setup({
	cursor = {
		enable = false,
		timing = require('mini.animate').gen_timing.linear({ duration = 150, unit = 'total' }),
		path = require('mini.animate').gen_path.line({
			predicate = function() return true end,

		})
	},
	scroll = {
		enable = false
	}
})

-- require('mini.bracketed').setup()
require('mini.bufremove').setup()
require('mini.cursorword').setup()
-- require('mini.move').setup()
require('mini.pairs').setup()
-- require('mini.splitjoin').setup()
require('mini.surround').setup()
