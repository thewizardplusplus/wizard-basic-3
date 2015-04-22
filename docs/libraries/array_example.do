include "array/number"
include "array/utils"
include "array/string"
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
	Show(CompareResultToString(Compare("test", "test 1")))
	Show(" \"test 1\";\n")

	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "test")))
	Show(" \"test\";\n")

	Show("\"test\" ")
	Show(CompareResultToString(Compare("test", "tes")))
	Show(" \"tes\";\n")

	Show("\"test\" + \"ololo\" == \"")
	Show(Append("test", "ololo"))
	Show("\";\n")

	Show("\"\" + \"ololo\" == \"")
	Show(Append("", "ololo"))
	Show("\";\n")

	Show("Slice(\"test\", 1, 4) == \"")
	Show(Slice("test", 1, 4))
	Show("\";\n")

	Show(ArrayNumberToString([1.2, 3.4, 5.6, 7.8, 9.0], 1))
	Show(";\n")

	Show(ArrayStringToString(["one", "two", "three", "four", "five"]))
	Show(";\n")

	Show("ArrayNumberIndexOf(\"ololo\", 'l') == \"")
	Show(ToString(ArrayNumberIndexOf("ololo", 'l'), 0))
	Show("\";\n")

	Show("ArrayNumberNextIndexOf(\"ololo\", 'l', 2) == \"")
	Show(ToString(ArrayNumberNextIndexOf("ololo", 'l', 2), 0))
	Show("\";\n")

	Show("ArrayNumberLastIndexOf(\"ololo\", 'l') == \"")
	Show(ToString(ArrayNumberLastIndexOf("ololo", 'l'), 0))
	Show("\";\n")

	Show("ArrayNumberPreviousLastIndexOf(\"ololo\", 'l', 2) == \"")
	Show(ToString(ArrayNumberPreviousLastIndexOf("ololo", 'l', 2), 0))
	Show("\";\n")

	Show("ArrayStringIndexOf(\"test long long test\", \"long\") == \"")
	Show(ToString(ArrayStringIndexOf("test long long test", "long"), 0))
	Show("\";\n")

	Show("ArrayStringNextIndexOf(\"test long long test\", \"long\", 6) == \"")
	Show(ToString(ArrayStringNextIndexOf("test long long test", "long", 6), 0))
	Show("\";\n")

	Show("ArrayStringLastIndexOf(\"test long long test\", \"long\") == \"")
	Show(ToString(ArrayStringLastIndexOf("test long long test", "long"), 0))
	Show("\";\n")

	Show("ArrayStringPreviousLastIndexOf(\"test long long test\", \"long\", 9) == \"")
	Show(ToString(ArrayStringPreviousLastIndexOf("test long long test", "long", 9), 0))
	Show("\";\n")

	Show("Split(\"test 1, test 2, test 3, test 4, test 5\", \", \") == ")
	Show(ArrayStringToString(Split("test 1, test 2, test 3, test 4, test 5", ", ")))
	Show(";\n")
end
