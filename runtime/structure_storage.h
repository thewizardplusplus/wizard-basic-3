/* The MIT License (MIT)
 *
 * Copyright (c) 2015 thewizardplusplus <thewizardplusplus@yandex.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef WIZARD_BASIC_3_STRUCTURE_STORAGE_HEADER
#define WIZARD_BASIC_3_STRUCTURE_STORAGE_HEADER

#include "runtime.h"
#include "uthash.h"

/*******************************************************************************
 * FieldInfo type.
 ******************************************************************************/
typedef struct __FieldInfo {
	const char* name;
	size_t index;
	UT_hash_handle hash_handle;
} __FieldInfo;

typedef __FieldInfo* __FieldMap;

size_t __GetNumberOfFields(__FieldMap* fields) {
	return HASH_CNT(hash_handle, *fields);
}

__FieldInfo* __FindField(__FieldMap* fields, const char* name) {
	__FieldInfo* field = NULL;
	HASH_FIND(hash_handle, *fields, name, strlen(name), field);

	return field;
}

void __AddField(__FieldMap* fields, const char* name, const size_t index) {
	__FieldInfo* field = NULL;
	HASH_FIND(hash_handle, *fields, name, strlen(name), field);
	if (field == NULL) {
		field = (__FieldInfo*)malloc(sizeof(__FieldInfo));
		field->name = name;
		field->index = index;

		HASH_ADD_KEYPTR(
			hash_handle,
			*fields,
			field->name,
			strlen(field->name),
			field
		);
	}
}

void __DeleteFields(__FieldMap* fields) {
	__FieldInfo* current_field = NULL;
	__FieldInfo* temporary_field = NULL;
	HASH_ITER(hash_handle, *fields, current_field, temporary_field) {
		HASH_DELETE(hash_handle, *fields, current_field);
		free(current_field);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * StructureInfo type.
 ******************************************************************************/
typedef struct __StructureInfo {
	const char* name;
	__FieldMap fields;
	UT_hash_handle hash_handle;
} __StructureInfo;

typedef __StructureInfo* __StructureMap;

__StructureInfo* __FindStructure(__StructureMap* structures, const char* name) {
	__StructureInfo* structure = NULL;
	HASH_FIND(hash_handle, *structures, name, strlen(name), structure);

	return structure;
}

void __AddStructure(
	__StructureMap* structures,
	const char* name,
	__FieldMap fields
) {
	__StructureInfo* structure = NULL;
	HASH_FIND(hash_handle, *structures, name, strlen(name), structure);
	if (structure == NULL) {
		structure = (__StructureInfo*)malloc(sizeof(__StructureInfo));
		structure->name = name;
		structure->fields = fields;

		HASH_ADD_KEYPTR(
			hash_handle,
			*structures,
			structure->name,
			strlen(structure->name),
			structure
		);
	}
}

void __DeleteStructures(__StructureMap* structures) {
	__StructureInfo* current_structure = NULL;
	__StructureInfo* temporary_structure = NULL;
	HASH_ITER(
		hash_handle,
		*structures,
		current_structure,
		temporary_structure
	) {
		if (current_structure != NULL) {
			__DeleteFields(&current_structure->fields);
		}

		HASH_DELETE(hash_handle, *structures, current_structure);
		free(current_structure);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Global variables.
 ******************************************************************************/
static __StructureMap __structures = NULL;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils.
 ******************************************************************************/
__StructureInfo* __GetStructure(__StructureMap* structures, const char* name) {
	__StructureInfo* structure = __FindStructure(structures, name);
	if (structure == NULL) {
		__ProcessError("unknown structure");
	}

	return structure;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
void __InitializeStructureStorage(void) {}

void __RegisterStructureField(
	const char* structure_name,
	const char* field_name,
	const size_t field_index
) {
	__StructureInfo* structure = __FindStructure(&__structures, structure_name);
	if (structure == NULL) {
		__AddStructure(&__structures, structure_name, NULL);
	}

	__AddField(&structure->fields, field_name, field_index);
}

size_t __GetStructureFieldsNumber(const char* name) {
	__StructureInfo* structure = __GetStructure(&__structures, name);
	return __GetNumberOfFields(&structure->fields);
}

size_t __GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
) {
	__StructureInfo* structure = __GetStructure(&__structures, structure_name);
	__FieldInfo* field = __FindField(&structure->fields, field_name);
	if (field == NULL) {
		__ProcessError("unknown structure field");
	}

	return field->index;
}

void __CleanupStructureStorage(void) {
	__DeleteStructures(&__structures);
}
//------------------------------------------------------------------------------
#endif
