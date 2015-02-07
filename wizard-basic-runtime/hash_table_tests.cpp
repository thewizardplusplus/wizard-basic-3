#include "structure_storage.h"
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

	FieldMap fields = NULL;
	__AddField(&fields, TEST_FIELD_NAME_VALUE.c_str(), TEST_FIELD_INDEX_VALUE);
	const auto size = __GetNumberOfFields(&fields);
	EXPECT_EQ(size, 1);

	const auto field = __FindField(&fields, TEST_FIELD_NAME_VALUE.c_str());
	ASSERT_NE(field, nullptr);
	EXPECT_EQ(field->index, TEST_FIELD_INDEX_VALUE);
}

TEST(StructureStorage, TestStructureInfo) {
	const auto TEST_STRUCTURE_NAME_VALUE = std::string("Test");

	StructureMap structures = NULL;
	FieldMap fields = NULL;
	__AddStructure(&structures, TEST_STRUCTURE_NAME_VALUE.c_str(), &fields);

	const auto structure = __FindStructure(
		&structures,
		TEST_STRUCTURE_NAME_VALUE.c_str()
	);
	ASSERT_NE(structure, nullptr);
	EXPECT_EQ(structure->fields, &fields);
}

int main(int number_of_arguments, char* arguments[]) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
