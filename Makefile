.PHONY: clean

main:
	$(MAKE) -C wizard-basic-interpreter
	$(MAKE) -C wizard-basic-runtime

clean:
	$(MAKE) -C wizard-basic-interpreter clean
	$(MAKE) -C wizard-basic-runtime clean
