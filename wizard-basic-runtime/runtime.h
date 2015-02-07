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
typedef enum __ValueType {
	__VALUE_TYPE_NULL = 1 << 0,
	__VALUE_TYPE_NUMBER = 1 << 1,
	__VALUE_TYPE_ARRAY = 1 << 2,
	__VALUE_TYPE_STRUCTURE = 1 << 3
} __ValueType;

typedef double __Number;

struct __Value;
typedef struct __Value* __Array;

typedef struct __ArrayData {
	size_t size;
	__Array data;
} __ArrayData;

typedef struct __StructureData {
	const char* name;
	__ArrayData fields;
} __StructureData;

typedef union __ValueStorage {
	__Number __number;
	__ArrayData __array;
	__StructureData structure;
} __ValueStorage;

typedef struct __Value {
	__ValueType type;
	__ValueStorage storage;
} __Value;

typedef struct __OpenedFileStorage {
	FILE** files;
	size_t __number;
	size_t capacity;
} __OpenedFileStorage;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Constants.
 ******************************************************************************/
const __Value __NULL = {__VALUE_TYPE_NULL, {0.0}};
const __Value __FALSE = {__VALUE_TYPE_NUMBER, {0.0}};
const __Value __TRUE = {__VALUE_TYPE_NUMBER, {1.0}};
static __Value __TYPE_NAME_NULL;
static __Value __TYPE_NAME_NUMBER;
static __Value __TYPE_NAME_ARRAY;

static __OpenedFileStorage __opened_file_storage = {NULL, 0, 0};
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
void* __AllocateMemory(const size_t size);
void __InitializeStructureStorage(void);
void __RegisterStructureField(
	const char* structure_name,
	const char* field_name,
	const size_t field_index
);
size_t __GetStructureFieldsNumber(const char* name);
size_t __GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
);
void __CleanupStructureStorage(void);
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils.
 ******************************************************************************/
__Value __CreateArrayFromString(const char* string);
void __InitializeConstants(void) {
	__TYPE_NAME_NULL = __CreateArrayFromString("null");
	__TYPE_NAME_NUMBER = __CreateArrayFromString("__number");
	__TYPE_NAME_ARRAY = __CreateArrayFromString("__array");
}

size_t __AddOpenedFile(__OpenedFileStorage* storage, FILE* file);
void __InitializeOpenedFileStorage(void) {
	__AddOpenedFile(&__opened_file_storage, stdin);
	__AddOpenedFile(&__opened_file_storage, stdout);
	__AddOpenedFile(&__opened_file_storage, stderr);
}

__Value __CreateArray(const size_t size);
__Value __WrapCommandLineArguments(
	const char* arguments[],
	const size_t number_of_arguments
) {
	__Value result = __CreateArray(number_of_arguments);
	for (size_t i = 0; i < number_of_arguments; i++) {
		result.storage.__array.data[i] = __CreateArrayFromString(arguments[i]);
	}

	return result;
}

void __ProcessError(const char* description) {
	fprintf(stderr, "Error: %s.\n", description);
	exit(EXIT_FAILURE);
}

bool __HasAllowedType(const __Value __value, const size_t allowed_types) {
	return allowed_types & __value.type;
}

void __TestTypeAndNotify(const __Value __value, const size_t allowed_types) {
	if (!__HasAllowedType(__value, allowed_types)) {
		__ProcessError("invalid type");
	}
}

bool __IsValidIndex(const __Value __array, const __Value index) {
	return
		index.storage.__number >= 0.0
		&& index.storage.__number < __array.storage.__array.size;
}

void __TestIndexAndNotify(const __Value __array, const __Value index) {
	if (!__IsValidIndex(__array, index)) {
		__ProcessError("out of range");
	}
}

bool __ToBoolean(const __Value __value) {
	return
		__value.type != __VALUE_TYPE_NULL
		&& (__value.type != __VALUE_TYPE_NUMBER
		|| __value.storage.__number != 0.0);
}

__Value __FromBoolean(bool __value) {
	return __value ? __TRUE : __FALSE;
}

long __Round(const double __number) {
	return (long)round(__number);
}

size_t __GetIntegralModule(const double __number) {
	return (size_t)floor(abs(__number));
}

char* __ToString(const __Value __array) {
	const size_t size = __GetIntegralModule(__array.storage.__array.size);
	char* buffer = (char*)malloc(size + 1);
	for (size_t i = 0; i < size; i++) {
		const __Value symbol = __array.storage.__array.data[i];
		__TestTypeAndNotify(symbol, __VALUE_TYPE_NUMBER);

		buffer[i] = symbol.storage.__number;
	}
	buffer[size] = '\0';

	return buffer;
}

