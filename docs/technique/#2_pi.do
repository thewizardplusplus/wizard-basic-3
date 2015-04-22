note Gauss-Legendre algorithm.

let STANDART_STREAM_OUTPUT = 1
let TRUE = 1
let EPSILON = 0.000001
let NUMBER_PRECISION = 5

function Module(number)
	let result = number
	if result < 0 then
		result = -result
	end

	return result
end

function Append(array_1, array_2)
	let result = new array(GetLength(array_1) + GetLength(array_2))

	let index = 0
	while index < GetLength(result) do
		if index < GetLength(array_1) then
			result[index] = array_1[index]
		else
			result[index] = array_2[index - GetLength(array_1)]
		end

		index = index + 1
	end

	return result
end

function Show(text)
	Write(STANDART_STREAM_OUTPUT, text)
end

function Main(arguments)
	let a = 1
	let b = 1 / SquareRoot(2)
	let t = 1 / 4
	let p = 1

	while TRUE do
		let new_a = (a + b) / 2
		let new_b = SquareRoot(a * b)
		let new_t = t - p * (a - new_a) * (a - new_a)
		let new_p = 2 * p

		a = new_a
		b = new_b
		t = new_t
		p = new_p

		if Module(a - b) <= EPSILON then
			break
		end
	end

	let pi = (a + b) * (a + b) / (4 * t)

	let message = "Pi = "
	message = Append(message, ToString(pi, NUMBER_PRECISION))
	message = Append(message, ".\n")
	Show(message)
end
