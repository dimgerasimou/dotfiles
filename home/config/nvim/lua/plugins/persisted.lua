require("persisted").setup {
	use_git_branch = true,
	should_autosave = function()
		if vim.bo.filetype == "alpha" then
			return false
		end

		return true
	end,
}

