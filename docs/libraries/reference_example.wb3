include "ref"
include "array/utils"
include "io"

function ChangeReference(reference)
	reference.value = 42
end

function Main(_)
	let value = Ref(23)
	Show(Append(ToString(Deref(value), 0), "\n"))

	ChangeReference(value)
	Show(Append(ToString(Deref(value), 0), "\n"))
end
