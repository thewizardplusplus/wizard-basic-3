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

#ifndef DO_STRUCTURE_STORAGE_HEADER
#define DO_STRUCTURE_STORAGE_HEADER

#include "runtime.h"
#include "uthash.h"

/*******************************************************************************
 * StructureInfo type.
 ******************************************************************************/
typedef struct __StructureInfo {
	const char* name;
	size_t number_of_fields;
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
	const size_t number_of_fields
) {
	__StructureInfo* structure = NULL;
	HASH_FIND(hash_handle, *structures, name, strlen(name), structure);
	if (structure == NULL) {
		structure = (__StructureInfo*)malloc(sizeof(__StructureInfo));
		structure->name = name;
		structure->number_of_fields = number_of_fields;

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
		HASH_DELETE(hash_handle, *structures, current_structure);
		free(current_structure);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * FieldInfo type.
 ******************************************************************************/
typedef struct __FieldInfo {
	char* name;
	size_t index;
	UT_hash_handle hash_handle;
} __FieldInfo;

typedef __FieldInfo* __FieldMap;

__FieldInfo* __FindField(__FieldMap* fields, const char* name) {
	__FieldInfo* field = NULL;
	HASH_FIND(hash_handle, *fields, name, strlen(name), field);

	return field;
}

void __AddField(__FieldMap* fields, char* name, const size_t index) {
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
		free(current_field->name);
		free(current_field);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Global variables.
 ******************************************************************************/
static __StructureMap __structures = NULL;
static __FieldMap __fields = NULL;
//------------------------------------------------------------------------------

/*******************************************************************************
 * Utils.
 ******************************************************************************/
char* __ConcatenateNames(const char* structure_name, const char* field_name) {
	const size_t result_length =
		strlen(structure_name)
		+ strlen(field_name)
		// one additional symbol for middle dot and one for \0
		+ 2;
	char* result = (char*)malloc(result_length);

	strcpy(result, structure_name);
	strcat(result, ".");
	strcat(result, field_name);

	return result;
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
void __InitializeStructureStorage() {}

void __RegisterStructureField(
	const char* structure_name,
	const char* field_name,
	const size_t field_index
) {
	char* name = __ConcatenateNames(structure_name, field_name);
	__StructureInfo* structure = __FindStructure(&__structures, structure_name);
	if (structure != NULL) {
		const __FieldInfo* field = __FindField(&__fields, name);
		if (field == NULL) {
			structure->number_of_fields++;
		}
	} else {
		__AddStructure(&__structures, structure_name, 1);
	}

	__AddField(&__fields, name, field_index);
}

size_t __GetStructureFieldsNumber(const char* name) {
	__StructureInfo* structure = __FindStructure(&__structures, name);
	if (structure == NULL) {
		__ProcessError("unknown structure");
	}

	return structure->number_of_fields;
}

size_t __GetStructureFieldIndex(
	const char* structure_name,
	const char* field_name
) {
	char* name = __ConcatenateNames(structure_name, field_name);
	__FieldInfo* field = __FindField(&__fields, name);
	free(name);
	if (field == NULL) {
		__ProcessError("unknown structure or field");
	}

	return field->index;
}

void __CleanupStructureStorage() {
	__DeleteFields(&__fields);
	__DeleteStructures(&__structures);
}
//------------------------------------------------------------------------------
#endif
