#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define VALUE_NUMBER(value) { \
	.type = VALUE_TYPE_NUMBER, \
	.storage = { \
		.number = value \
	} \
}

typedef enum MessageType {
	MESSAGE_TYPE_INFO,
	MESSAGE_TYPE_WARNING,
	MESSAGE_TYPE_ERROR
} MessageType;

typedef enum ValueType {
	VALUE_TYPE_NULL = 1 << 0,
	VALUE_TYPE_NUMBER = 1 << 1,
	VALUE_TYPE_ARRAY = 1 << 2,
	VALUE_TYPE_STRUCTURE = 1 << 3
} ValueType;

typedef double Number;

struct Value;
typedef struct Value** Array;

typedef struct ArrayData {
	struct Value* size;
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
} Value, *ValuePointer;

Value FALSE = VALUE_NUMBER(0.0);
Value TRUE = VALUE_NUMBER(1.0);

void* AllocateMemory(size_t size) {
	return malloc(size);
}

size_t GetStructureFieldsNumber(const char* name) {
	(void)name;
	return 1;
}

void ProcessMessage(MessageType type, const char* message) {
	if (type == MESSAGE_TYPE_WARNING || type == MESSAGE_TYPE_ERROR) {
		const char* prefix = NULL;
		if (type == MESSAGE_TYPE_WARNING) {
			prefix = "Warning! ";
		} else {
			prefix = "Error! ";
		}

		char buffer[strlen(prefix) + strlen(message) + 1];
		strcpy(buffer, prefix);
		strcat(buffer, message);

		fputs(buffer, stderr);
	} else {
		puts(message);
	}
}

void TestType(ValuePointer value, size_t allowed_types) {
	bool valid = false;
	switch (value->type) {
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

ValuePointer CreateValue(void) {
	return (ValuePointer)AllocateMemory(sizeof(Value));
}

ValuePointer CreateNull(void) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_NULL;

	return value;
}

ValuePointer CreateNumber(Number number) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_NUMBER;
	value->storage.number = number;

	return value;
}

ArrayData CreateArrayData(size_t size) {
	ArrayData array_data;
	array_data.size = CreateNumber(size);
	array_data.data = (Array)AllocateMemory(sizeof(Value) * size);

	return array_data;
}

ValuePointer CreateArray(size_t size) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_ARRAY;
	value->storage.array = CreateArrayData(size);

	return value;
}

ValuePointer GetLength(ValuePointer array) {
	TestType(array, VALUE_TYPE_ARRAY);
	return array->storage.array.size;
}

ValuePointer GetArrayItem(ValuePointer array, ValuePointer index) {
	TestType(array, VALUE_TYPE_ARRAY);
	TestType(index, VALUE_TYPE_NUMBER);

	if (
		index->storage.number >= 0.0
		&& index->storage.number < array->storage.array.size->storage.number
	) {
		size_t integral_index = (size_t)floor(abs(index->storage.number));
		return array->storage.array.data[integral_index];
	} else {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Out of range.");
	}
}

void SetArrayItem(ValuePointer array, ValuePointer index, ValuePointer value) {
	TestType(array, VALUE_TYPE_ARRAY);
	TestType(index, VALUE_TYPE_NUMBER);

	if (
		index->storage.number >= 0.0
		&& index->storage.number < array->storage.array.size->storage.number
	) {
		size_t integral_index = (size_t)floor(abs(index->storage.number));
		array->storage.array.data[integral_index] = value;
	} else {
		ProcessMessage(MESSAGE_TYPE_ERROR, "Out of range.");
	}
}

ValuePointer CreateArrayFromData(size_t size, ...) {
	ValuePointer array = CreateArray(size);

	va_list arguments;
	va_start(arguments, size);
	for (size_t i = 0; i < size; i++) {
		ValuePointer argument = va_arg(arguments, ValuePointer);
		SetArrayItem(array, CreateNumber(i), argument);
	}
	va_end(arguments);

	return array;
}

ValuePointer CreateStructure(const char* name) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_STRUCTURE;
	value->storage.structure.name = name;

	size_t fields_number = GetStructureFieldsNumber(name);
	value->storage.structure.fields = CreateArrayData(fields_number);

	return value;
}

ValuePointer Add(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1->storage.number + value_2->storage.number;
	return CreateNumber(number);
}

ValuePointer Subtract(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1->storage.number - value_2->storage.number;
	return CreateNumber(number);
}

ValuePointer Multiply(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1->storage.number * value_2->storage.number;
	return CreateNumber(number);
}

ValuePointer Divide(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number = value_1->storage.number / value_2->storage.number;
	return CreateNumber(number);
}

ValuePointer Modulo(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	double number =
		(long)round(value_1->storage.number)
		% (long)round(value_2->storage.number);
	return CreateNumber(number);
}

ValuePointer Less(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1->storage.number < value_2->storage.number;
	return result ? &TRUE : &FALSE;
}

ValuePointer LessOrEqual(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1->storage.number <= value_2->storage.number;
	return result ? &TRUE : &FALSE;
}

ValuePointer Greater(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1->storage.number > value_2->storage.number;
	return result ? &TRUE : &FALSE;
}

ValuePointer GreaterOrEqual(ValuePointer value_1, ValuePointer value_2) {
	TestType(value_1, VALUE_TYPE_NUMBER);
	TestType(value_2, VALUE_TYPE_NUMBER);

	bool result = value_1->storage.number >= value_2->storage.number;
	return result ? &TRUE : &FALSE;
}

int main(void) {
	puts("Test.");
}
