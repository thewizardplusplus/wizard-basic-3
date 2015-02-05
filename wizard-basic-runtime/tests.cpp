#include "runtime.h"
#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <boost/preprocessor.hpp>

using namespace testing;

const auto TEST_NUMBER_1_VALUE = 2.3;
const auto TEST_NUMBER_2_VALUE = 4.2;
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
void ValidateArrayContents(const Value array, const std::string contents) {
	EXPECT_EQ(array.storage.array.size, contents.size());

	for (size_t i = 0; i < array.storage.array.size; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, VALUE_TYPE_NUMBER);
		EXPECT_EQ(array.storage.array.data[i].storage.number, contents[i]);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
* Constats tests.
******************************************************************************/
TEST(Constats, TestNull) {
	EXPECT_EQ(__NULL.type, VALUE_TYPE_NULL);
	EXPECT_EQ(__NULL.storage.number, 0.0);
}

TEST(Constats, TestFalse) {
	EXPECT_EQ(__FALSE.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(__FALSE.storage.number, 0.0);
}

TEST(Constats, TestTrue) {
	EXPECT_EQ(__TRUE.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(__TRUE.storage.number, 1.0);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils tests.
 ******************************************************************************/
// __InitializeConstants() test
TEST(Utils, TestInitializeConstants) {
	__InitializeConstants();

	EXPECT_EQ(__TYPE_NAME_NULL.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_NULL, "null");

	EXPECT_EQ(__TYPE_NAME_NUMBER.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_NUMBER, "number");

	EXPECT_EQ(__TYPE_NAME_ARRAY.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(__TYPE_NAME_ARRAY, "array");
}

// __HasAllowedType() tests
TEST(Utils, TestHasAllowedTypeWithOneAllowedType) {
	const auto null = __NULL;
	const auto result = __HasAllowedType(null, VALUE_TYPE_NULL);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndRightValue) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __HasAllowedType(
		array,
		VALUE_TYPE_NULL | VALUE_TYPE_NUMBER | VALUE_TYPE_ARRAY
	);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndNotRightValue) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __HasAllowedType(
		structure,
		VALUE_TYPE_NULL | VALUE_TYPE_NUMBER | VALUE_TYPE_ARRAY
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
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation tests.
 ******************************************************************************/
TEST(TypesCreation, TestNULL) {
	const auto null = __NULL;
	EXPECT_EQ(null.type, VALUE_TYPE_NULL);
}

TEST(TypesCreation, TestCreateNumber) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);

	EXPECT_EQ(number.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(number.storage.number, TEST_NUMBER_1_VALUE);
}

TEST(TypesCreation, TestCreateArrayData) {
	const auto array_data = __CreateArrayData(TEST_SIZE_VALUE);

	EXPECT_EQ(array_data.size, TEST_SIZE_VALUE);

	for (size_t i = 0; i < TEST_SIZE_VALUE; i++) {
		EXPECT_EQ(array_data.data[i].type, VALUE_TYPE_NULL);
	}
}

TEST(TypesCreation, TestCreateArray) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);

	EXPECT_EQ(array.type, VALUE_TYPE_ARRAY);
	EXPECT_EQ(array.storage.array.size, TEST_SIZE_VALUE);

	for (size_t i = 0; i < TEST_SIZE_VALUE; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, VALUE_TYPE_NULL);
	}
}

TEST(TypesCreation, TestCreateArrayFromList) {
	#define TEST_LIST_SIZE 2
	const auto TEST_LIST = std::array<Number, TEST_LIST_SIZE>{
		TEST_NUMBER_1_VALUE,
		TEST_NUMBER_2_VALUE
	};

	#define TEST_LIST_VALUE(z, index, name) __CreateNumber(TEST_LIST[index])
	const auto array = __CreateArrayFromList(
		TEST_LIST_SIZE,
		BOOST_PP_ENUM(TEST_LIST_SIZE, TEST_LIST_VALUE, ~)
	);

	EXPECT_EQ(array.type, VALUE_TYPE_ARRAY);
	EXPECT_EQ(array.storage.array.size, TEST_LIST_SIZE);

	for (size_t i = 0; i < TEST_LIST_SIZE; i++) {
		EXPECT_EQ(array.storage.array.data[i].type, VALUE_TYPE_NUMBER);
		EXPECT_EQ(array.storage.array.data[i].storage.number, TEST_LIST[i]);
	}

	#undef TEST_LIST_VALUE
	#undef TEST_LIST_SIZE
}

TEST(TypesCreation, TestCreateArrayFromString) {
	const auto TEST_STRING = std::string("test");

	const auto array = __CreateArrayFromString(TEST_STRING.c_str());

	EXPECT_EQ(array.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(array, TEST_STRING);
}

TEST(TypesCreation, TestCreateStructure) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());

	EXPECT_EQ(structure.type, VALUE_TYPE_STRUCTURE);

	const auto number_of_fields = __GetStructureFieldsNumber(
		TEST_STRUCTURE_NAME.c_str()
	);
	EXPECT_EQ(structure.storage.structure.fields.size, number_of_fields);

	for (size_t i = 0; i < number_of_fields; i++) {
		EXPECT_EQ(
			structure.storage.structure.fields.data[i].type,
			VALUE_TYPE_NULL
		);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Number operations tests.
 ******************************************************************************/
TEST(NumberOperations, TestUnaryMinus) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = __UnaryMinus(number);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, -TEST_NUMBER_1_VALUE);
}

