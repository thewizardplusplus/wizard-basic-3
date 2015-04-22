include "array/number"
include "io"

let COMPARE_RESULT_NAMES = ["less then", "equal to", "greater then"]

function CompareResultToString(compare_result)
	return COMPARE_RESULT_NAMES[compare_result + 1]
end

function Main(arguments)
	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "ololo")))
	Show(" \"ololo\";\n")

	Show("\"ololo\" ")
	Show(CompareResultToString(Compare("ololo", "test")))
	Show(" \"test\";\n")

	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "test new")))
	Show(" \"test new\";\n")

	Show("\"test new\" ")
	Show(CompareResultToString(Compare("test new", "test")))
	Show(" \"test\";\n")

	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "tes")))
	Show(" \"tes\";\n")

	Show("\"tes\" ")
	Show(CompareResultToString(Compare("tes", "test")))
	Show(" \"test\";\n")

	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "test")))
	Show(" \"test\";\n")

	Show("\"\" ")
	Show(CompareResultToString(Compare("", "")))
	Show(" \"\";\n")
end
