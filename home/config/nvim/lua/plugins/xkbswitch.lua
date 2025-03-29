require('xkbswitch').setup({
	insert_focus_lost = false,
	post_hook = function()
		vim.fn.system("dwmblocksctl -s keyboard")
	end
})
