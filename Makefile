SOURCE = src
OUTPUT = output

MAIN = $(SOURCE)/main.cpp
MAIN_OUTPUT = $(OUTPUT)/main.o

HELLO = $(SOURCE)/hello.cpp
HELLO_OUTPUT = $(OUTPUT)/hello.o

RESULT = hello
RESULT_OUTPUT = $(OUTPUT)/$(RESULT)

$(RESULT): output hello_output main_output
	$(CXX) $(HELLO_OUTPUT) $(MAIN_OUTPUT) -o $(RESULT_OUTPUT)

output:
	mkdir -p $(OUTPUT)

hello_output: $(HELLO)
	$(CXX) -c $(HELLO) -o $(HELLO_OUTPUT)

main_output: $(MAIN)
	$(CXX) -c $(MAIN) -o $(MAIN_OUTPUT)

clean:
	rm -r $(OUTPUT)
