BUILD = build/libraries/

.PHONY: clean

main:
	$(MAKE) -C wizard-basic-3-compiler
	$(MAKE) -C wizard-basic-3-runtime

	mkdir -p $(BUILD)
	cp -rv libraries/* $(BUILD)

clean:
	$(MAKE) -C wizard-basic-3-compiler clean
	$(MAKE) -C wizard-basic-3-runtime clean
