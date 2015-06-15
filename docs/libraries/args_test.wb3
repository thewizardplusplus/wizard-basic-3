include "array/utils"
include "null"
include "args"
include "io"

function ShowArg(arg, i)
	let msg = "argument #"
	msg = Append(msg, ToString(i, 0))
	msg = Append(msg, ": \"")
	msg = Append(msg, IfNull(arg, "NULL"))
	msg = Append(msg, "\"\n")

	Show(msg)
end

function Main(_)
	let array = ["one", "two", "three", "four", "five"]

	let arg_1 = Arg([array, 0])
	ShowArg(arg_1, 0)

	let arg_2 = Arg([array, 2])
	ShowArg(arg_2, 2)

	let arg_3 = Arg([array, 4])
	ShowArg(arg_3, 4)

	let arg_4 = Arg([array, 6])
	ShowArg(arg_4, 6)

	let arg_5 = Arg([array, 6, "nonexistent"])
	ShowArg(arg_5, 6)
end
