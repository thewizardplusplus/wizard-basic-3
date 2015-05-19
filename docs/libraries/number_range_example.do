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
	let range_1 = Range([1, 5])
	ShowRange(range_1)

	let range_2 = Range([1, 5, 2])
	Show("\n")
	ShowRange(range_2)

	let range_3 = Range([1, 1, 2])
	Show("\n")
	ShowRange(range_3)

	let range_4 = Range([5, 1, 2])
	Show("\n")
	ShowRange(range_4)

	let range_5 = Range([1, 5, 2, TRUE])
	Show("\n")
	ShowRange(range_5)

	let range_6 = Range([1, 1, 2, TRUE])
	Show("\n")
	ShowRange(range_6)

	let range_7 = Range([5, 1, 2, TRUE])
	Show("\n")
	ShowRange(range_7)
end
