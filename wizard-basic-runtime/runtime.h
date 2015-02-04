#ifndef WIZARD_BASIC_3_RUNTIME_HEADER
#define WIZARD_BASIC_3_RUNTIME_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

/*******************************************************************************
 * Messages.
 ******************************************************************************/
typedef enum MessageType {
	MESSAGE_TYPE_INFO,
	MESSAGE_TYPE_WARNING,
	MESSAGE_TYPE_ERROR
} MessageType;

void ProcessMessage(const MessageType type, const char* message) {
	switch (type) {
		case MESSAGE_TYPE_WARNING:
			fprintf(stderr, "Warning! %s", message);
			break;
		case MESSAGE_TYPE_ERROR:
			fprintf(stderr, "Error! %s", message);
			exit(EXIT_FAILURE);
		default:
			puts(message);
			break;
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types.
 ******************************************************************************/
typedef enum ValueType {
	VALUE_TYPE_NULL = 1 << 0,
	VALUE_TYPE_NUMBER = 1 << 1,
	VALUE_TYPE_ARRAY = 1 << 2,
	VALUE_TYPE_STRUCTURE = 1 << 3
} ValueType;

typedef double Number;

struct Value;
typedef struct Value* Array;

typedef struct ArrayData {
	size_t size;
	Array data;
} ArrayData;

typedef struct StructureData {
	const char* name;
	ArrayData fields;
} StructureData;

typedef union ValueStorage {
	Number number;
	ArrayData array;
	StructureData structure;
} ValueStorage;

typedef struct Value {
	ValueType type;
	ValueStorage storage;
} Value;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Global constants and variables.
 ******************************************************************************/
const Value __FALSE = {VALUE_TYPE_NUMBER, {0.0}};
const Value __TRUE = {VALUE_TYPE_NUMBER, {1.0}};
const size_t NUMBER_FORMAT_BUFFER_SIZE = 1024;
Value __TYPE_NAME_NULL;
Value __TYPE_NAME_NUMBER;
Value __TYPE_NAME_ARRAY;

static bool prng_initialized = false;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
void* AllocateMemory(const size_t size);
size_t GetStructureFieldsNumber(const char* name);
size_t GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
);
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils.
 ******************************************************************************/
Value CreateArrayFromString(const char* string);
void InitializeConstants(void) {
	__TYPE_NAME_NULL = CreateArrayFromString("null");
	__TYPE_NAME_NUMBER = CreateArrayFromString("number");
	__TYPE_NAME_ARRAY = CreateArrayFromString("array");
}

bool HasAllowedType(const Value value, const size_t allowed_types) {
	return allowed_types & value.type;
}

void TestTypeAndNotify(const Value value, const size_t allowed_types) {
	if (!HasAllowedType(value, allowed_types)) {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Invalid type.");
	}
}

bool IsValidIndex(const Value array, const Value index) {
	return
		index.storage.number >= 0.0
		&& index.storage.number < array.storage.array.size;
}

void TestIndexAndNotify(const Value array, const Value index) {
	if (!IsValidIndex(array, index)) {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Out of range.");
	}
}

bool ToBoolean(const Value value) {
	return
		value.type != VALUE_TYPE_NULL
		&& (value.type != VALUE_TYPE_NUMBER
		|| value.storage.number != 0.0);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation.
 ******************************************************************************/
Value CreateNull(void) {
	Value value;
	value.type = VALUE_TYPE_NULL;

	return value;
}

Value CreateNumber(const Number number) {
	Value value;
	value.type = VALUE_TYPE_NUMBER;
	value.storage.number = number;

	return value;
}

ArrayData CreateArrayData(const size_t size) {
	ArrayData array_data;
	array_data.size = size;
	array_data.data = (Array)AllocateMemory(size * sizeof(Value));

	for (size_t i = 0; i < size; i++) {
		array_data.data[i] = CreateNull();
	}

	return array_data;
}

Value CreateArray(const size_t size) {
	Value value;
	value.type = VALUE_TYPE_ARRAY;
	value.storage.array = CreateArrayData(size);

	return value;
}

Value CreateArrayFromList(const size_t size, ...) {
	Value array = CreateArray(size);

	va_list arguments;
	va_start(arguments, size);
	for (size_t i = 0; i < size; i++) {
		array.storage.array.data[i] = va_arg(arguments, Value);
	}
	va_end(arguments);

	return array;
}

Value CreateArrayFromString(const char* string) {
	const size_t length = strlen(string);
	Value array = CreateArray(length);
	for (size_t i = 0; i < length; i++) {
		array.storage.array.data[i] = CreateNumber(string[i]);
	}

	return array;
}

Value CreateStructure(const char* name) {
	Value value;
	value.type = VALUE_TYPE_STRUCTURE;
	value.storage.structure.name = name;

	const size_t fields_number = GetStructureFieldsNumber(name);
	value.storage.structure.fields = CreateArrayData(fields_number);

	return value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Number operations.
 ******************************************************************************/
Value UnaryMinus(const Value value) {
	TestTypeAndNotify(value, VALUE_TYPE_NUMBER);

	const double number = -value.storage.number;
	return CreateNumber(number);
}

Value Add(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number + value_2.storage.number;
	return CreateNumber(number);
}

Value Subtract(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number - value_2.storage.number;
	return CreateNumber(number);
}

Value Multiply(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number * value_2.storage.number;
	return CreateNumber(number);
}

Value Divide(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number / value_2.storage.number;
	return CreateNumber(number);
}

Value Modulo(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number =
		(long)round(value_1.storage.number)
		% (long)round(value_2.storage.number);
	return CreateNumber(number);
}

Value Less(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number < value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value LessOrEqual(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number <= value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value Greater(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number > value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value GreaterOrEqual(const Value value_1, const Value value_2) {
	TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number >= value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value ToString(const Value value) {
	TestTypeAndNotify(value, VALUE_TYPE_NUMBER);

	char buffer[NUMBER_FORMAT_BUFFER_SIZE];
	sprintf(buffer, "%f", value.storage.number);

	return CreateArrayFromString(buffer);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Array operations.
 ******************************************************************************/
Value GetArrayItem(const Value array, const Value index) {
	TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	TestTypeAndNotify(index, VALUE_TYPE_NUMBER);
	TestIndexAndNotify(array, index);

	const size_t integral_index = (size_t)floor(abs(index.storage.number));
	return array.storage.array.data[integral_index];
}

void SetArrayItem(const Value array, const Value index, const Value value) {
	TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	TestTypeAndNotify(index, VALUE_TYPE_NUMBER);
	TestIndexAndNotify(array, index);

	const size_t integral_index = (size_t)floor(abs(index.storage.number));
	array.storage.array.data[integral_index] = value;
}

Value GetLength(const Value array) {
	TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	return CreateNumber(array.storage.array.size);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations.
 ******************************************************************************/
Value GetStructureField(const Value structure, const char* field_name) {
	TestTypeAndNotify(structure, VALUE_TYPE_STRUCTURE);

	const size_t field_index = GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	return structure.storage.structure.fields.data[field_index];
}

void SetStructureField(
	const Value structure,
	const char* field_name,
	const Value value
) {
	TestTypeAndNotify(structure, VALUE_TYPE_STRUCTURE);

	const size_t field_index = GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	structure.storage.structure.fields.data[field_index] = value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations.
 ******************************************************************************/
Value Not(const Value value) {
	const bool result = !ToBoolean(value);
	return result ? __TRUE : __FALSE;
}

Value Equal(const Value value_1, const Value value_2) {
	if (value_1.type != value_2.type) {
		return __FALSE;
	}

	bool result = false;
	switch (value_1.type) {
		case VALUE_TYPE_NULL:
			result = true;
			break;
		case VALUE_TYPE_NUMBER:
			result = value_1.storage.number == value_2.storage.number;
			break;
		case VALUE_TYPE_ARRAY:
			result = value_1.storage.array.data == value_2.storage.array.data;
			break;
		case VALUE_TYPE_STRUCTURE:
			result =
				value_1.storage.structure.fields.data
				== value_2.storage.structure.fields.data;
			break;
	}

	return result ? __TRUE : __FALSE;
}

Value NotEqual(const Value value_1, const Value value_2) {
	if (value_1.type != value_2.type) {
		return __TRUE;
	}

	bool result = true;
	switch (value_1.type) {
		case VALUE_TYPE_NULL:
			result = false;
			break;
		case VALUE_TYPE_NUMBER:
			result = value_1.storage.number != value_2.storage.number;
			break;
		case VALUE_TYPE_ARRAY:
			result = value_1.storage.array.data != value_2.storage.array.data;
			break;
		case VALUE_TYPE_STRUCTURE:
			result =
				value_1.storage.structure.fields.data
				!= value_2.storage.structure.fields.data;
			break;
	}

	return result ? __TRUE : __FALSE;
}

Value And(const Value value_1, const Value value_2) {
	const bool result = ToBoolean(value_1) && ToBoolean(value_2);
	return result ? __TRUE : __FALSE;
}

Value Or(const Value value_1, const Value value_2) {
	const bool result = ToBoolean(value_1) || ToBoolean(value_2);
	return result ? __TRUE : __FALSE;
}

Value GetType(const Value value) {
	Value type_name;
	switch (value.type) {
		case VALUE_TYPE_NULL:
			type_name = __TYPE_NAME_NULL;
			break;
		case VALUE_TYPE_NUMBER:
			type_name = __TYPE_NAME_NUMBER;
			break;
		case VALUE_TYPE_ARRAY:
			type_name = __TYPE_NAME_ARRAY;
			break;
		case VALUE_TYPE_STRUCTURE:
			type_name = CreateArrayFromString(value.storage.structure.name);
			break;
	}

	return type_name;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * System module.
 ******************************************************************************/
void Exit(const Value exit_code) {
	TestTypeAndNotify(exit_code, VALUE_TYPE_NUMBER);

	const long unwrapped_exit_code = (long)round(exit_code.storage.number);
	exit(unwrapped_exit_code);
}

Value GetTime(void) {
	const time_t current_time = time(NULL);
	return CreateNumber(current_time);
}

void Show(const Value array) {
	TestTypeAndNotify(array, VALUE_TYPE_ARRAY);

	for (size_t i = 0; i < array.storage.array.size; i++) {
		TestTypeAndNotify(array.storage.array.data[i], VALUE_TYPE_NUMBER);

		putchar(array.storage.array.data[i].storage.number);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Maths module.
 ******************************************************************************/
Value Sin(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = sin(number.storage.number);
	return CreateNumber(result);
}

Value Cos(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = cos(number.storage.number);
	return CreateNumber(result);
}

Value Tg(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = tan(number.storage.number);
	return CreateNumber(result);
}

Value Arcsin(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = asin(number.storage.number);
	return CreateNumber(result);
}

Value Arccos(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = acos(number.storage.number);
	return CreateNumber(result);
}

Value Arctg(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = atan(number.storage.number);
	return CreateNumber(result);
}

Value SquareRoot(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = sqrt(number.storage.number);
	return CreateNumber(result);
}

Value Power(const Value base, const Value exponent) {
	TestTypeAndNotify(base, VALUE_TYPE_NUMBER);
	TestTypeAndNotify(exponent, VALUE_TYPE_NUMBER);

	const double result = pow(base.storage.number, exponent.storage.number);
	return CreateNumber(result);
}

Value Exp(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = exp(number.storage.number);
	return CreateNumber(result);
}

Value Ln(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = log(number.storage.number);
	return CreateNumber(result);
}

Value Integral(const Value number) {
	TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	double result = 0.0;
	modf(number.storage.number, &result);
	return CreateNumber(result);
}

Value GetRandom(void) {
	if (!prng_initialized) {
		srand(time(NULL));
		prng_initialized = true;
	}

	const double result = (double)rand() / RAND_MAX;
	return CreateNumber(result);
}
//------------------------------------------------------------------------------
#endif
