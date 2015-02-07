#include "runtime.h"
#include <array>
#include <cstring>
#include <cmath>
#include <gtest/gtest.h>
#include <boost/preprocessor.hpp>

using namespace testing;

const auto TEST_NUMBER_1_VALUE = 2.3;
const auto TEST_NUMBER_2_VALUE = 4.2;
const auto TEST_STRING_VALUE = std::string("test");
const auto TEST_SIZE_VALUE = 23;
const auto FALSE_VALUE = 0.0;
const auto TRUE_VALUE = 1.0;
const auto TEST_STRUCTURE_NAME = std::string("Test");
const auto TEST_STRUCTURE_FIELD_NAME = std::string("test");

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

/*******************************************************************************
 * Utils.
 ******************************************************************************/
void ValidateArrayContents(const __Value array, const std::string contents) {
	EXPECT_EQ(array.storage.array.size, contents.size());

	for (size_t i = 0; i < array.storage.array.size; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, __VALUE_TYPE_NUMBER);
		EXPECT_EQ(array.storage.array.data[i].storage.number, contents[i]);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Constats tests.
 ******************************************************************************/
TEST(Constats, TestNull) {
	EXPECT_EQ(__NULL.type, __VALUE_TYPE_NULL);
	EXPECT_EQ(__NULL.storage.number, 0.0);
}

TEST(Constats, TestFalse) {
	EXPECT_EQ(__FALSE.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(__FALSE.storage.number, 0.0);
}

TEST(Constats, TestTrue) {
	EXPECT_EQ(__TRUE.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(__TRUE.storage.number, 1.0);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils tests.
 ******************************************************************************/
// __InitializeConstants() test
TEST(Utils, TestInitializeConstants) {
	__InitializeConstants();

	EXPECT_EQ(__TYPE_NAME_NULL.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_NULL, "null");

	EXPECT_EQ(__TYPE_NAME_NUMBER.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_NUMBER, "number");

	EXPECT_EQ(__TYPE_NAME_ARRAY.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_ARRAY, "array");
}

// __InitializeOpenedFileStorage() test
TEST(Utils, TestInitializeOpenedFileStorage) {
	const auto START_CAPACITY_OF_OPENED_FILE_STORAGE = 12;
	const auto START_NUMBER_OF_OPENED_FILES = 3;
	const auto STANDART_STREAM_INPUT_ID = 0;
	const auto STANDART_STREAM_OUTPUT_ID = 1;
	const auto STANDART_STREAM_ERROR_ID = 2;

	__InitializeOpenedFileStorage();

	EXPECT_EQ(
		__opened_file_storage.capacity,
		START_CAPACITY_OF_OPENED_FILE_STORAGE
	);
	EXPECT_EQ(__opened_file_storage.number, START_NUMBER_OF_OPENED_FILES);
	EXPECT_EQ(__opened_file_storage.files[STANDART_STREAM_INPUT_ID], stdin);
	EXPECT_EQ(__opened_file_storage.files[STANDART_STREAM_OUTPUT_ID], stdout);
	EXPECT_EQ(__opened_file_storage.files[STANDART_STREAM_ERROR_ID], stderr);
}

// __WrapCommandLineArguments() test
TEST(Utils, TestWrapCommandLineArguments) {
	const auto TEST_ARGUMENTS_NUMBER = 2;

	const char* test_arguments[TEST_ARGUMENTS_NUMBER];
	for (size_t i = 0; i < TEST_ARGUMENTS_NUMBER; i++) {
		const auto test_argument = "argument_" + std::to_string(i);
		test_arguments[i] = (new std::string(test_argument))->c_str();
	}

	const auto result = __WrapCommandLineArguments(
		test_arguments,
		TEST_ARGUMENTS_NUMBER
	);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	EXPECT_EQ(result.storage.array.size, TEST_ARGUMENTS_NUMBER);

	for (size_t i = 0; i < TEST_ARGUMENTS_NUMBER; i++) {
		const auto item = result.storage.array.data[i];

		EXPECT_EQ(item.type, __VALUE_TYPE_ARRAY);
		ValidateArrayContents(item, "argument_" + std::to_string(i));
	}
}

// __HasAllowedType() tests
TEST(Utils, TestHasAllowedTypeWithOneAllowedType) {
	const auto null = __NULL;
	const auto result = __HasAllowedType(null, __VALUE_TYPE_NULL);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndRightValue) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __HasAllowedType(
		array,
		__VALUE_TYPE_NULL | __VALUE_TYPE_NUMBER | __VALUE_TYPE_ARRAY
	);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndNotRightValue) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __HasAllowedType(
		structure,
		__VALUE_TYPE_NULL | __VALUE_TYPE_NUMBER | __VALUE_TYPE_ARRAY
	);

	EXPECT_FALSE(result);
}

// __IsValidIndex() tests
TEST(Utils, TestIsValidIndexWithNegativeIndex) {
	const auto TEST_NEGATIVE_INDEX_VALUE = -5;

	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_NEGATIVE_INDEX_VALUE);
	const auto result = __IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidIndexWithNullIndex) {
	const auto TEST_NULL_INDEX_VALUE = 0;

	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_NULL_INDEX_VALUE);
	const auto result = __IsValidIndex(array, index);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidIndexWithMiddleIndex) {
	const auto TEST_MIDDLE_INDEX_VALUE = 5;

	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_MIDDLE_INDEX_VALUE);
	const auto result = __IsValidIndex(array, index);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidIndexWithSizeIndex) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_SIZE_VALUE);
	const auto result = __IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidIndexWithTooBigIndex) {
	const auto TEST_TOO_BIG_INDEX_VALUE = 42;

	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_TOO_BIG_INDEX_VALUE);
	const auto result = __IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

