#include "structure_storage.h"
#include <cstring>
#include <gtest/gtest.h>

using namespace testing;

const auto TEST_STRUCTURE_NAME_VALUE = std::string("Test");
const auto TEST_FIELD_1_NAME_VALUE = std::string("test_1");
const auto TEST_FIELD_1_INDEX_VALUE = 23;
const auto TEST_FIELD_2_NAME_VALUE = std::string("test_2");
const auto TEST_FIELD_2_INDEX_VALUE = 42;

/*******************************************************************************
 * Interpreter API mocks.
 ******************************************************************************/
void* __AllocateMemory(const size_t size) {
	return malloc(size);
}
//------------------------------------------------------------------------------

TEST(StructureStorage, TestStructureInfo) {
	const auto TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE = 23;

	__StructureMap structures = NULL;
	__AddStructure(
		&structures,
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE
	);

	const auto structure = __FindStructure(
		&structures,
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	ASSERT_NE(structure, nullptr);
	EXPECT_EQ(
		structure->number_of_fields,
		TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE
	);
}

TEST(StructureStorage, TestFieldInfo) {
	__FieldMap fields = NULL;

	const size_t name_length = TEST_FIELD_1_NAME_VALUE.size() + 1;
	char* name = (char*)malloc(name_length);
	std::strcpy(name, TEST_FIELD_1_NAME_VALUE.c_str());

	__AddField(&fields, name, TEST_FIELD_1_INDEX_VALUE);
	const auto field = __FindField(&fields, TEST_FIELD_1_NAME_VALUE.c_str());

	ASSERT_NE(field, nullptr);
	EXPECT_EQ(field->index, TEST_FIELD_1_INDEX_VALUE);
}

TEST(StructureStorage, TestConcatenateNames) {
	const char* result = __ConcatenateNames(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str()
	);
	EXPECT_EQ(
		std::string(result),
		TEST_STRUCTURE_NAME_VALUE + "." + TEST_FIELD_1_NAME_VALUE
	);
}

TEST(StructureStorage, TestRegisterStructureFieldWithOneField) {
	const auto TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE = 1;

	__CleanupStructureStorage();
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str(),
		TEST_FIELD_1_INDEX_VALUE
	);

	const auto structure = __FindStructure(
		&__structures,
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	EXPECT_NE(structure, nullptr);
	if (structure != nullptr) {
		EXPECT_EQ(
			structure->number_of_fields,
			TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE
		);
	}

	const char* name = __ConcatenateNames(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str()
	);
	const auto field = __FindField(&__fields, name);
	ASSERT_NE(field, nullptr);
	EXPECT_EQ(field->index, TEST_FIELD_1_INDEX_VALUE);
}

TEST(StructureStorage, TestRegisterStructureFieldWithTwoField) {
	const auto TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE = 2;

	__CleanupStructureStorage();
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str(),
		TEST_FIELD_1_INDEX_VALUE
	);
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_2_NAME_VALUE.c_str(),
		TEST_FIELD_2_INDEX_VALUE
	);

	const auto structure = __FindStructure(
		&__structures,
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	EXPECT_NE(structure, nullptr);
	if (structure != nullptr) {
		EXPECT_EQ(
			structure->number_of_fields,
			TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE
		);
	}

	const char* name_1 = __ConcatenateNames(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str()
	);
	const auto field_1 = __FindField(&__fields, name_1);
	ASSERT_NE(field_1, nullptr);
	EXPECT_EQ(field_1->index, TEST_FIELD_1_INDEX_VALUE);

	const char* name_2 = __ConcatenateNames(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_2_NAME_VALUE.c_str()
	);
	const auto field_2 = __FindField(&__fields, name_2);
	ASSERT_NE(field_2, nullptr);
	EXPECT_EQ(field_2->index, TEST_FIELD_2_INDEX_VALUE);
}

TEST(StructureStorage, TestGetStructureFieldsNumberWithOneField) {
	const auto TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE = 1;

	__CleanupStructureStorage();
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str(),
		TEST_FIELD_1_INDEX_VALUE
	);

	const auto number_of_fields = __GetStructureFieldsNumber(
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	EXPECT_EQ(number_of_fields, TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE);
}

TEST(StructureStorage, TestGetStructureFieldsNumberWithTwoField) {
	const auto TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE = 2;

	__CleanupStructureStorage();
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str(),
		TEST_FIELD_1_INDEX_VALUE
	);
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_2_NAME_VALUE.c_str(),
		TEST_FIELD_2_INDEX_VALUE
	);

	const auto number_of_fields = __GetStructureFieldsNumber(
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	EXPECT_EQ(number_of_fields, TEST_STRUCTURE_NUMBER_OF_FIELD_VALUE);
}

TEST(StructureStorage, TestGetStructureFieldIndex) {
	__CleanupStructureStorage();
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str(),
		TEST_FIELD_1_INDEX_VALUE
	);
	__RegisterStructureField(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_2_NAME_VALUE.c_str(),
		TEST_FIELD_2_INDEX_VALUE
	);

	const auto index_1 = __GetStructureFieldIndex(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_1_NAME_VALUE.c_str()
	);
	EXPECT_EQ(index_1, TEST_FIELD_1_INDEX_VALUE);

	const auto index_2 = __GetStructureFieldIndex(
		TEST_STRUCTURE_NAME_VALUE.c_str(),
		TEST_FIELD_2_NAME_VALUE.c_str()
	);
	EXPECT_EQ(index_2, TEST_FIELD_2_INDEX_VALUE);
}

int main(int number_of_arguments, char* arguments[]) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
