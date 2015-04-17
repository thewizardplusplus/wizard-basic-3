include "array/number"
include "array/string"
include "io"

function IndexToString(index)
	if index /= NULL then
		return ToString(index, 0)
	else
		return "NULL"
	end
end

function Main(arguments)
	let number_array = [1, 2, 3, 4, 5, 4, 3, 2, 1]

	Show("2 in [1, 2, 3, 4, 5, 4, 3, 2, 1] on ")
	Show(IndexToString(ArrayNumberIndexOf([number_array, 2])))
	Show(" position;\n")

	Show("2 in [1, 2, 3, 4, 5, 4, 3, 2, 1] with offset 3 on ")
	Show(IndexToString(ArrayNumberIndexOf([number_array, 2, 3])))
	Show(" position;\n")

	Show("6 in [1, 2, 3, 4, 5, 4, 3, 2, 1] on ")
	Show(IndexToString(ArrayNumberIndexOf([number_array, 6])))
	Show(" position;\n")

	Show("2 in [] on ")
	Show(IndexToString(ArrayNumberIndexOf([[], 2])))
	Show(" position;\n")

	Show("2 in [1, 2, 3, 4, 5, 4, 3, 2, 1] on ")
	Show(IndexToString(ArrayNumberLastIndexOf([number_array, 2])))
	Show(" position from end;\n")

	Show("2 in [1, 2, 3, 4, 5, 4, 3, 2, 1] with offset 3 on ")
	Show(IndexToString(ArrayNumberLastIndexOf([number_array, 2, 3])))
	Show(" position from end;\n")

	Show("6 in [1, 2, 3, 4, 5, 4, 3, 2, 1] on ")
	Show(IndexToString(ArrayNumberLastIndexOf([number_array, 6])))
	Show(" position from end;\n")

	Show("2 in [] on ")
	Show(IndexToString(ArrayNumberLastIndexOf([[], 2])))
	Show(" position from end;\n")

	Show("\"lo\" in \"ollollol\" on ")
	Show(IndexToString(ArrayNumberIndexOf(["ollollol", "lo"])))
	Show(" position;\n")

	Show("\"lo\" in \"ollollol\" with offset 3 on ")
	Show(IndexToString(ArrayNumberIndexOf(["ollollol", "lo", 3])))
	Show(" position;\n")

	Show("\"test\" in \"ollollol\" on ")
	Show(IndexToString(ArrayNumberIndexOf(["ollollol", "test"])))
	Show(" position;\n")

	Show("\"lo\" in \"\" on ")
	Show(IndexToString(ArrayNumberIndexOf(["", "lo"])))
	Show(" position;\n")

	Show("\"lo\" in \"ollollol\" on ")
	Show(IndexToString(ArrayNumberLastIndexOf(["ollollol", "lo"])))
	Show(" position from end;\n")

	Show("\"lo\" in \"ollollol\" with offset 2 on ")
	Show(IndexToString(ArrayNumberLastIndexOf(["ollollol", "lo", 2])))
	Show(" position from end;\n")

	Show("\"test\" in \"ollollol\" on ")
	Show(IndexToString(ArrayNumberLastIndexOf(["ollollol", "test"])))
	Show(" position from end;\n")

	Show("\"lo\" in \"\" on ")
	Show(IndexToString(ArrayNumberLastIndexOf(["", "lo"])))
	Show(" position from end;\n")

	let string_array = ["one", "two", "three", "two", "one"]

	Show("\"two\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] on ")
	Show(IndexToString(ArrayStringIndexOf([string_array, "two"])))
	Show(" position;\n")

	Show("\"two\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] ")
	Show("with offset 2 on ")
	Show(IndexToString(ArrayStringIndexOf([string_array, "two", 2])))
	Show(" position;\n")

	Show("\"five\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] on ")
	Show(IndexToString(ArrayStringIndexOf([string_array, "five"])))
	Show(" position;\n")

	Show("\"two\" in [] on ")
	Show(IndexToString(ArrayStringIndexOf([[], "two"])))
	Show(" position;\n")

	Show("\"two\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] on ")
	Show(IndexToString(ArrayStringLastIndexOf([string_array, "two"])))
	Show(" position from end;\n")

	Show("\"two\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] ")
	Show("with offset 2 on ")
	Show(IndexToString(ArrayStringLastIndexOf([string_array, "two", 2])))
	Show(" position from end;\n")

	Show("\"five\" in [\"one\", \"two\", \"three\", \"two\", \"one\"] on ")
	Show(IndexToString(ArrayStringLastIndexOf([string_array, "five"])))
	Show(" position from end;\n")

	Show("\"two\" in [] on ")
	Show(IndexToString(ArrayStringLastIndexOf([[], "two"])))
	Show(" position from end;\n")
end
