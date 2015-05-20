include "array/number"
include "io"

let COMPARE_RESULT_NAMES = ["less then", "equal to", "greater then"]

function CompareResultToString(compare_result)
	return COMPARE_RESULT_NAMES[compare_result + 1]
end

function TestCompare(string_1, string_2)
	let compare_result = Compare(string_1, string_2)

	let msg = "\""
	msg = Append(msg, string_1)
	msg = Append(msg, "\" ")
	msg = Append(msg, CompareResultToString(compare_result))
	msg = Append(msg, " \"")
	msg = Append(msg, string_2)
	msg = Append(msg, "\";\n")

	Show(msg)
end

function Main(arguments)
	TestCompare("test", "ololo")
	TestCompare("ololo", "test")
	TestCompare("test", "test new")
	TestCompare("test new", "test")
	TestCompare("test old", "test new")
	TestCompare("test new", "test old")
	TestCompare("test", "test")
	TestCompare("", "test")
	TestCompare("test", "")
	TestCompare("", "")
end
