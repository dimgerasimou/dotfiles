local green = '#A6E3A1'
local blue = '#89B4FA'

local selfg = '#89B4FA'


vim.opt.termguicolors = true
local get_hex = require('cokeline/hlgroups').get_hl_attr

local comments_fg = get_hex('Comment', 'fg')
local errors_fg = get_hex('DiagnosticError', 'fg')
local warnings_fg = get_hex('DiagnosticWarn', 'fg')

local components = {
	delimiter = {
		text = "│",
		fg = function(buffer)
			return (buffer.is_focused and selfg) or nil
		end,
		style = function(buffer)
			return (buffer.is_focused and 'bold') or nil
		end,
	},
	space = {
		text = ' ',
		truncation = { priority = 1 },
	},
	spaceend = {
		text = function(buffer)
			return
			 (buffer.diagnostics.errors ~= 0 or buffer.diagnostics.warnings ~= 0) and ""
			 or " "
		end,
		truncation = { priority = 1 },
	},
	devicon = {
		text = function(buffer)
			return buffer.devicon.icon
		end,
		fg = function(buffer)
			return buffer.devicon.color
		end,
		truncation = { priority = 1 },
	},
	index = {
		text = function(buffer)
			return buffer.index .. ': '
		end,
		truncation = { priority = 1 },
	},
	unique_prefix = {
		text = function(buffer)
			return buffer.unique_prefix
		end,
		fg = function(buffer)
			return
			 (buffer.diagnostics.errors ~= 0 and errors_fg)
			 or (buffer.diagnostics.warnings ~= 0 and warnings_fg)
			 or comments_fg
		end,
		style = function(buffer)
			return
			(buffer.diagnostics.errors ~= 0 and 'underline,italic')
			 or 'italic'
		end,
		truncation = {
			priority = 3,
			direction = 'left',
		},
	},
	filename = {
		text = function(buffer)
			return buffer.filename
		end,
		fg = function(buffer)
			return
			 (buffer.diagnostics.errors ~= 0 and errors_fg)
			 or (buffer.diagnostics.warnings ~= 0 and warnings_fg)
			 or nil
		end,
		style = function(buffer)
			return
			 ((buffer.is_focused and buffer.diagnostics.errors ~= 0) and 'bold,underline')
			 or (buffer.is_focused and 'bold')
			 or (buffer.diagnostics.errors ~= 0 and 'underline')
			 or nil
		end,
		truncation = {
			priority = 2,
			direction = 'left',
		},
	},
	lsperrors = {
		text = function(buffer)
			local s = ""
			if (buffer.diagnostics.errors ~= 0) then
				s = " " .. buffer.diagnostics.errors .. " "
			end
			return s
		end,
		fg = errors_fg,
	},
	lspwarnings = {
		text = function(buffer)
			local s = ""
			if (buffer.diagnostics.warnings ~= 0) then
				s = " " .. buffer.diagnostics.warnings .. " "
			end
			return s
		end,
		fg = warnings_fg,
	},
	close_or_unsaved = {
		text = function(buffer)
			return buffer.is_modified and '●' or ' '
		end,
		fg = function(buffer)
			return buffer.is_modified and green or nil
		end,
		delete_buffer_on_left_click = true,
		truncation = { priority = 1 },
	},
}

require('cokeline').setup({
	show_if_buffers_are_at_least = 2,

	buffers = {
		focus_on_delete = 'next',
		new_buffers_position = 'next',
	},

	default_hl = {
		fg = function(buffer)
			return
			 buffer.is_focused
			 and get_hex('Normal', 'fg')
			 or get_hex('Comment', 'fg')
		end,
		bg = get_hex('Normal', 'bg')
	},

	sidebar = {
		filetype = 'neo-tree',
		components = {
			{
			text = '  Neotree',
			fg = blue,
			bg = get_hex('Normal', 'bg'),
			style = 'bold',
			},
		}
	},

	components = {
		components.delimiter,
		components.devicon,
		components.space,
		components.index,
		components.unique_prefix,
		components.filename,
		components.lsperrors,
		components.lspwarnings,
		components.spaceend,
		components.close_or_unsaved,
	},
})

