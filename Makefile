# ==============================================================================
# Makefile para compilar el sumador de 8 bits en SystemC
# ==============================================================================
CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Ruta base de SystemC (puedes sobrescribirla al invocar: make SYSTEMC_HOME=/ruta/a/systemc)
SYSTEMC_HOME ?= /usr/local/systemc

# Deteccion de arquitectura / carpetas de librerias de SystemC
SYSTEMC_INC = $(SYSTEMC_HOME)/include

# Seleccionar carpeta lib segun SO / compilador si existe
ifeq ($(OS),Windows_NT)
    SYSTEMC_LIB ?= $(SYSTEMC_HOME)/lib-mingw64
    TARGET = adder_8bit_sim.exe
else
    SYSTEMC_LIB ?= $(SYSTEMC_HOME)/lib-linux64
    TARGET = adder_8bit_sim
endif

CXXFLAGS += -I$(SYSTEMC_INC)
LDFLAGS += -L$(SYSTEMC_LIB) -lsystemc -lpthread

SRCS = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRCS) include/*.h
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) *.vcd *.o

.PHONY: all run clean
