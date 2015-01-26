#include <stdio.h>

typedef enum ValueType {
	NULL_VALUE,
	NUMBER,
	ARRAY,
	STRUCTURE
} ValueType;

typedef double Number;

struct Value;
typedef struct Value* Array;

typedef struct ArrayData {
	size_t size;
	Array array;
} ArrayData;

typedef struct StructureData {
	char* name;
	void* structure;
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

ValuePointer CreateNull(void) {
	ValuePointer value = CreateValue();
	value->type = NULL_VALUE;

	return value;
}

ValuePointer CreateNumber(Number number) {
	ValuePointer value = CreateValue();
	value->type = NUMBER;
	value->storage.number = number;

	return value;
}

int main(void) {
	puts("Test.");
}
