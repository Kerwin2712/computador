# ==============================================================================
# Makefile para compilar los modulos de SystemC
# ==============================================================================
CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Ruta base de SystemC (puedes sobrescribirla al invocar: make SYSTEMC_HOME=/ruta/a/systemc)
SYSTEMC_HOME ?= /usr/local/systemc

# Deteccion de arquitectura / carpetas de librerias de SystemC
SYSTEMC_INC = $(SYSTEMC_HOME)/include

ifeq ($(OS),Windows_NT)
    SYSTEMC_LIB ?= $(SYSTEMC_HOME)/lib-mingw64
    EXE_EXT = .exe
else
    SYSTEMC_LIB ?= $(SYSTEMC_HOME)/lib-linux64
    EXE_EXT =
endif

CXXFLAGS += -I$(SYSTEMC_INC)
LDFLAGS += -L$(SYSTEMC_LIB) -lsystemc -lpthread

TARGET_ADDER   = adder_8bit_sim$(EXE_EXT)
TARGET_ADD_SUB = add_sub_8bit_sim$(EXE_EXT)

all: $(TARGET_ADDER) $(TARGET_ADD_SUB)

$(TARGET_ADDER): src/main.cpp include/*.h
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(TARGET_ADDER) $(LDFLAGS)

$(TARGET_ADD_SUB): src/main_add_sub.cpp include/*.h
	$(CXX) $(CXXFLAGS) src/main_add_sub.cpp -o $(TARGET_ADD_SUB) $(LDFLAGS)

run_adder: $(TARGET_ADDER)
	./$(TARGET_ADDER)

run_add_sub: $(TARGET_ADD_SUB)
	./$(TARGET_ADD_SUB)

clean:
	rm -f $(TARGET_ADDER) $(TARGET_ADD_SUB) *.vcd *.o

.PHONY: all run_adder run_add_sub clean
