local get_visual = function(args, parent)
	if (#parent.snippet.env.LS_SELECT_RAW > 0) then
		return sn(nil, i(1, parent.snippet.env.LS_SELECT_RAW))
	else
		return sn(nil, i(1))
	end
end

local tex = {}
tex.in_mathzone = function() return vim.fn['vimtex#syntax#in_mathzone']() == 1 end
tex.in_text = function() return not tex.in_mathzone() end

return {
	-- SUPERSCRIPT SHORTCUT
	-- Places the first alphanumeric character after the trigger into a superscript.
	s({trig = '([%w%)%]%}])"([%w])', regTrig = true, wordTrig = false, snippetType="autosnippet"},
		fmta(
			"<>^{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				f( function(_, snip) return snip.captures[2] end ),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- SUBSCRIPT SHORTCUT
	-- Places the first alphanumeric character after the trigger into a subscript.
	s({trig = '([%w%)%]%}]):([%w])', regTrig = true, wordTrig = false, snippetType="autosnippet"},
		fmta(
			"<>_{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				f( function(_, snip) return snip.captures[2] end ),
			}
		),
		{condition = tex.in_mathzone}
	),

  	-- EULER'S NUMBER SUPERSCRIPT SHORTCUT
	s({trig = '([^%a])ee', regTrig = true, wordTrig = false, snippetType="autosnippet"},
		fmta(
			"<>e^{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual)
			}
		),
		{condition = tex.in_mathzone}
	),

	-- VECTOR, i.e. \vb
	s({trig = "([^%a])vv", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\vb{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- Vector Unit, i.e. \vu 
	s({trig = "([^%a])uu", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\vu{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- FRACTION
	s({trig = "([^%a])ff", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\frac{<>}{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual),
				i(2),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- ABSOLUTE VALUE
	s({trig = "([^%a])aa", regTrig = true, wordTrig = false, snippetType="autosnippet"},
		fmta(
			"<>\\ab |<>|",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- SQUARE ROOT
	s({trig = "([^%\\])sq", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\sqrt{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				d(1, get_visual),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- BINOMIAL SYMBOL
	s({trig = "([^%\\])bnn", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\binom{<>}{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				i(1),
				i(2),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- LOGARITHM WITH BASE SUBSCRIPT
	s({trig = "([^%a%\\])ll", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\log_{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				i(1),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- DERIVATIVE with numerator and denominator
	s({trig = "([^%a])dvv", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\odv{<>}{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				i(1),
				i(2)
			}
		),
		{condition = tex.in_mathzone}
	),

	-- PARTIAL DERIVATIVE with numerator and denominator
	s({trig = "([^%a])pvv", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\pdv{<>}{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				i(1),
				i(2)
			}
		),
		{condition = tex.in_mathzone}
	),

	-- SUM with upper and lower limit
	s({trig = "([^%a])smm", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\sum_{<>}^{<>}",
			{
				f( function(_, snip) return snip.captures[1] end ),
				i(1),
				i(2),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- INTEGRAL with upper and lower limit
	s({trig = "([^%a])intt", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\int_{<>}^{<>}",
			{
        			f( function(_, snip) return snip.captures[1] end ),
        			i(1),
        			i(2),
			}
		),
		{condition = tex.in_mathzone}
	),

	-- INTEGRAL from positive to negative infinity
	s({trig = "([^%a])intf", wordTrig = false, regTrig = true, snippetType="autosnippet"},
		fmta(
			"<>\\int_{\\infty}^{\\infty}",
			{
				f( function(_, snip) return snip.captures[1] end ),
			}
		),
		{condition = tex.in_mathzone}
	),

--
-- BEGIN STATIC SNIPPETS
--

	-- -- DIFFERENTIAL, i.e. \diff
	-- s({trig = "df", priority=2000, snippetType="autosnippet"},
	-- 	{ t("\\diff") },
	-- 	{ condition = tex.in_mathzone }
	-- ),

	-- BASIC INTEGRAL SYMBOL, i.e. \int
	s({trig = "in1", snippetType="autosnippet"},
    		{ t("\\int") },
		{ condition = tex.in_mathzone }
	),

	-- DOUBLE INTEGRAL, i.e. \iint
	s({trig = "in2", snippetType="autosnippet"},
		{ t("\\iint") },
   		{ condition = tex.in_mathzone }
	),

	-- TRIPLE INTEGRAL, i.e. \iiint
	s({trig = "in3", snippetType="autosnippet"},
		{ t("\\iiint") },
		{ condition = tex.in_mathzone }
	),

	-- CLOSED SINGLE INTEGRAL, i.e. \oint
	s({trig = "oi1", snippetType="autosnippet"},
		{ t("\\oint") },
		{ condition = tex.in_mathzone }
	),

	-- CLOSED DOUBLE INTEGRAL, i.e. \oiint
	s({trig = "oi2", snippetType="autosnippet"},
		{ t("\\oiint") },
		{ condition = tex.in_mathzone }
	),

	-- GRADIENT OPERATOR, i.e. \grad
	s({trig = "gdd", snippetType="autosnippet"},
		{ t("\\grad ") },
		{ condition = tex.in_mathzone }
	),

	-- CURL OPERATOR, i.e. \curl
	s({trig = "cll", snippetType="autosnippet"},
		{ t("\\curl ") },
		{ condition = tex.in_mathzone }
	),

	-- DIVERGENCE OPERATOR, i.e. \divergence
	s({trig = "DI", snippetType="autosnippet"},
		{ t("\\div ") },
		{ condition = tex.in_mathzone }
	),

	-- LAPLACIAN OPERATOR, i.e. \laplacian
	s({trig = "laa", snippetType="autosnippet"},
		{ t("\\laplacian ") },
		{ condition = tex.in_mathzone }
	),

	-- PARALLEL SYMBOL, i.e. \parallel
	s({trig = "||", snippetType="autosnippet"},
		{ t("\\parallel") }
	),

	-- CDOTS, i.e. \cdots
	s({trig = "cdd", snippetType="autosnippet"},
		{ t("\\cdots") }
	),

	-- LDOTS, i.e. \ldots
	s({trig = "ldd", snippetType="autosnippet"},
		{ t("\\ldots") }
	),

	-- EQUIV, i.e. \equiv
	s({trig = "eqq", snippetType="autosnippet"},
		{ t("\\equiv ") }
	),

	-- SETMINUS, i.e. \setminus
	s({trig = "stm", snippetType="autosnippet"},
		{ t("\\setminus ") }
	),

	-- SUBSET, i.e. \subset
	s({trig = "sbb", snippetType="autosnippet"},
		{ t("\\subset ") }
	),

	-- APPROX, i.e. \approx
	s({trig = "px", snippetType="autosnippet"},
		{ t("\\approx ") },
	  {condition = tex.in_mathzone}
	),

	-- PROPTO, i.e. \propto
	s({trig = "pt", snippetType="autosnippet"},
		{ t("\\propto ") },
	  {condition = tex.in_mathzone}
	),

	-- COLON, i.e. \colon
	s({trig = "::", snippetType="autosnippet"},
		{ t("\\colon ") }
	),

	-- IMPLIES, i.e. \implies
	s({trig = ">>", snippetType="autosnippet"},
		{ t("\\implies ") }
	),

	-- DOT PRODUCT, i.e. \cdot
	s({trig = ",.", snippetType="autosnippet"},
		{ t("\\cdot ") }
	),

	-- CROSS PRODUCT, i.e. \times
	s({trig = "xx", snippetType="autosnippet"},
		{ t("\\times ") }
	),
}
