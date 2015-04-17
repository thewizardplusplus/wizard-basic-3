include "range/utils"
include "io"

function Main(_)
	let array = [11, 12, 13, 14, 15]
	let i = Range([array, 1, 4])
	Show("Range size: ")
	Show(ToString(i.size, 0))
	Show(".\n")

	while Next(i) do
		Show("#")
		Show(ToString(i.index, 0))
		Show(": ")
		Show(ToString(i.value, 1))
		if i.first then
			Show(" - it's first")
		else if i.last then
			Show(" - it's last")
		end
		Show(";\n")
	end
end