// __ToBoolean() tests
TEST(Utils, TestToBooleanWithNullType) {
	const auto null = __NULL;
	const auto result = __ToBoolean(null);

	EXPECT_FALSE(result);
}

TEST(Utils, TestToBooleanWithNullNumber) {
	const auto TEST_NULL_NUMBER_VALUE = 0.0;

	const auto number = __CreateNumber(TEST_NULL_NUMBER_VALUE);
	const auto result = __ToBoolean(number);

	EXPECT_FALSE(result);
}

TEST(Utils, TestToBooleanWithNotNullNumber) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = __ToBoolean(number);

	EXPECT_TRUE(result);
}

TEST(Utils, TestToBooleanWithArray) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __ToBoolean(array);

	EXPECT_TRUE(result);
}

TEST(Utils, TestToBooleanWithStructure) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __ToBoolean(structure);

	EXPECT_TRUE(result);
}

// __FromBoolean() test
TEST(Utils, TestFromBoolean) {
	const auto result = __FromBoolean(true);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

// __Round() tests
TEST(Utils, TestRoundWithPositiveNumber) {
	const auto TEST_NUMBER_VALUE = 2.8;

	const auto result = __Round(TEST_NUMBER_VALUE);
	EXPECT_EQ(result, 3);
}

TEST(Utils, TestRoundWithNegativeNumber) {
	const auto TEST_NUMBER_VALUE = -2.8;

	const auto result = __Round(TEST_NUMBER_VALUE);
	EXPECT_EQ(result, -3);
}

// __GetIntegralModule() tests
TEST(Utils, TestGetIntegralModuleWithPositiveNumber) {
	const auto TEST_NUMBER_VALUE = 2.8;

	const auto result = __GetIntegralModule(TEST_NUMBER_VALUE);
	EXPECT_EQ(result, 2);
}

TEST(Utils, TestGetIntegralModuleWithNegativeNumber) {
	const auto TEST_NUMBER_VALUE = -2.8;

	const auto result = __GetIntegralModule(TEST_NUMBER_VALUE);
	EXPECT_EQ(result, 2);
}

// __ToString() test
TEST(Utils, TestToString) {
	const auto array = __CreateArrayFromString(TEST_STRING_VALUE.c_str());
	const auto result = __ToString(array);

	EXPECT_EQ(std::string(result), TEST_STRING_VALUE);
}

// __IsValidFileId() tests
TEST(Utils, TestIsValidFileIdWithNullIndex) {
	const auto TEST_OPENED_FILE_STORAGE_SIZE = 3;
	FILE* TEST_OPENED_FILES[] = {stdin, stdout, stderr};
	const auto TEST_OPENED_FILE_STORAGE = __OpenedFileStorage{
		TEST_OPENED_FILES,
		TEST_OPENED_FILE_STORAGE_SIZE,
		TEST_OPENED_FILE_STORAGE_SIZE
	};
	const auto TEST_NULL_INDEX_VALUE = 0;

	const auto result = __IsValidFileId(
		&TEST_OPENED_FILE_STORAGE,
		TEST_NULL_INDEX_VALUE
	);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidFileIdWithMiddleIndexAndNotNullFiles) {
	const auto TEST_OPENED_FILE_STORAGE_SIZE = 3;
	FILE* TEST_OPENED_FILES[] = {stdin, stdout, stderr};
	const auto TEST_OPENED_FILE_STORAGE = __OpenedFileStorage{
		TEST_OPENED_FILES,
		TEST_OPENED_FILE_STORAGE_SIZE,
		TEST_OPENED_FILE_STORAGE_SIZE
	};
	const auto TEST_NULL_INDEX_VALUE = 1;

	const auto result = __IsValidFileId(
		&TEST_OPENED_FILE_STORAGE,
		TEST_NULL_INDEX_VALUE
	);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidFileIdWithMiddleIndexAndNullFiles) {
	const auto TEST_OPENED_FILE_STORAGE_SIZE = 3;
	FILE* TEST_OPENED_FILES[] = {NULL, NULL, NULL};
	const auto TEST_OPENED_FILE_STORAGE = __OpenedFileStorage{
		TEST_OPENED_FILES,
		TEST_OPENED_FILE_STORAGE_SIZE,
		TEST_OPENED_FILE_STORAGE_SIZE
	};
	const auto TEST_NULL_INDEX_VALUE = 1;

	const auto result = __IsValidFileId(
		&TEST_OPENED_FILE_STORAGE,
		TEST_NULL_INDEX_VALUE
	);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidFileIdWithSizeIndex) {
	const auto TEST_OPENED_FILE_STORAGE_SIZE = 3;
	FILE* TEST_OPENED_FILES[] = {stdin, stdout, stderr};
	const auto TEST_OPENED_FILE_STORAGE = __OpenedFileStorage{
		TEST_OPENED_FILES,
		TEST_OPENED_FILE_STORAGE_SIZE,
		TEST_OPENED_FILE_STORAGE_SIZE
	};

	const auto result = __IsValidFileId(
		&TEST_OPENED_FILE_STORAGE,
		TEST_OPENED_FILE_STORAGE_SIZE
	);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidFileIdWithTooBigIndex) {
	const auto TEST_OPENED_FILE_STORAGE_SIZE = 3;
	FILE* TEST_OPENED_FILES[] = {stdin, stdout, stderr};
	const auto TEST_OPENED_FILE_STORAGE = __OpenedFileStorage{
		TEST_OPENED_FILES,
		TEST_OPENED_FILE_STORAGE_SIZE,
		TEST_OPENED_FILE_STORAGE_SIZE
	};
	const auto TEST_NULL_INDEX_VALUE = 42;

	const auto result = __IsValidFileId(
		&TEST_OPENED_FILE_STORAGE,
		TEST_NULL_INDEX_VALUE
	);

	EXPECT_FALSE(result);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation tests.
 ******************************************************************************/
TEST(TypesCreation, TestNULL) {
	const auto null = __NULL;
	EXPECT_EQ(null.type, __VALUE_TYPE_NULL);
}

TEST(TypesCreation, TestCreateNumber) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);

	EXPECT_EQ(number.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(number.storage.number, TEST_NUMBER_1_VALUE);
}

TEST(TypesCreation, TestCreateArrayData) {
	const auto array_data = __CreateArrayData(TEST_SIZE_VALUE);

	EXPECT_EQ(array_data.size, TEST_SIZE_VALUE);

	for (size_t i = 0; i < TEST_SIZE_VALUE; i++) {
		EXPECT_EQ(array_data.data[i].type, __VALUE_TYPE_NULL);
	}
}

TEST(TypesCreation, TestCreateArray) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);

	EXPECT_EQ(array.type, __VALUE_TYPE_ARRAY);
	EXPECT_EQ(array.storage.array.size, TEST_SIZE_VALUE);

	for (size_t i = 0; i < TEST_SIZE_VALUE; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, __VALUE_TYPE_NULL);
	}
}

