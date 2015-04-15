BUILD = build/libraries/

.PHONY: clean

main:
	$(MAKE) -C wizard-basic-interpreter
	$(MAKE) -C wizard-basic-runtime

	mkdir -p $(BUILD)
	cp -rv libraries/* $(BUILD)

clean:
	$(MAKE) -C wizard-basic-interpreter clean
	$(MAKE) -C wizard-basic-runtime clean
