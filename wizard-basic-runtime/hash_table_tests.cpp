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
	FieldMap fields = NULL;
	__AddField(&fields, "test", 23);
	const auto size = __GetNumberOfFields(&fields);
	EXPECT_EQ(size, 1);

	const auto result = __FindField(&fields, "test");
	EXPECT_EQ(result, 23);

	__DeleteFields(&fields);
}

int main(int number_of_arguments, char* arguments[]) {
	InitGoogleTest(&number_of_arguments, arguments);
	return RUN_ALL_TESTS();
}
