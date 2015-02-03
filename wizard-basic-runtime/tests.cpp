#include "runtime.h"
#include <gtest/gtest.h>

/*******************************************************************************
 * Interpreter API mocks.
 ******************************************************************************/
void* AllocateMemory(size_t size) {
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
 * Types creation tests.
 ******************************************************************************/
TEST(TypesCreation, TestCreateNull) {
	const auto null = CreateNull();
	EXPECT_EQ(null.type, VALUE_TYPE_NULL);
}

TEST(TypesCreation, TestCreateNumber) {
	const auto TEST_NUMBER_VALUE = 23;

	const auto number = CreateNumber(TEST_NUMBER_VALUE);
	EXPECT_EQ(number.type, VALUE_TYPE_NUMBER);
	EXPECT_EQ(number.storage.number, TEST_NUMBER_VALUE);
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Number operations tests.
 ******************************************************************************/

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
