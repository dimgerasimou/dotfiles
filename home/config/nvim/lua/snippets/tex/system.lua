local get_visual = function(args, parent)
	if (#parent.snippet.env.LS_SELECT_RAW > 0) then
		return sn(nil, i(1, parent.snippet.env.LS_SELECT_RAW))
	else
		return sn(nil, i(1))
	end
end

local line_begin = require("luasnip.extras.expand_conditions").line_begin

-- Math context detection 
local tex = {}
tex.in_mathzone = function() return vim.fn['vimtex#syntax#in_mathzone']() == 1 end
tex.in_text = function() return not tex.in_mathzone() end

-- Return snippet tables
return
{
	-- DOCUMENTCLASS
	s({trig = "dcc", snippetType="autosnippet"},
		fmta(
			[=[
			\documentclass[<>]{<>}
			]=],
			{
				i(1, "a4paper"),
				i(2, "article"),
			}
		),
		{condition = line_begin}
	),

	-- USE A LATEX PACKAGE
	s({trig = "pack", snippetType="autosnippet"},
		fmta(
			[[
			\usepackage{<>}
			]],
			{d(1, get_visual)}
		),
		{condition = line_begin}
	),

	-- LABEL
	s({trig = "lbl", snippetType="autosnippet"},
		fmta(
			[[
			\label{<>}
			]],
			{d(1, get_visual)}
		)
	),

	s({trig="sii", snippetType="autosnippet"},
		fmta(
			[[\si{<>}]],
			{i(1)}
		)
	),

	s({trig="SI"},
		fmta(
			[[\SI{<>}{<>}]],
			{
				i(1),
				i(2)
			}
		)
	),

	-- HSPACE
	s({trig="hss", snippetType="autosnippet"},
		fmta(
			[[\hspace{<>}]],
			{d(1, get_visual)}
		)
	),

	-- VSPACE
	s({trig="vss", snippetType="autosnippet"},
		fmta(
			[[\vspace{<>}]],
			{d(1, get_visual)}
		)
	),

	-- SECTION
	s({trig="h1", snippetType="autosnippet"},
		fmta(
			[[\section{<>}]],
			{d(1, get_visual)}
		)
	),

	-- SUBSECTION
	s({trig="h2", snippetType="autosnippet"},
		fmta(
			[[\subsection{<>}]],
			{d(1, get_visual)}
		)
	),

	-- SUBSUBSECTION
	s({trig="h3", snippetType="autosnippet"},
		fmta(
			[[\subsubsection{<>}]],
			{d(1, get_visual)}
		)
	),
}
