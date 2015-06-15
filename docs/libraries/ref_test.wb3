include "ref"
include "io"

let OLD_VAL = 23
let NEW_VAL = 42

function Change(val)
	val = NEW_VAL
end

function ChangeWithRef(ref)
	ref.value = NEW_VAL
end

function ShowVal(old_val, new_val)
	new_val = Deref(new_val)

	let msg = ToString(old_val, 0)
	msg = Append(msg, " => ")
	msg = Append(msg, ToString(new_val, 0))
	msg = Append(msg, "\n")

	Show(msg)
end

function Main(_)
	let num_1 = OLD_VAL
	Change(num_1)
	ShowVal(OLD_VAL, num_1)

	let num_2 = Ref(OLD_VAL)
	ChangeWithRef(num_2)
	ShowVal(OLD_VAL, num_2)
end
