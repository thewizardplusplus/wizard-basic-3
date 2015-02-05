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

size_t __capacity_of_opened_files_storage = 0;
size_t __number_of_opened_files = 0;
FILE** __opened_files = NULL;
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
void __ProcessError(const char* description) {
	fprintf(stderr, "Error: %s.\n", description);
	exit(EXIT_FAILURE);
}

bool __HasAllowedType(const Value value, const size_t allowed_types) {
	return allowed_types & value.type;
}

void __TestTypeAndNotify(const Value value, const size_t allowed_types) {
	if (!__HasAllowedType(value, allowed_types)) {
		__ProcessError("invalid type");
	}
}

bool __IsValidIndex(const Value array, const Value index) {
	return
		index.storage.number >= 0.0
		&& index.storage.number < array.storage.array.size;
}

void __TestIndexAndNotify(const Value array, const Value index) {
	if (!__IsValidIndex(array, index)) {
		__ProcessError("out of range");
	}
}

bool __ToBoolean(const Value value) {
	return
		value.type != VALUE_TYPE_NULL
		&& (value.type != VALUE_TYPE_NUMBER
		|| value.storage.number != 0.0);
}

Value __FromBoolean(bool value) {
	return value ? __TRUE : __FALSE;
}

long __Round(const double number) {
	return (long)round(number);
}

size_t __ToIntegral(const double number) {
	return (size_t)floor(abs(number));
}

char* __ToString(const Value array) {
	const size_t size = __ToIntegral(array.storage.array.size);
	char* buffer = (char*)malloc(size + 1);
	for (size_t i = 0; i < size; i++) {
		const Value symbol = array.storage.array.data[i];
		__TestTypeAndNotify(symbol, VALUE_TYPE_ARRAY);

		buffer[i] = symbol.storage.number;
	}
	buffer[size] = '\0';

	return buffer;
}

bool __IsValidFileId(const size_t file_id) {
	return
		file_id >= __number_of_opened_files
		|| __opened_files[file_id] == NULL;
}

void __TestFileIdAndNotify(const size_t file_id) {
	if (!__IsValidFileId(file_id)) {
		__ProcessError("invalid file ID");
	}
}

size_t __AddOpenedFile(FILE* file) {
	const size_t OPENED_FILE_STORAGE_RESIZE_FACTOR = 2;
	const size_t OPENED_FILE_STORAGE_DEFAULT_CAPACITY = 12;

	if (__number_of_opened_files == __capacity_of_opened_files_storage) {
		if (__capacity_of_opened_files_storage != 0) {
			__capacity_of_opened_files_storage *=
				OPENED_FILE_STORAGE_RESIZE_FACTOR;
		} else {
			__capacity_of_opened_files_storage =
				OPENED_FILE_STORAGE_DEFAULT_CAPACITY;
		}

		__opened_files = (FILE**)realloc(
			__opened_files,
			__capacity_of_opened_files_storage
		);
	}

	__opened_files[__number_of_opened_files++] = file;

	return __number_of_opened_files;
}

Value __CreateArrayFromString(const char* string);
void __Initialize(void) {
	__TYPE_NAME_NULL = __CreateArrayFromString("null");
	__TYPE_NAME_NUMBER = __CreateArrayFromString("number");
	__TYPE_NAME_ARRAY = __CreateArrayFromString("array");

	__AddOpenedFile(stdin);
	__AddOpenedFile(stdout);
	__AddOpenedFile(stderr);
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
		__Round(value_1.storage.number)
		% __Round(value_2.storage.number);
	return __CreateNumber(number);
}

Value __Less(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number < value_2.storage.number;
	return __FromBoolean(result);
}

Value __LessOrEqual(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number <= value_2.storage.number;
	return __FromBoolean(result);
}

Value __Greater(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number > value_2.storage.number;
	return __FromBoolean(result);
}

