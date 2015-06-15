let NUMBER_PRECISION = 2
let STANDART_STREAM_OUTPUT = 1

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

function Append(array_1, array_2)
	let result = new array(GetSize(array_1) + GetSize(array_2))

	let index = 0
	while index < GetSize(result) do
		if index < GetSize(array_1) then
			result[index] = array_1[index]
		else
			result[index] = array_2[index - GetSize(array_1)]
		end

		index = index + 1
	end

	return result
end

function Show(text)
	Write(STANDART_STREAM_OUTPUT, text)
end

function Main(_)
	let number_1 = 23.5
	TestModuleFunction(number_1, Module(number_1), NUMBER_PRECISION)

	let number_2 = -23.5
	TestModuleFunction(number_2, Module(number_2), NUMBER_PRECISION)
end
