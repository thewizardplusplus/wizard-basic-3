include "array/utils"
include "io"

function TestAppend(string_1, string_2)
	let append_result = Append(string_1, string_2)

	let msg = "\""
	msg = Append(msg, string_1)
	msg = Append(msg, "\" + \"")
	msg = Append(msg, string_2)
	msg = Append(msg, "\" == \"")
	msg = Append(msg, append_result)
	msg = Append(msg, "\"\n")

	Show(msg)
end

function TestMultiAppend(strings)
	let result = MultiAppend(strings)

	let msg = "\""
	msg = Append(msg, result)
	msg = Append(msg, "\"\n")

	Show(msg)
end

function Main(arguments)
	TestAppend("-test 1-", "-test 2-")
	TestAppend("", "-test 2-")
	TestAppend("-test 1-", "")
	TestAppend("", "")
	Show("\n")

	TestMultiAppend(["one-", "two-", "three-", "four", "five"])
	TestMultiAppend(["one-", "two-", ""])
	TestMultiAppend(["one-", "two-", "", "", ""])
	TestMultiAppend(["", "", "", "", ""])
	TestMultiAppend([])
end