Value __GreaterOrEqual(const Value value_1, const Value value_2) {
	__TestTypeAndNotify(value_1, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.number >= value_2.storage.number;
	return __FromBoolean(result);
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

	const size_t integral_index = __ToIntegral(index.storage.number);
	return array.storage.array.data[integral_index];
}

void __SetArrayItem(const Value array, const Value index, const Value value) {
	__TestTypeAndNotify(array, VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(index, VALUE_TYPE_NUMBER);
	__TestIndexAndNotify(array, index);

	const size_t integral_index = __ToIntegral(index.storage.number);
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
	return __FromBoolean(result);
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

	return __FromBoolean(result);
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

	return __FromBoolean(result);
}

Value __And(const Value value_1, const Value value_2) {
	const bool result = __ToBoolean(value_1) && __ToBoolean(value_2);
	return __FromBoolean(result);
}

Value __Or(const Value value_1, const Value value_2) {
	const bool result = __ToBoolean(value_1) || __ToBoolean(value_2);
	return __FromBoolean(result);
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
Value GetTime(void) {
	const time_t current_time = time(NULL);
	return __CreateNumber(current_time);
}

Value Exit(const Value exit_code) {
	__TestTypeAndNotify(exit_code, VALUE_TYPE_NUMBER);

	const size_t unwrapped_exit_code = __ToIntegral(exit_code.storage.number);
	exit(unwrapped_exit_code);

	return __NULL;
}

Value Read(const Value stream, const Value number) {
	__TestTypeAndNotify(stream, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(number, VALUE_TYPE_NUMBER);

	const size_t file_id = __ToIntegral(stream.storage.number);
	__TestFileIdAndNotify(file_id);

	FILE* file = __opened_files[file_id];

	const size_t number_of_bytes = __ToIntegral(number.storage.array.size);
	char* buffer = (char*)malloc(number_of_bytes + 1);
	size_t i = 0;
	for (; i < number_of_bytes; i++) {
		const int byte = fgetc(file);
		if (byte == EOF) {
			break;
		}

		buffer[i] = byte;
	}
	buffer[i] = '\0';

	const Value result = __CreateArrayFromString(buffer);
	free(buffer);

	return result;
}

Value Write(const Value stream, const Value bytes) {
	__TestTypeAndNotify(stream, VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(bytes, VALUE_TYPE_ARRAY);

	const size_t file_id = __ToIntegral(stream.storage.number);
	__TestFileIdAndNotify(file_id);

	FILE* file = __opened_files[file_id];
	char* buffer = __ToString(bytes);

	int result = fputs(buffer, file);
	free(buffer);
	if (result == EOF) {
		return __FALSE;
	}

	result = fflush(file);
	return __FromBoolean(result != EOF);
}

Value Open(const Value path, const Value mode) {
	const size_t __FILE_OPEN_MODE_READ = 0;
	const size_t __FILE_OPEN_MODE_WRITE = 1;
	const size_t __FILE_OPEN_MODE_APPEND = 2;

	__TestTypeAndNotify(path, VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(mode, VALUE_TYPE_NUMBER);

	char* path_buffer = __ToString(path);

	const size_t unwrapped_mode = __ToIntegral(mode.storage.number);
	const char* mode_string = NULL;
	switch (unwrapped_mode) {
		case __FILE_OPEN_MODE_READ:
			mode_string = "r";
			break;
		case __FILE_OPEN_MODE_WRITE:
			mode_string = "w";
			break;
		case __FILE_OPEN_MODE_APPEND:
			mode_string = "a";
			break;
	}

	FILE* file = fopen(path_buffer, mode_string);
	free(path_buffer);

	const size_t file_id = __AddOpenedFile(file);
	return __CreateNumber(file_id);
}

Value Close(const Value stream) {
	__TestTypeAndNotify(stream, VALUE_TYPE_NUMBER);

	const size_t file_id = __ToIntegral(stream.storage.number);
	__TestFileIdAndNotify(file_id);

	FILE* file = __opened_files[file_id];
	fclose(file);
	__opened_files[file_id] = NULL;

	return __NULL;
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