TEST(NumberOperations, TestAdd) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Add(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE + TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestSubtract) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Subtract(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE - TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestMultiply) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Multiply(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE * TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestDivide) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Divide(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE / TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestModulo) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Modulo(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(
		result.storage.number,
		(long)round(TEST_NUMBER_1_VALUE) % (long)round(TEST_NUMBER_2_VALUE)
	);
}

TEST(NumberOperations, TestLess) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Less(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestLessOrEqual) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __LessOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestGreater) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Greater(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}

TEST(NumberOperations, TestGreaterOrEqual) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __GreaterOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}

TEST(NumberOperations, TestToString) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = ToString(number);

	EXPECT_EQ(result.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "2.300000");
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Array operations tests.
 ******************************************************************************/
TEST(ArrayOperations, TestGetArrayItem) {
	const auto TEST_SIZE_VALUE = 1;
	const auto TEST_ARRAY_ITEM_INDEX = 0;

	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto array = __CreateArrayFromList(TEST_SIZE_VALUE, number);
	const auto index = __CreateNumber(TEST_ARRAY_ITEM_INDEX);
	const auto result = __GetArrayItem(array, index);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
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
		VALUE_TYPE_NUMBER
	);
	EXPECT_EQ(
		array.storage.array.data[TEST_ARRAY_ITEM_INDEX].storage.number,
		TEST_NUMBER_1_VALUE
	);
}

TEST(ArrayOperations, TestGetLength) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = GetLength(array);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
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

	EXPECT_EQ(result.type, VALUE_TYPE_NULL);
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
		VALUE_TYPE_NUMBER
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

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __Equal() tests
TEST(RestOperations, TestEqualWithNulls) {
	const auto null_1 = __NULL;
	const auto null_2 = __NULL;
	const auto result = __Equal(null_1, null_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNumbers) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __Equal(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestEqualWithEqualArrays) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __Equal(array, array);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNotEqualArrays) {
	const auto array_1 = __CreateArray(TEST_SIZE_VALUE);
	const auto array_2 = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __Equal(array_1, array_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestEqualWithEqualStructures) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __Equal(structure, structure);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestEqualWithNotEqualStructures) {
	const auto structure_1 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto structure_2 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __Equal(structure_1, structure_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __NotEqual() tests
TEST(RestOperations, TestNotEqualWithNulls) {
	const auto null_1 = __NULL;
	const auto null_2 = __NULL;
	const auto result = __NotEqual(null_1, null_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNumbers) {
	const auto number_1 = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = __CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = __NotEqual(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestNotEqualWithEqualArrays) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __NotEqual(array, array);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNotEqualArrays) {
	const auto array_1 = __CreateArray(TEST_SIZE_VALUE);
	const auto array_2 = __CreateArray(TEST_SIZE_VALUE);
	const auto result = __NotEqual(array_1, array_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

TEST(RestOperations, TestNotEqualWithEqualStructures) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __NotEqual(structure, structure);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

TEST(RestOperations, TestNotEqualWithNotEqualStructures) {
	const auto structure_1 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto structure_2 = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = __NotEqual(structure_1, structure_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

// __And() test
TEST(RestOperations, TestAnd) {
	const auto result = __And(__TRUE, __FALSE);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 0.0);
}

// __Or() test
TEST(RestOperations, TestOr) {
	const auto result = __Or(__TRUE, __FALSE);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, 1.0);
}

// GetType() tests
TEST(RestOperations, TestGetTypeWithNull) {
	const auto null = __NULL;
	const auto result = GetType(null);

	EXPECT_EQ(result.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "null");
}

TEST(RestOperations, TestGetTypeWithNumber) {
	const auto number = __CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = GetType(number);

	EXPECT_EQ(result.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "number");
}

TEST(RestOperations, TestGetTypeWithArray) {
	const auto array = __CreateArray(TEST_SIZE_VALUE);
	const auto result = GetType(array);

	EXPECT_EQ(result.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, "array");
}

TEST(RestOperations, TestGetTypeWithStructure) {
	const auto structure = __CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = GetType(structure);

	EXPECT_EQ(result.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(result, TEST_STRUCTURE_NAME);
}
//------------------------------------------------------------------------------

int main(int number_of_arguments, char** arguments) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
