include "array/utils"
include "array/number"
include "io"

function Main(_)
	let array = [1, 2, 3, 4, 5]
	ShowLn(ArrayNumberToString([array]))

	let slice_1 = Slice([array, 1, 4])
	ShowLn(ArrayNumberToString([slice_1]))

	let slice_2 = Slice([array, 1])
	ShowLn(ArrayNumberToString([slice_2]))

	let slice_3 = Slice([array])
	ShowLn(ArrayNumberToString([slice_3]))

	let slice_4 = Slice([array, 2, 2])
	ShowLn(ArrayNumberToString([slice_4]))
end
