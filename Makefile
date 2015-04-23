BUILD = build/libraries/

.PHONY: clean

main:
	$(MAKE) -C do-interpreter
	$(MAKE) -C do-runtime

	mkdir -p $(BUILD)
	cp -rv libraries/* $(BUILD)

clean:
	$(MAKE) -C do-interpreter clean
	$(MAKE) -C do-runtime clean