TEST(TypesCreation, TestCreateArrayFromList) {
	#define TEST_LIST_SIZE 2
	const auto TEST_LIST = std::array<__Number, TEST_LIST_SIZE>{
		TEST_NUMBER_1_VALUE,
		TEST_NUMBER_2_VALUE
	};

	#define TEST_LIST_VALUE(z, index, name) __CreateNumber(TEST_LIST[index])
	const auto array = __CreateArrayFromList(
		TEST_LIST_SIZE,
		BOOST_PP_ENUM(TEST_LIST_SIZE, TEST_LIST_VALUE, ~)
	);

	EXPECT_EQ(array.type, __VALUE_TYPE_ARRAY);
	EXPECT_EQ(array.storage.array.size, TEST_LIST_SIZE);

	for (size_t i = 0; i < TEST_LIST_SIZE; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, __VALUE_TYPE_NUMBER);
		EXPECT_EQ(array.storage.array.data[i].storage.number, TEST_LIST[i]);
	}

	#undef TEST_LIST_VALUE
	#undef TEST_LIST_SIZE
}

TEST(TypesCreation, TestCreateArrayFromString) {
	const auto array = __CreateArrayFromString(TEST_STRING_VALUE.c_str());

	EXPECT_EQ(array.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(array, TEST_STRING_VALUE);
}

TEST(TypesCreation, TestCreateStructure) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());

	EXPECT_EQ(structure.type, __VALUE_TYPE_STRUCTURE);

	const auto number_of_fields = __GetStructureFieldsNumber(
		TEST_STRUCTURE_NAME.c_str()
	);
	EXPECT_EQ(structure.storage.structure.fields.size, number_of_fields);

	for (size_t i = 0; i < number_of_fields; i++) {
		EXPECT_EQ(
			structure.storage.structure.fields.data[i].type,
			__VALUE_TYPE_NULL
		);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * __Number operations tests.
 ******************************************************************************/
TEST(NumberOperations, TestUnaryMinus) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = __UnaryMinus(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, -TEST_NUMBER_1_VALUE);
}

