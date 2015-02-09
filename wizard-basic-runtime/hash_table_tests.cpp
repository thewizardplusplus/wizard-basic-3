#include "structure_storage.h"
#include <cstring>
#include <gtest/gtest.h>

using namespace testing;

/*******************************************************************************
 * Interpreter API mocks.
 ******************************************************************************/
void* __AllocateMemory(const size_t size) {
	return malloc(size);
}
//------------------------------------------------------------------------------

TEST(StructureStorage, TestFieldInfo) {
	const auto TEST_FIELD_NAME_VALUE = std::string("test");
	const auto TEST_FIELD_INDEX_VALUE = 23;

	__FieldMap fields = NULL;

	const size_t name_length = TEST_FIELD_NAME_VALUE.size() + 1;
	char* name = (char*)malloc(name_length);
	std::strcpy(name, TEST_FIELD_NAME_VALUE.c_str());

	__AddField(&fields, name, TEST_FIELD_INDEX_VALUE);
	const auto field = __FindField(&fields, TEST_FIELD_NAME_VALUE.c_str());

	ASSERT_NE(field, nullptr);
	EXPECT_EQ(field->index, TEST_FIELD_INDEX_VALUE);
}

TEST(StructureStorage, TestStructureInfo) {
	const auto TEST_STRUCTURE_NAME_VALUE = std::string("Test");
	const auto TEST_NUMBER_OF_FIELD_VALUE = 23;

	__StructureMap structures = NULL;
	__AddStructure(
		&structures,
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_NUMBER_OF_FIELD_VALUE
	);

	const auto structure = __FindStructure(
		&structures,
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	ASSERT_NE(structure, nullptr);
	EXPECT_EQ(structure->number_of_fields, TEST_NUMBER_OF_FIELD_VALUE);
}

TEST(StructureStorage, TestConcatenateNames) {
	const auto TEST_STRUCTURE_NAME_VALUE = std::string("Test");
	const auto TEST_FIELD_NAME_VALUE = std::string("test");

	const char* result = __ConcatenateNames(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_NAME_VALUE.c_str()
	);
	EXPECT_EQ(
		std::string(result),
		TEST_STRUCTURE_NAME_VALUE + "." + TEST_FIELD_NAME_VALUE
	);
}

int main(int number_of_arguments, char* arguments[]) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
