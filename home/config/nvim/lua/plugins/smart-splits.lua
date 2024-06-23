require('smart-splits').setup({
	resize_mode = {
		hooks = {
			on_leave = require('bufresize').register,
		},
	},
})