TEST(NumberOperations, TestAdd) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Add(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE + TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestSubtract) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Subtract(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE - TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestMultiply) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Multiply(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE * TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestDivide) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Divide(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE / TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestModulo) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Modulo(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(
		result.storage.number,
		(long)round(TEST_NUMBER_1_VALUE) % (long)round(TEST_NUMBER_2_VALUE)
	);
}

TEST(NumberOperations, TestLess) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Less(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestLessOrEqual) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __LessOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestGreater) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Greater(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}

TEST(NumberOperations, TestGreaterOrEqual) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __GreaterOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}

TEST(NumberOperations, TestToString) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = ToString(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "2.300000");
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * __Array operations tests.
 ******************************************************************************/
TEST(ArrayOperations, TestGetArrayItem) {
	const auto TEST_SIZE_VALUE = 1;
	const auto TEST_ARRAY_ITEM_INDEX = 0;

	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto array = __CreateArrayFromList(TEST_SIZE_VALUE, number);
	const auto index = __CreateNumber(TEST_ARRAY_ITEM_INDEX);
	const auto result = __GetArrayItem(array, index);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE);
}

TEST(ArrayOperations, TestSetArrayItem) {
	const auto TEST_ARRAY_ITEM_INDEX = 12;

	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto index = __CreateNumber(TEST_ARRAY_ITEM_INDEX);
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	__SetArrayItem(array, index, number);

	EXPECT_EQ(
		array.storage.array.data[TEST_ARRAY_ITEM_INDEX].type,
		__VALUE_TYPE_NUMBER
	);
	EXPECT_EQ(
		array.storage.array.data[TEST_ARRAY_ITEM_INDEX].storage.number,
		TEST_NUMBER_1_VALUE
	);
}

TEST(ArrayOperations, TestGetLength) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = GetLength(array);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_SIZE_VALUE);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations tests.
 ******************************************************************************/
TEST(StructureOperations, TestGetStructureField) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __GetStructureField(
		structure,
		TEST_STRUCTURE_FIELD_NAME.c_str()
	);

	EXPECT_EQ(result.type, __VALUE_TYPE_NULL);
}

