let STANDART_STREAM_OUTPUT = 1

function StringArrayToString(array)
	let result = "[\""

	let i = 0
	while i < GetSize(array) do
		if i > 0 then
			result = Append(result, "\", \"")
		end
		result = Append(result, array[i])

		i = i + 1
	end

	result = Append(result, "\"]")
	return result
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

function Main(arguments)
	let message = StringArrayToString(arguments)
	message = Append(message, "\n")
	Show(message)
end
