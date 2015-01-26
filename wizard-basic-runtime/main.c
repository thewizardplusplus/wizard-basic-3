#include <stdio.h>

typedef enum ValueType {
	NULL,
	NUMBER,
	ARRAY,
	STRUCTURE
} ValueType;

typedef Number double;

struct Value;
typedef Array *struct Value;

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

int main(void) {
	puts("Test.");
}