bool __IsValidFileId(const __OpenedFileStorage* storage, const size_t file_id) {
	return file_id < storage->__number && storage->files[file_id] != NULL;
}

void __TestFileIdAndNotify(
	const __OpenedFileStorage* storage,
	const size_t file_id
) {
	if (!__IsValidFileId(storage, file_id)) {
		__ProcessError("invalid file ID");
	}
}

size_t __AddOpenedFile(__OpenedFileStorage* storage, FILE* file) {
	const size_t OPENED_FILE_STORAGE_RESIZE_FACTOR = 2;
	const size_t OPENED_FILE_STORAGE_DEFAULT_CAPACITY = 12;

	if (storage->__number == storage->capacity) {
		if (storage->capacity != 0) {
			storage->capacity *= OPENED_FILE_STORAGE_RESIZE_FACTOR;
		} else {
			storage->capacity = OPENED_FILE_STORAGE_DEFAULT_CAPACITY;
		}

		storage->files = (FILE**)realloc(storage->files, storage->capacity);
	}

	const size_t file_id = storage->__number++;
	storage->files[file_id] = file;

	return file_id;
}

void __CleanupOpenedFileStorage() {
	const size_t NUMBER_OF_UNCLOSED_FILES = 3;

	for (
		size_t i = NUMBER_OF_UNCLOSED_FILES;
		i < __opened_file_storage.__number;
		i++
	) {
		FILE* file = __opened_file_storage.files[i];
		if (file == NULL) {
			continue;
		}

		fclose(file);
		__opened_file_storage.files[i] = NULL;
	}

	//free(__opened_file_storage.files);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation.
 ******************************************************************************/
__Value __CreateNumber(const __Number __number) {
	__Value __value;
	__value.type = __VALUE_TYPE_NUMBER;
	__value.storage.__number = __number;

	return __value;
}

__ArrayData __CreateArrayData(const size_t size) {
	__ArrayData array_data;
	array_data.size = size;
	array_data.data = (__Array)__AllocateMemory(size * sizeof(__Value));

	for (size_t i = 0; i < size; i++) {
		array_data.data[i] = __NULL;
	}

	return array_data;
}

__Value __CreateArray(const size_t size) {
	__Value __value;
	__value.type = __VALUE_TYPE_ARRAY;
	__value.storage.__array = __CreateArrayData(size);

	return __value;
}

__Value __CreateArrayFromList(const size_t size, ...) {
	__Value __array = __CreateArray(size);

	va_list arguments;
	va_start(arguments, size);
	for (size_t i = 0; i < size; i++) {
		__array.storage.__array.data[i] = va_arg(arguments, __Value);
	}
	va_end(arguments);

	return __array;
}

__Value __CreateArrayFromString(const char* string) {
	const size_t length = strlen(string);
	__Value __array = __CreateArray(length);
	for (size_t i = 0; i < length; i++) {
		__array.storage.__array.data[i] = __CreateNumber(string[i]);
	}

	return __array;
}

__Value __CreateStructure(const char* name) {
	__Value __value;
	__value.type = __VALUE_TYPE_STRUCTURE;
	__value.storage.structure.name = name;

	const size_t fields_number = __GetStructureFieldsNumber(name);
	__value.storage.structure.fields = __CreateArrayData(fields_number);

	return __value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * __Number operations.
 ******************************************************************************/
__Value __UnaryMinus(const __Value __value) {
	__TestTypeAndNotify(__value, __VALUE_TYPE_NUMBER);

	const double __number = -__value.storage.__number;
	return __CreateNumber(__number);
}

__Value __Add(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const double __number = value_1.storage.__number + value_2.storage.__number;
	return __CreateNumber(__number);
}

__Value __Subtract(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const double __number = value_1.storage.__number - value_2.storage.__number;
	return __CreateNumber(__number);
}

__Value __Multiply(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const double __number = value_1.storage.__number * value_2.storage.__number;
	return __CreateNumber(__number);
}

__Value __Divide(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const double __number = value_1.storage.__number / value_2.storage.__number;
	return __CreateNumber(__number);
}

__Value __Modulo(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const double __number =
		__Round(value_1.storage.__number)
		% __Round(value_2.storage.__number);
	return __CreateNumber(__number);
}

__Value __Less(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.__number < value_2.storage.__number;
	return __FromBoolean(result);
}

__Value __LessOrEqual(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.__number <= value_2.storage.__number;
	return __FromBoolean(result);
}

__Value __Greater(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.__number > value_2.storage.__number;
	return __FromBoolean(result);
}

__Value __GreaterOrEqual(const __Value value_1, const __Value value_2) {
	__TestTypeAndNotify(value_1, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(value_2, __VALUE_TYPE_NUMBER);

	const bool result = value_1.storage.__number >= value_2.storage.__number;
	return __FromBoolean(result);
}

__Value ToString(const __Value __value) {
	const size_t NUMBER_FORMAT_BUFFER_SIZE = 1024;

	__TestTypeAndNotify(__value, __VALUE_TYPE_NUMBER);

	char buffer[NUMBER_FORMAT_BUFFER_SIZE];
	sprintf(buffer, "%f", __value.storage.__number);

	return __CreateArrayFromString(buffer);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * __Array operations.
 ******************************************************************************/
__Value __GetArrayItem(const __Value __array, const __Value index) {
	__TestTypeAndNotify(__array, __VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(index, __VALUE_TYPE_NUMBER);
	__TestIndexAndNotify(__array, index);

	const size_t integral_index = __GetIntegralModule(index.storage.__number);
	return __array.storage.__array.data[integral_index];
}

void __SetArrayItem(const __Value __array, const __Value index, const __Value __value) {
	__TestTypeAndNotify(__array, __VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(index, __VALUE_TYPE_NUMBER);
	__TestIndexAndNotify(__array, index);

	const size_t integral_index = __GetIntegralModule(index.storage.__number);
	__array.storage.__array.data[integral_index] = __value;
}

__Value GetLength(const __Value __array) {
	__TestTypeAndNotify(__array, __VALUE_TYPE_ARRAY);
	return __CreateNumber(__array.storage.__array.size);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations.
 ******************************************************************************/
__Value __GetStructureField(const __Value structure, const char* field_name) {
	__TestTypeAndNotify(structure, __VALUE_TYPE_STRUCTURE);

	const size_t field_index = __GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	return structure.storage.structure.fields.data[field_index];
}

void __SetStructureField(
	const __Value structure,
	const char* field_name,
	const __Value __value
) {
	__TestTypeAndNotify(structure, __VALUE_TYPE_STRUCTURE);

	const size_t field_index = __GetStructureFieldIndex(
		structure.storage.structure.name,
		field_name
	);
	structure.storage.structure.fields.data[field_index] = __value;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations.
 ******************************************************************************/
__Value __Not(const __Value __value) {
	const bool result = !__ToBoolean(__value);
	return __FromBoolean(result);
}

__Value __Equal(const __Value value_1, const __Value value_2) {
	if (value_1.type != value_2.type) {
		return __FALSE;
	}

	bool result = false;
	switch (value_1.type) {
		case __VALUE_TYPE_NULL:
			result = true;
			break;
		case __VALUE_TYPE_NUMBER:
			result = value_1.storage.__number == value_2.storage.__number;
			break;
		case __VALUE_TYPE_ARRAY:
			result = value_1.storage.__array.data == value_2.storage.__array.data;
			break;
		case __VALUE_TYPE_STRUCTURE:
			result =
				value_1.storage.structure.fields.data
				== value_2.storage.structure.fields.data;
			break;
	}

	return __FromBoolean(result);
}

__Value __NotEqual(const __Value value_1, const __Value value_2) {
	if (value_1.type != value_2.type) {
		return __TRUE;
	}

	bool result = true;
	switch (value_1.type) {
		case __VALUE_TYPE_NULL:
			result = false;
			break;
		case __VALUE_TYPE_NUMBER:
			result = value_1.storage.__number != value_2.storage.__number;
			break;
		case __VALUE_TYPE_ARRAY:
			result = value_1.storage.__array.data != value_2.storage.__array.data;
			break;
		case __VALUE_TYPE_STRUCTURE:
			result =
				value_1.storage.structure.fields.data
				!= value_2.storage.structure.fields.data;
			break;
	}

	return __FromBoolean(result);
}

__Value __And(const __Value value_1, const __Value value_2) {
	const bool result = __ToBoolean(value_1) && __ToBoolean(value_2);
	return __FromBoolean(result);
}

__Value __Or(const __Value value_1, const __Value value_2) {
	const bool result = __ToBoolean(value_1) || __ToBoolean(value_2);
	return __FromBoolean(result);
}

__Value GetType(const __Value __value) {
	__Value type_name;
	switch (__value.type) {
		case __VALUE_TYPE_NULL:
			type_name = __TYPE_NAME_NULL;
			break;
		case __VALUE_TYPE_NUMBER:
			type_name = __TYPE_NAME_NUMBER;
			break;
		case __VALUE_TYPE_ARRAY:
			type_name = __TYPE_NAME_ARRAY;
			break;
		case __VALUE_TYPE_STRUCTURE:
			type_name = __CreateArrayFromString(__value.storage.structure.name);
			break;
	}

	return type_name;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * System module.
 ******************************************************************************/
__Value GetTime(void) {
	const time_t current_time = time(NULL);
	return __CreateNumber(current_time);
}

__Value Exit(const __Value exit_code) {
	__TestTypeAndNotify(exit_code, __VALUE_TYPE_NUMBER);

	const size_t unwrapped_exit_code = __GetIntegralModule(
		exit_code.storage.__number
	);
	exit(unwrapped_exit_code);

	return __NULL;
}

__Value Read(const __Value stream, const __Value __number) {
	__TestTypeAndNotify(stream, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const size_t file_id = __GetIntegralModule(stream.storage.__number);
	__TestFileIdAndNotify(&__opened_file_storage, file_id);

	FILE* file = __opened_file_storage.files[file_id];

	const size_t number_of_bytes = __GetIntegralModule(__number.storage.__number);
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

	const __Value result = __CreateArrayFromString(buffer);
	free(buffer);

	return result;
}

__Value Write(const __Value stream, const __Value bytes) {
	__TestTypeAndNotify(stream, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(bytes, __VALUE_TYPE_ARRAY);

	const size_t file_id = __GetIntegralModule(stream.storage.__number);
	__TestFileIdAndNotify(&__opened_file_storage, file_id);

	FILE* file = __opened_file_storage.files[file_id];
	char* buffer = __ToString(bytes);

	int result = fputs(buffer, file);
	free(buffer);
	if (result == EOF) {
		return __FALSE;
	}

	result = fflush(file);
	return __FromBoolean(result != EOF);
}

__Value Open(const __Value path, const __Value mode) {
	#define __FILE_OPEN_MODE_READ 0
	#define __FILE_OPEN_MODE_WRITE 1
	#define __FILE_OPEN_MODE_APPEND 2

	__TestTypeAndNotify(path, __VALUE_TYPE_ARRAY);
	__TestTypeAndNotify(mode, __VALUE_TYPE_NUMBER);

	char* path_buffer = __ToString(path);

	const size_t unwrapped_mode = __GetIntegralModule(mode.storage.__number);
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
	if (file == NULL) {
		return __FALSE;
	}

	const size_t file_id = __AddOpenedFile(&__opened_file_storage, file);
	return __CreateNumber(file_id);

	#undef __FILE_OPEN_MODE_APPEND
	#undef __FILE_OPEN_MODE_WRITE
	#undef __FILE_OPEN_MODE_READ
}

__Value Close(const __Value stream) {
	__TestTypeAndNotify(stream, __VALUE_TYPE_NUMBER);

	const size_t file_id = __GetIntegralModule(stream.storage.__number);
	__TestFileIdAndNotify(&__opened_file_storage, file_id);

	FILE* file = __opened_file_storage.files[file_id];
	fclose(file);
	__opened_file_storage.files[file_id] = NULL;

	return __NULL;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Maths module.
 ******************************************************************************/
__Value Sin(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = sin(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Cos(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = cos(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Tg(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = tan(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Arcsin(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = asin(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Arccos(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = acos(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Arctg(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = atan(__number.storage.__number);
	return __CreateNumber(result);
}

__Value SquareRoot(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = sqrt(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Power(const __Value base, const __Value exponent) {
	__TestTypeAndNotify(base, __VALUE_TYPE_NUMBER);
	__TestTypeAndNotify(exponent, __VALUE_TYPE_NUMBER);

	const double result = pow(base.storage.__number, exponent.storage.__number);
	return __CreateNumber(result);
}

__Value Exp(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = exp(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Ln(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	const double result = log(__number.storage.__number);
	return __CreateNumber(result);
}

__Value Integral(const __Value __number) {
	__TestTypeAndNotify(__number, __VALUE_TYPE_NUMBER);

	double result = 0.0;
	modf(__number.storage.__number, &result);
	return __CreateNumber(result);
}

__Value GetRandom(void) {
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
