let STANDART_STREAM_OUTPUT = 1
let TRUE = 1
let NUMBER_PRECISION = 5

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

function Main(arguments)
	while TRUE do
		let random_number = GetRandom()

		let message = ToString(random_number, NUMBER_PRECISION)
		message = Append(message, "\n")
		Show(message)
	end
end
