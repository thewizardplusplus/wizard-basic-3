include "array/utils"
include "array/number"
include "io"
include "array/string"

function Main(_)
	let array_1 = [1, 2, 3, 4, 5]
	Show(Append(ArrayNumberToString([array_1]), "\n"))

	let array_1_copy = array_1
	array_1_copy[2] = 6
	Show(Append(ArrayNumberToString([array_1]), "\n"))
	Show(Append(ArrayNumberToString([array_1_copy]), "\n"))
	Show("\n")

	let array_2 = [1, 2, 3, 4, 5]
	Show(Append(ArrayNumberToString([array_2]), "\n"))

	let array_2_copy = Copy(array_2)
	array_2_copy[2] = 6
	Show(Append(ArrayNumberToString([array_2]), "\n"))
	Show(Append(ArrayNumberToString([array_2_copy]), "\n"))
	Show("\n")

	let array_3 = ["ololo 1", "ololo 2"]
	Show(Append(ArrayStringToString(array_3), "\n"))

	let array_3_copy = array_3
	array_3_copy[0] = "test"
	array_3_copy[1][2] = '*'
	Show(Append(ArrayStringToString(array_3), "\n"))
	Show(Append(ArrayStringToString(array_3_copy), "\n"))
	Show("\n")

	let array_4 = ["ololo 1", "ololo 2"]
	Show(Append(ArrayStringToString(array_4), "\n"))

	let array_4_copy = Copy(array_4)
	array_4_copy[0] = "test"
	array_4_copy[1][2] = '*'
	Show(Append(ArrayStringToString(array_4), "\n"))
	Show(Append(ArrayStringToString(array_4_copy), "\n"))
	Show("\n")

	let array_5 = ["ololo 1", "ololo 2"]
	Show(Append(ArrayStringToString(array_5), "\n"))

	let array_5_copy = DeepCopy(array_5)
	array_5_copy[0] = "test"
	array_5_copy[1][2] = '*'
	Show(Append(ArrayStringToString(array_5), "\n"))
	Show(Append(ArrayStringToString(array_5_copy), "\n"))
	Show("\n")
end
