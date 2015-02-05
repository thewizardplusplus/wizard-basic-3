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

void __ProcessMessage(const MessageType type, const char* message) {
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
 * Constants.
 ******************************************************************************/
const Value __NULL = {VALUE_TYPE_NULL, {0.0}};
const Value __FALSE = {VALUE_TYPE_NUMBER, {0.0}};
const Value __TRUE = {VALUE_TYPE_NUMBER, {1.0}};
Value __TYPE_NAME_NULL;
Value __TYPE_NAME_NUMBER;
Value __TYPE_NAME_ARRAY;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
void* __AllocateMemory(const size_t size);
size_t __GetStructureFieldsNumber(const char* name);
size_t __GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
);
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils.
 ******************************************************************************/
Value __CreateArrayFromString(const char* string);
void __InitializeConstants(void) {
	__TYPE_NAME_NULL = __CreateArrayFromString("null");
	__TYPE_NAME_NUMBER = __CreateArrayFromString("number");
	__TYPE_NAME_ARRAY = __CreateArrayFromString("array");
}

bool __HasAllowedType(const Value value, const size_t allowed_types) {
	return allowed_types & value.type;
}

void __TestTypeAndNotify(const Value value, const size_t allowed_types) {
	if (!__HasAllowedType(value, allowed_types)) {
		__ProcessMessage(MESSAGE_TYPE_ERROR, "Invalid type.");
	}
}

bool __IsValidIndex(const Value array, const Value index) {
	return
		index.storage.number >= 0.0
		&& index.storage.number < array.storage.array.size;
}

void __TestIndexAndNotify(const Value array, const Value index) {
	if (!__IsValidIndex(array, index)) {
		__ProcessMessage(MESSAGE_TYPE_ERROR, "Out of range.");
	}
}

