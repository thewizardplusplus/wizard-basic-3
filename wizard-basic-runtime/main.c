#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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
} Value, *ValuePointer;

ValuePointer CreateValue(void) {
	return (ValuePointer)malloc(sizeof(Value));
}

size_t GetStructureFieldsNumber(const char* name) {
	(void)name;
	return 1;
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
	array_data.size = size;
	array_data.data = (Array)malloc(sizeof(Value) * size);

	return array_data;
}

ValuePointer CreateArray(size_t size) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_ARRAY;
	value->storage.array = CreateArrayData(size);

	return value;
}

ValuePointer CreateStructure(const char* name) {
	ValuePointer value = CreateValue();
	value->type = VALUE_TYPE_STRUCTURE;
	value->storage.structure.name = name;

	size_t fields_number = GetStructureFieldsNumber(name);
	value->storage.structure.fields = CreateArrayData(fields_number);

	return value;
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

int main(void) {
	puts("Test.");
}
