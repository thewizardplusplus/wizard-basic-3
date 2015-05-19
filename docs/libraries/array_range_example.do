include "range/utils"
include "io"

function ShowRange(range)
	let msg = "size: "
	msg = Append(msg, ToString(range.size, 0))
	msg = Append(msg, "\n")
	Show(msg)

	while Next(range) do
		msg = "item #"
		msg = Append(msg, ToString(range.index, 0))
		msg = Append(msg, ": ")
		msg = Append(msg, ToString(range.value, 2))
		if range.first then
			msg = Append(msg, " <= it's first")
		else if range.last then
			msg = Append(msg, " <= it's last")
		end
		msg = Append(msg, "\n")
		Show(msg)
	end
end

function Main(_)
	let array = [10, 11, 12, 13, 14, 15]

	let range_1 = Range([array])
	ShowRange(range_1)

	let range_2 = Range([array, 2])
	Show("\n")
	ShowRange(range_2)

	let range_3 = Range([array, 1, 4])
	Show("\n")
	ShowRange(range_3)

	let range_4 = Range([array, 2, 2])
	Show("\n")
	ShowRange(range_4)

	let range_5 = Range([[]])
	Show("\n")
	ShowRange(range_5)

	let range_6 = Range([array, 1, 4, TRUE])
	Show("\n")
	ShowRange(range_6)

	let range_7 = Range([array, 0, GetSize(array), TRUE])
	Show("\n")
	ShowRange(range_7)

	let range_8 = Range([array, 2, 2, TRUE])
	Show("\n")
	ShowRange(range_8)

	let range_9 = Range([[], 0, 0, TRUE])
	Show("\n")
	ShowRange(range_9)
end
