#include "runtime.h"
#include <stdio.h>

/*******************************************************************************
 * Interpreter API mocks.
 ******************************************************************************/
void* __AllocateMemory(const size_t size) {
	return malloc(size);
}

size_t __GetStructureFieldsNumber(const char* name) {
	(void)name;
	return 1;
}

size_t __GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
) {
	(void)structure_name;
	(void)field_name;
	return 0;
}
//------------------------------------------------------------------------------

void WriteTestFile(const Value filename) {
	Value file = Open(filename, __CreateNumber(1));
	if (__ToBoolean(__Not(file))) {
		fputs("Error: unable to create file \"io_test.txt\".\n", stderr);
	}

	Value result = Write(file, __CreateArrayFromString("test\n"));
	if (__ToBoolean(__Not(result))) {
		fputs("Error: unable to write to file \"io_test.txt\".\n", stderr);
	}

	Close(file);
}

Value ReadTestFile(const Value filename) {
	Value file = Open(filename, __CreateNumber(0));
	if (__ToBoolean(__Not(file))) {
		fputs("Error: unable to open file \"io_test.txt\".\n", stderr);
	}

	Value data_length = __CreateNumber(4);
	Value data = Read(file, data_length);
	if (__ToBoolean(__NotEqual(GetLength(data), data_length))) {
		fputs("Error: unable to read to file \"io_test.txt\".\n", stderr);
	}

	Close(file);

	return data;
}

int main(void) {
	__InitializeConstants();
	__InitializeOpenedFileStorage();

	Value test_filename = __CreateArrayFromString("io_test.txt");
	WriteTestFile(test_filename);
	Value data = ReadTestFile(test_filename);
	if (__ToBoolean(__NotEqual(GetLength(data), __CreateNumber(0)))) {
		Write(__CreateNumber(1), data);
		Write(__CreateNumber(1), __CreateArrayFromString("\n"));
	}

	__CleanupOpenedFileStorage();
}
