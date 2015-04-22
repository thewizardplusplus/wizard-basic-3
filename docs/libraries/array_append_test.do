include "array/utils"
include "io"

function Main(arguments)
	Show("\"-test 1-\" + \"-test 2-\" == \"")
	Show(Append("-test 1-", "-test 2-"))
	Show("\";\n")

	Show("\"\" + \"-test 2-\" == \"")
	Show(Append("", "-test 2-"))
	Show("\";\n")

	Show("\"-test 1-\" + \"\" == \"")
	Show(Append("-test 1-", ""))
	Show("\";\n")

	Show("\"\" + \"\" == \"")
	Show(Append("", ""))
	Show("\";\n")
end
