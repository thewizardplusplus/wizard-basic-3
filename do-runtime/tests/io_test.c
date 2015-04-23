#include "../source/runtime.h"
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

void WriteTestFile(const __Value filename, const __Value test_string) {
	__Value file = Open(filename, __CreateNumber(1));
	if (__ToBoolean(__Not(file))) {
		fputs("Error: unable to create test file.\n", stderr);
	}

	__Value result = Write(file, test_string);
	if (__ToBoolean(__Not(result))) {
		fputs("Error: unable to write to test file.\n", stderr);
	}

	Close(file);
}

__Value ReadTestFile(const __Value filename, const __Value test_string_length) {
	__Value file = Open(filename, __CreateNumber(0));
	if (__ToBoolean(__Not(file))) {
		fputs("Error: unable to open test file.\n", stderr);
	}

	__Value data = Read(file, test_string_length);
	if (__ToBoolean(__NotEqual(GetSize(data), test_string_length))) {
		fputs("Error: unable to read to test file.\n", stderr);
	}

	Close(file);

	return data;
}

int main(void) {
	__InitializeConstants();
	__InitializeOpenedFileStorage();

	__Value test_filename = __CreateArrayFromString("io_test_result.txt");
	__Value test_string = __CreateArrayFromString("OK\n");
	WriteTestFile(test_filename, test_string);
	__Value data = ReadTestFile(test_filename, GetSize(test_string));
	if (__ToBoolean(__NotEqual(GetSize(data), __CreateNumber(0)))) {
		Write(__CreateNumber(1), data);
	}

	__CleanupOpenedFileStorage();
}
