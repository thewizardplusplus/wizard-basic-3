#ifndef WIZARD_BASIC_3_RUNTIME_HEADER
#define WIZARD_BASIC_3_RUNTIME_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

/*******************************************************************************
 * Messages.
 ******************************************************************************/
typedef enum MessageType {
	MESSAGE_TYPE_INFO,
	MESSAGE_TYPE_WARNING,
	MESSAGE_TYPE_ERROR
} MessageType;

void ProcessMessage(MessageType type, const char* message) {
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
 * Interpreter API.
 ******************************************************************************/
void* AllocateMemory(size_t size);
size_t GetStructureFieldsNumber(const char* name);
size_t GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
);
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation.
 ******************************************************************************/
Value CreateNull(void) {
	Value value;
	value.type = VALUE_TYPE_NULL;

	return value;
}

Value CreateNumber(Number number) {
	Value value;
	value.type = VALUE_TYPE_NUMBER;
	value.storage.number = number;

	return value;
}

ArrayData CreateArrayData(size_t size) {
	ArrayData array_data;
	array_data.size = size;
	if (size > 0) {
		array_data.data = (Array)AllocateMemory(size * sizeof(Value));
		for (size_t i = 0; i < size; i++) {
			array_data.data[i] = CreateNull();
		}
	}

	return array_data;
}

Value CreateArray(size_t size) {
	Value value;
	value.type = VALUE_TYPE_ARRAY;
	value.storage.array = CreateArrayData(size);

	return value;
}

Value CreateArrayFromList(size_t size, ...) {
	Value array = CreateArray(size);

	va_list arguments;
	va_start(arguments, size);
	for (size_t i = 0; i < size; i++) {
		Value argument = va_arg(arguments, Value);
		array.storage.array.data[i] = argument;
	}
	va_end(arguments);

	return array;
}

Value CreateArrayFromString(const char* string) {
	size_t length = strlen(string);
	Value array = CreateArray(length);
	for (size_t i = 0; i < length; i++) {
		Value symbol_code = CreateNumber(string[i]);
		array.storage.array.data[i] = symbol_code;
	}

	return array;
}

Value CreateStructure(const char* name) {
	Value value;
	value.type = VALUE_TYPE_STRUCTURE;
	value.storage.structure.name = name;

	size_t fields_number = GetStructureFieldsNumber(name);
	value.storage.structure.fields = CreateArrayData(fields_number);

	return value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Number operations.
 ******************************************************************************/
void TestType(Value value, size_t allowed_types) {
	bool valid = false;
	switch (value.type) {
		case VALUE_TYPE_NULL:
			valid = allowed_types & VALUE_TYPE_NULL;
			break;
		case VALUE_TYPE_NUMBER:
			valid = allowed_types & VALUE_TYPE_NUMBER;
			break;
		case VALUE_TYPE_ARRAY:
			valid = allowed_types & VALUE_TYPE_ARRAY;
			break;
		case VALUE_TYPE_STRUCTURE:
			valid = allowed_types & VALUE_TYPE_STRUCTURE;
			break;
	}

	if (!valid) {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Invalid type.");
	}
}

Value UnaryMinus(Value value) {
	TestType(value, VALUE_TYPE_NUMBER);

	double number = -value.storage.number;
	return CreateNumber(number);
}

Value Add(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1.storage.number + value_2.storage.number;
	return CreateNumber(number);
}

Value Subtract(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1.storage.number - value_2.storage.number;
	return CreateNumber(number);
}

Value Multiply(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1.storage.number * value_2.storage.number;
	return CreateNumber(number);
}

Value Divide(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1.storage.number / value_2.storage.number;
	return CreateNumber(number);
}

Value Modulo(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number =
		(long)round(value_1.storage.number)
		% (long)round(value_2.storage.number);
	return CreateNumber(number);
}

Value Less(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1.storage.number < value_2.storage.number;
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value LessOrEqual(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1.storage.number <= value_2.storage.number;
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value Greater(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1.storage.number > value_2.storage.number;
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value GreaterOrEqual(Value value_1, Value value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1.storage.number >= value_2.storage.number;
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Array operations.
 ******************************************************************************/
void TestIndex(Value array, Value index) {
	if (
		index.storage.number < 0.0
		|| index.storage.number >= array.storage.array.size
	) {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Out of range.");
	}
}

Value GetArrayItem(Value array, Value index) {
	TestType(array, VALUE_TYPE_ARRAY);
	TestType(index, VALUE_TYPE_NUMBER);
	TestIndex(array, index);

	size_t integral_index = (size_t)floor(abs(index.storage.number));
	return array.storage.array.data[integral_index];
}

void SetArrayItem(Value array, Value index, Value value) {
	TestType(array, VALUE_TYPE_ARRAY);
	TestType(index, VALUE_TYPE_NUMBER);
	TestIndex(array, index);

	size_t integral_index = (size_t)floor(abs(index.storage.number));
	array.storage.array.data[integral_index] = value;
}

Value GetArrayLength(Value array) {
	TestType(array, VALUE_TYPE_ARRAY);
	return CreateNumber(array.storage.array.size);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations.
 ******************************************************************************/
Value GetStructureField(Value structure, const char* field_name) {
	TestType(structure, VALUE_TYPE_STRUCTURE);

	size_t field_index = GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	return structure.storage.structure.fields.data[field_index];
}

void SetStructureField(
	Value structure,
	const char* field_name,
	Value value
) {
	TestType(structure, VALUE_TYPE_STRUCTURE);

	size_t field_index = GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	structure.storage.structure.fields.data[field_index] = value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations.
 ******************************************************************************/
bool ToBoolean(Value value) {
	return
		value.type != VALUE_TYPE_NULL
		&& (value.type != VALUE_TYPE_NUMBER
		|| value.storage.number != 0.0);
}

Value Not(Value value) {
	bool result = !ToBoolean(value);
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value Equal(Value value_1, Value value_2) {
	if (value_1.type != value_2.type) {
		return CreateNumber(0.0);
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

	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value NotEqual(Value value_1, Value value_2) {
	if (value_1.type != value_2.type) {
		return CreateNumber(1.0);
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

	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value And(Value value_1, Value value_2) {
	bool result = ToBoolean(value_1) && ToBoolean(value_2);
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value Or(Value value_1, Value value_2) {
	bool result = ToBoolean(value_1) || ToBoolean(value_2);
	return result ? CreateNumber(1.0) : CreateNumber(0.0);
}

Value GetValueType(Value value) {
	Value type_name;
	switch (value.type) {
		case VALUE_TYPE_NULL:
			type_name = CreateArrayFromString("null");
			break;
		case VALUE_TYPE_NUMBER:
			type_name = CreateArrayFromString("number");
			break;
		case VALUE_TYPE_ARRAY:
			type_name = CreateArrayFromString("array");
			break;
		case VALUE_TYPE_STRUCTURE:
			type_name = CreateArrayFromString(value.storage.structure.name);
			break;
	}

	return type_name;
}
//------------------------------------------------------------------------------
#endif
