#include <stdlib.h>
#include <stdio.h>

typedef enum ValueType {
	VALUE_TYPE_NULL,
	VALUE_TYPE_NUMBER,
	VALUE_TYPE_ARRAY,
	VALUE_TYPE_STRUCTURE
} ValueType;

typedef double Number;

struct Value;
typedef struct Value* Array;

typedef struct ArrayData {
	size_t size;
	Array array;
} ArrayData;

typedef struct StructureData {
	const char* name;
	ArrayData structure;
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

size_t GetStructureSize(const char* name) {
	(void)name;
	return 0;
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
	array_data.array = (Array)malloc(sizeof(Value) * size);

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

	size_t size = GetStructureSize(name);
	value->storage.structure.structure = CreateArrayData(size);

	return value;
}

int main(void) {
	puts("Test.");
}
