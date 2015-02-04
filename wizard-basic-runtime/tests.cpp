#include "runtime.h"
#include <array>
#include <cstring>
#include <gtest/gtest.h>
#include <boost/preprocessor.hpp>

const auto TEST_NUMBER_1_VALUE = 2.3;
const auto TEST_NUMBER_2_VALUE = 4.2;
const auto TEST_SIZE_VALUE = 23;
const auto FALSE_VALUE = 0.0;
const auto TRUE_VALUE = 1.0;
const auto TEST_STRUCTURE_NAME = std::string("Test");

/*******************************************************************************
 * Interpreter API mocks.
 ******************************************************************************/
void* AllocateMemory(const size_t size) {
	return malloc(size);
}

size_t GetStructureFieldsNumber(const char* name) {
	(void)name;
	return 1;
}

size_t GetStructureFieldIndex(
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
 * Utils tests.
 ******************************************************************************/
// InitializeConstants() test
TEST(Utils, TestInitializeConstants) {
	InitializeConstants();

	ValidateArrayContents(__TYPE_NAME_NULL, "null");
	ValidateArrayContents(__TYPE_NAME_NUMBER, "number");
	ValidateArrayContents(__TYPE_NAME_ARRAY, "array");
}

// HasAllowedType() tests
TEST(Utils, TestHasAllowedTypeWithOneAllowedType) {
	const auto null = CreateNull();
	const auto result = HasAllowedType(null, VALUE_TYPE_NULL);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndRightValue) {
	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto result = HasAllowedType(
		array,
		VALUE_TYPE_NULL | VALUE_TYPE_NUMBER | VALUE_TYPE_ARRAY
	);

	EXPECT_TRUE(result);
}

TEST(Utils, TestHasAllowedTypeWithSomeAllowedTypeAndNotRightValue) {
	const auto structure = CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = HasAllowedType(
		structure,
		VALUE_TYPE_NULL | VALUE_TYPE_NUMBER | VALUE_TYPE_ARRAY
	);

	EXPECT_FALSE(result);
}

// IsValidIndex() tests
TEST(Utils, TestIsValidIndexWithNegativeIndex) {
	const auto TEST_NEGATIVE_INDEX_VALUE = -5;

	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto index = CreateNumber(TEST_NEGATIVE_INDEX_VALUE);
	const auto result = IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidIndexWithNullIndex) {
	const auto TEST_NULL_INDEX_VALUE = 0;

	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto index = CreateNumber(TEST_NULL_INDEX_VALUE);
	const auto result = IsValidIndex(array, index);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidIndexWithMiddleIndex) {
	const auto TEST_MIDDLE_INDEX_VALUE = 5;

	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto index = CreateNumber(TEST_MIDDLE_INDEX_VALUE);
	const auto result = IsValidIndex(array, index);

	EXPECT_TRUE(result);
}

TEST(Utils, TestIsValidIndexWithSizeIndex) {
	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto index = CreateNumber(TEST_SIZE_VALUE);
	const auto result = IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

TEST(Utils, TestIsValidIndexWithTooBigIndex) {
	const auto TEST_TOO_BIG_INDEX_VALUE = 42;

	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto index = CreateNumber(TEST_TOO_BIG_INDEX_VALUE);
	const auto result = IsValidIndex(array, index);

	EXPECT_FALSE(result);
}

// ToBoolean() tests
TEST(Utils, TestToBooleanWithNullType) {
	const auto null = CreateNull();
	const auto result = ToBoolean(null);

	EXPECT_FALSE(result);
}

TEST(Utils, TestToBooleanWithNullNumber) {
	const auto TEST_NULL_NUMBER_VALUE = 0.0;

	const auto number = CreateNumber(TEST_NULL_NUMBER_VALUE);
	const auto result = ToBoolean(number);

	EXPECT_FALSE(result);
}

TEST(Utils, TestToBooleanWithNotNullNumber) {
	const auto number = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = ToBoolean(number);

	EXPECT_TRUE(result);
}

TEST(Utils, TestToBooleanWithArray) {
	const auto array = CreateArray(TEST_SIZE_VALUE);
	const auto result = ToBoolean(array);

	EXPECT_TRUE(result);
}

TEST(Utils, TestToBooleanWithStructure) {
	const auto structure = CreateStructure(TEST_STRUCTURE_NAME.c_str());
	const auto result = ToBoolean(structure);

	EXPECT_TRUE(result);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Types creation tests.
 ******************************************************************************/
TEST(TypesCreation, TestCreateNull) {
	const auto null = CreateNull();
	EXPECT_EQ(null.type, VALUE_TYPE_NULL);
}

TEST(TypesCreation, TestCreateNumber) {
	const auto number = CreateNumber(TEST_NUMBER_1_VALUE);

	EXPECT_EQ(number.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(number.storage.number, TEST_NUMBER_1_VALUE);
}

TEST(TypesCreation, TestCreateArrayData) {
	const auto array_data = CreateArrayData(TEST_SIZE_VALUE);

	EXPECT_EQ(array_data.size, TEST_SIZE_VALUE);

	for (size_t i = 0; i < TEST_SIZE_VALUE; i++) {
		EXPECT_EQ(array_data.data[i].type, VALUE_TYPE_NULL);
	}
}

TEST(TypesCreation, TestCreateArray) {
	const auto array = CreateArray(TEST_SIZE_VALUE);

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

	#define TEST_LIST_VALUE(z, index, name) CreateNumber(TEST_LIST[index])
	const auto array = CreateArrayFromList(
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

	const auto array = CreateArrayFromString(TEST_STRING.c_str());

	EXPECT_EQ(array.type, VALUE_TYPE_ARRAY);
	ValidateArrayContents(array, TEST_STRING);
}

TEST(TypesCreation, TestCreateStructure) {
	const auto structure = CreateStructure(TEST_STRUCTURE_NAME.c_str());

	EXPECT_EQ(structure.type, VALUE_TYPE_STRUCTURE);

	const auto number_of_fields = GetStructureFieldsNumber(
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
	const auto number = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto result = UnaryMinus(number);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, -TEST_NUMBER_1_VALUE);
}

TEST(NumberOperations, TestAdd) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Add(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE + TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestSubtract) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Subtract(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE - TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestMultiply) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Multiply(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE * TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestDivide) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Divide(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TEST_NUMBER_1_VALUE / TEST_NUMBER_2_VALUE);
}

TEST(NumberOperations, TestModulo) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Modulo(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(
		result.storage.number,
		(long)round(TEST_NUMBER_1_VALUE) % (long)round(TEST_NUMBER_2_VALUE)
	);
}

TEST(NumberOperations, TestLess) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Less(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestLessOrEqual) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = LessOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, TRUE_VALUE);
}

TEST(NumberOperations, TestGreater) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = Greater(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}

TEST(NumberOperations, TestGreaterOrEqual) {
	const auto number_1 = CreateNumber(TEST_NUMBER_1_VALUE);
	const auto number_2 = CreateNumber(TEST_NUMBER_2_VALUE);
	const auto result = GreaterOrEqual(number_1, number_2);

	EXPECT_EQ(result.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(result.storage.number, FALSE_VALUE);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Array operations tests.
 ******************************************************************************/

//------------------------------------------------------------------------------

/*******************************************************************************
 * Structure operations tests.
 ******************************************************************************/

//------------------------------------------------------------------------------

/*******************************************************************************
 * Rest operations tests.
 ******************************************************************************/

//------------------------------------------------------------------------------

int main(int number_of_arguments, char** arguments) {
	::testing::InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
