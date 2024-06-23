require('xkbswitch').setup({
	no_focus_lost_in_insert = true,
	set_layout_hook = function()
		vim.fn.system("dwmblocksctl -s keyboard")
	end
})