TEST(StructureOperations, TestSetStructureField) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	__SetStructureField(structure, TEST_STRUCTURE_FIELD_NAME.c_str(), number);

	const auto field_index = __GetStructureFieldIndex(
		TEST_STRUCTURE_NAME.c_str(),
		TEST_STRUCTURE_FIELD_NAME.c_str()
	);
	EXPECT_EQ(
		structure.storage.structure.fields.data[field_index].type,
		__VALUE_TYPE_NUMBER
	);
	EXPECT_EQ(
		structure.storage.structure.fields.data[field_index].storage.number,
		TEST_NUMBER_1_VALUE
	);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations tests.
 ******************************************************************************/
// __Not() test
TEST(RestOperations, TestNot) {
	const auto result = __Not(__TRUE);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __Equal() tests
TEST(RestOperations, TestEqualWithNulls) {
	const auto null_1 = __NULL;
	const auto null_2 = __NULL;
	const auto result = __Equal(null_1, null_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNumbers) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Equal(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestEqualWithEqualArrays) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __Equal(array, array);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNotEqualArrays) {
	const auto array_1 = __CreateArray(TEST_SIZE_VALUE);
	const auto array_2 = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __Equal(array_1, array_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestEqualWithEqualStructures) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __Equal(structure, structure);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNotEqualStructures) {
	const auto structure_1 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto structure_2 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __Equal(structure_1, structure_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __NotEqual() tests
TEST(RestOperations, TestNotEqualWithNulls) {
	const auto null_1 = __NULL;
	const auto null_2 = __NULL;
	const auto result = __NotEqual(null_1, null_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNumbers) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __NotEqual(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestNotEqualWithEqualArrays) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __NotEqual(array, array);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNotEqualArrays) {
	const auto array_1 = __CreateArray(TEST_SIZE_VALUE);
	const auto array_2 = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __NotEqual(array_1, array_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestNotEqualWithEqualStructures) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __NotEqual(structure, structure);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNotEqualStructures) {
	const auto structure_1 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto structure_2 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __NotEqual(structure_1, structure_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

// __And() test
TEST(RestOperations, TestAnd) {
	const auto result = __And(__TRUE, __FALSE);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __Or() test
TEST(RestOperations, TestOr) {
	const auto result = __Or(__TRUE, __FALSE);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

// GetType() tests
TEST(RestOperations, TestGetTypeWithNull) {
	const auto null = __NULL;
	const auto result = GetType(null);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "null");
}

TEST(RestOperations, TestGetTypeWithNumber) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = GetType(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "number");
}

TEST(RestOperations, TestGetTypeWithArray) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = GetType(array);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "array");
}

TEST(RestOperations, TestGetTypeWithStructure) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = GetType(structure);

	EXPECT_EQ(result.type, __VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, TEST_STRUCTURE_NAME);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Maths module tests.
 ******************************************************************************/
TEST(MathsModule, TestSin) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Sin(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::sin(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestCos) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Cos(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::cos(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestTg) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Tg(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::tan(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestArcsin) {
	const auto TEST_NUMBER_VALUE = 0.23;

	const auto number = __CreateNumber(TEST_NUMBER_VALUE);
	const auto result = Arcsin(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::asin(TEST_NUMBER_VALUE));
}

TEST(MathsModule, TestArccos) {
	const auto TEST_NUMBER_VALUE = 0.23;

	const auto number = __CreateNumber(TEST_NUMBER_VALUE);
	const auto result = Arccos(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::acos(TEST_NUMBER_VALUE));
}

TEST(MathsModule, TestArctg) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Arctg(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::atan(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestSquareRoot) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = SquareRoot(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::sqrt(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestPower) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Power(number_1, number_2);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(
		result.storage.number,
		std::pow(TEST_NUMBER_1_VALUE, TEST_NUMBER_2_VALUE)
	);
}

TEST(MathsModule, TestExp) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Exp(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::exp(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestLn) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Ln(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, std::log(TEST_NUMBER_1_VALUE));
}

TEST(MathsModule, TestIntegral) {
	const auto TEST_INTERGAL_NUMBER_VALUE = 2.0;

	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = Integral(number);

	EXPECT_EQ(result.type, __VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_INTERGAL_NUMBER_VALUE);
}
//------------------------------------------------------------------------------

int main(int number_of_arguments, char* arguments[]) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