bool __ToBoolean(const Value value) {
	return
		value.type != VALUE_TYPE_NULL
		&& (value.type != VALUE_TYPE_NUMBER
		|| value.storage.number != 0.0);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation.
 ******************************************************************************/
Value __CreateNumber(const Number number) {
	Value value;
	value.type = VALUE_TYPE_NUMBER;
	value.storage.number = number;

	return value;
}

ArrayData __CreateArrayData(const size_t size) {
	ArrayData array_data;
	array_data.size = size;
	array_data.data = (Array)__AllocateMemory(size * sizeof(Value));

	for (size_t i = 0; i < size; i++) {
		array_data.data[i] = __NULL;
	}

	return array_data;
}

Value __CreateArray(const size_t size) {
	Value value;
	value.type = VALUE_TYPE_ARRAY;
	value.storage.array = __CreateArrayData(size);

	return value;
}

Value __CreateArrayFromList(const size_t size, ...) {
	Value array = __CreateArray(size);

	va_list arguments;
	va_start(arguments, size);
	for (size_t i = 0; i < size; i++) {
		array.storage.array.data[i] = va_arg(arguments, Value);
	}
	va_end(arguments);

	return array;
}

Value __CreateArrayFromString(const char* string) {
	const size_t length = strlen(string);
	Value array = __CreateArray(length);
	for (size_t i = 0; i < length; i++) {
		array.storage.array.data[i] = __CreateNumber(string[i]);
	}

	return array;
}

Value __CreateStructure(const char* name) {
	Value value;
	value.type = VALUE_TYPE_STRUCTURE;
	value.storage.structure.name = name;

	const size_t fields_number = __GetStructureFieldsNumber(name);
	value.storage.structure.fields = __CreateArrayData(fields_number);

	return value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Number operations.
 ******************************************************************************/
Value __UnaryMinus(const Value value) {
	__TestTypeAndNotify(value, VALUE_TYPE_NUMBER);

	const double number = -value.storage.number;
	return __CreateNumber(number);
}

Value __Add(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number + value_2.storage.number;
	return __CreateNumber(number);
}

Value __Subtract(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number - value_2.storage.number;
	return __CreateNumber(number);
}

Value __Multiply(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number * value_2.storage.number;
	return __CreateNumber(number);
}

Value __Divide(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number = value_1.storage.number / value_2.storage.number;
	return __CreateNumber(number);
}

Value __Modulo(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const double number =
		(long)round(value_1.storage.number)
		% (long)round(value_2.storage.number);
	return __CreateNumber(number);
}

Value __Less(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number < value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value __LessOrEqual(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number <= value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value __Greater(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number > value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value __GreaterOrEqual(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number >= value_2.storage.number;
	return result ? __TRUE : __FALSE;
}

Value ToString(const Value value) {
	const size_t NUMBER_FORMAT_BUFFER_SIZE = 1024;

	__TestTypeAndNotify(value, VALUE_TYPE_NUMBER);

	char buffer[NUMBER_FORMAT_BUFFER_SIZE];
	sprintf(buffer, "%f", value.storage.number);

	return __CreateArrayFromString(buffer);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Array operations.
 ******************************************************************************/
Value __GetArrayItem(const Value array, const Value index) {
	__TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(index, VALUE_TYPE_NUMBER);
	__TestIndexAndNotify(array, index);

	const size_t integral_index = (size_t)floor(abs(index.storage.number));
	return array.storage.array.data[integral_index];
}

void __SetArrayItem(const Value array, const Value index, const Value value) {
	__TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(index, VALUE_TYPE_NUMBER);
	__TestIndexAndNotify(array, index);

	const size_t integral_index = (size_t)floor(abs(index.storage.number));
	array.storage.array.data[integral_index] = value;
}

Value GetLength(const Value array) {
	__TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	return __CreateNumber(array.storage.array.size);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations.
 ******************************************************************************/
Value __GetStructureField(const Value structure, const char* field_name) {
	__TestTypeAndNotify(structure, VALUE_TYPE_STRUCTURE);

	const size_t field_index = __GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	return structure.storage.structure.fields.data[field_index];
}

void __SetStructureField(
	const Value structure,
	const char* field_name,
	const Value value
) {
	__TestTypeAndNotify(structure, VALUE_TYPE_STRUCTURE);

	const size_t field_index = __GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	structure.storage.structure.fields.data[field_index] = value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations.
 ******************************************************************************/
Value __Not(const Value value) {
	const bool result = !__ToBoolean(value);
	return result ? __TRUE : __FALSE;
}

Value __Equal(const Value value_1, const Value value_2) {
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

Value __NotEqual(const Value value_1, const Value value_2) {
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

Value __And(const Value value_1, const Value value_2) {
	const bool result = __ToBoolean(value_1) && __ToBoolean(value_2);
	return result ? __TRUE : __FALSE;
}

Value __Or(const Value value_1, const Value value_2) {
	const bool result = __ToBoolean(value_1) || __ToBoolean(value_2);
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
			type_name = __CreateArrayFromString(value.storage.structure.name);
			break;
	}

	return type_name;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * System module.
 ******************************************************************************/
void Exit(const Value exit_code) {
	__TestTypeAndNotify(exit_code, VALUE_TYPE_NUMBER);

	const long unwrapped_exit_code = (long)round(exit_code.storage.number);
	exit(unwrapped_exit_code);
}

Value GetTime(void) {
	const time_t current_time = time(NULL);
	return __CreateNumber(current_time);
}

void Show(const Value array) {
	__TestTypeAndNotify(array, VALUE_TYPE_ARRAY);

	for (size_t i = 0; i < array.storage.array.size; i++) {
		__TestTypeAndNotify(array.storage.array.data[i], VALUE_TYPE_NUMBER);

		putchar(array.storage.array.data[i].storage.number);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Maths module.
 ******************************************************************************/
Value Sin(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = sin(number.storage.number);
	return __CreateNumber(result);
}

Value Cos(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = cos(number.storage.number);
	return __CreateNumber(result);
}

Value Tg(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = tan(number.storage.number);
	return __CreateNumber(result);
}

Value Arcsin(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = asin(number.storage.number);
	return __CreateNumber(result);
}

Value Arccos(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = acos(number.storage.number);
	return __CreateNumber(result);
}

Value Arctg(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = atan(number.storage.number);
	return __CreateNumber(result);
}

Value SquareRoot(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = sqrt(number.storage.number);
	return __CreateNumber(result);
}

Value Power(const Value base, const Value exponent) {
	__TestTypeAndNotify(base, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(exponent, VALUE_TYPE_NUMBER);

	const double result = pow(base.storage.number, exponent.storage.number);
	return __CreateNumber(result);
}

Value Exp(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = exp(number.storage.number);
	return __CreateNumber(result);
}

Value Ln(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const double result = log(number.storage.number);
	return __CreateNumber(result);
}

Value Integral(const Value number) {
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	double result = 0.0;
	modf(number.storage.number, &result);
	return __CreateNumber(result);
}

Value GetRandom(void) {
	static bool prng_initialized = false;

	if (!prng_initialized) {
		srand(time(NULL));
		prng_initialized = true;
	}

	const double result = (double)rand() / RAND_MAX;
	return __CreateNumber(result);
}
//------------------------------------------------------------------------------
#endif
