let NUMBER_PRECISION = 0
let STANDART_STREAM_OUTPUT = 1

structure Vector2D
	x
	y
end

function Vector2DNew(x, y)
	let result = new Vector2D
	result.x = x
	result.y = y

	return result
end

function Vector2DAdd(vector_1, vector_2)
	return Vector2DNew(vector_1.x + vector_2.x, vector_1.y + vector_2.x)
end

function Vector2DToString(vector, number_precision)
	let result = "Vector("
	result = Append(result, ToString(vector.x, number_precision))
	result = Append(result, "; ")
	result = Append(result, ToString(vector.y, number_precision))
	result = Append(result, ")")

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
	let vector_1 = Vector2DNew(1, 2)
	let vector_2 = Vector2DNew(3, 4)
	let vector_sum = Vector2DAdd(vector_1, vector_2)

	let message = Vector2DToString(vector_sum, NUMBER_PRECISION)
	message = Append(message, "\n")
	Show(message)
end
