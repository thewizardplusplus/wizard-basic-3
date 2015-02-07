#ifndef WIZARD_BASIC_3_STRUCTURE_STORAGE_HEADER
#define WIZARD_BASIC_3_STRUCTURE_STORAGE_HEADER

#include "runtime.h"
#include "uthash.h"

/*******************************************************************************
 * FieldInfo type.
 ******************************************************************************/
typedef struct FieldInfo {
	const char* name;
	size_t index;
	UT_hash_handle hash_handle;
} FieldInfo;

typedef FieldInfo* FieldMap;

size_t __GetNumberOfFields(FieldMap* fields) {
	return HASH_CNT(hash_handle, *fields);
}

FieldInfo* __FindField(FieldMap* fields, const char* name) {
	FieldInfo* field = NULL;
	HASH_FIND(hash_handle, *fields, name, strlen(name), field);

	return field;
}

void __AddField(FieldMap* fields, const char* name, const size_t index) {
	FieldInfo* field = (FieldInfo*)malloc(sizeof(FieldInfo));
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

void __DeleteFields(FieldMap* fields) {
	FieldInfo* current_field = NULL;
	FieldInfo* temporary_field = NULL;
	HASH_ITER(hash_handle, *fields, current_field, temporary_field) {
		HASH_DELETE(hash_handle, *fields, current_field);
		free(current_field);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * StructureInfo type.
 ******************************************************************************/
typedef struct StructureInfo {
	const char* name;
	FieldMap* fields;
	UT_hash_handle hash_handle;
} StructureInfo;

typedef StructureInfo* StructureMap;

StructureInfo* __FindStructure(StructureMap* structures, const char* name) {
	StructureInfo* structure = NULL;
	HASH_FIND(hash_handle, *structures, name, strlen(name), structure);

	return structure;
}

void __AddStructure(
	StructureMap* structures,
	const char* name,
	FieldMap* fields
) {
	StructureInfo* structure = (StructureInfo*)malloc(sizeof(StructureInfo));
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

void __DeleteFields(StructureMap* structures) {
	StructureInfo* current_structure = NULL;
	StructureInfo* temporary_structure = NULL;
	HASH_ITER(
		hash_handle,
		*structures,
		current_structure,
		temporary_structure
	) {
		if (current_structure != NULL && current_structure->fields != NULL) {
			__DeleteFields(current_structure->fields);
		}

		HASH_DELETE(hash_handle, *structures, current_structure);
		free(current_structure);
	}
}
//------------------------------------------------------------------------------

/*******************************************************************************
 * Interpreter API.
 ******************************************************************************/
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
#endif
