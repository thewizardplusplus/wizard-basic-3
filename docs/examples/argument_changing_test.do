include "array/utils"
include "io"

let PRECISION = 2

function Module(number)
	if number < 0 then
		number = -number
	end

	return number
end

function TestModuleFunction(number, module, precision)
	let message = "Module of number "
	message = Append(message, ToString(number, precision))
	message = Append(message, " equal to ")
	message = Append(message, ToString(module, precision))
	message = Append(message, ".\n")

	Show(message)
end

function Main(_)
	let number_1 = 23.5
	TestModuleFunction(number_1, Module(number_1), PRECISION)

	let number_2 = -23.5
	TestModuleFunction(number_2, Module(number_2), PRECISION)
end
