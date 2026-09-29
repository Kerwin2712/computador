# 🖥️ Simulador de Computador en SystemC (IEEE 1666)

Proyecto para el diseño y simulación modular de la arquitectura de un computador utilizando la librería **SystemC** en C++.

---

## 📌 Paso 1: Sumador de 8 Bits (*Ripple-Carry Adder*)

El primer bloque fundamental de la Unidad Aritmético-Lógica (**ALU**) de nuestro computador es el **sumador de 8 bits**.

### 📐 Arquitectura del Sumador

```
                    Entradas: A[7:0], B[7:0], Cin
                                  │
      ┌───────────────────────────┴───────────────────────────┐
      │                                                       │
  Bit 7               Bit 2               Bit 1               Bit 0
┌──────────────┐    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐
│  Full Adder  │◄───│  Full Adder  │◄───│  Full Adder  │◄───│  Full Adder  │◄── Cin
│    (fa_7)    │ C7 │    (fa_2)    │ C2 │    (fa_1)    │ C1 │    (fa_0)    │
└──────┬───────┘    └──────┬───────┘    └──────┬───────┘    └──────┬───────┘
       │ Cout              │ S[2]              │ S[1]              │ S[0]
       │                   └─────────────┬─────┴───────────────────┘
       ▼                                 ▼
   Cout (Carry)                     Sum[7:0]
```

1. **Sumador Completo de 1 Bit (`full_adder`)**:
   - Entradas: `a`, `b`, `cin`
   - Salidas: `sum`, `cout`
   - Ecuaciones lógicas:
     $$\text{sum} = a \oplus b \oplus c_{in}$$
     $$c_{out} = (a \cdot b) + (c_{in} \cdot (a \oplus b))$$

2. **Sumador de 8 Bits (`adder_8bit`)**:
   - Conecta **8 sumadores completos** en cascada pasando el acarreo de cada etapa a la siguiente ($C_0 = C_{in}$, $C_1$, ..., $C_8 = C_{out}$).
   - Entradas: `a` (8 bits: `sc_uint<8>`), `b` (8 bits: `sc_uint<8>`), `cin` (1 bit: `bool`).
   - Salidas: `sum` (8 bits: `sc_uint<8>`), `cout` (1 bit: `bool`).

---

## 📂 Estructura del Proyecto

```
computador/
├── include/
│   ├── full_adder.h              # Sumador completo de 1 bit (compuertas lógicas)
│   ├── adder_8bit.h              # Sumador estructural de 8 bits (Ripple Carry)
│   ├── adder_8bit_behavioral.h   # Alternativa comportamental con sc_uint<9>
│   └── tb_adder_8bit.h           # Testbench auto-verificable con casos límite
├── src/
│   └── main.cpp                  # sc_main(): instanciación, enlaces y trazas VCD
├── CMakeLists.txt                # Configuración de compilación con CMake
├── Makefile                      # Configuración de compilación con Make/g++
└── README.md                     # Documentación y hoja de ruta
```

---

## 🚀 Cómo Compilar y Ejecutar

### Opción A: Usando CMake (Recomendada)
Si tienes CMake y un compilador C++ (GCC/MinGW, Clang o MSVC):
```bash
mkdir build
cd build
cmake ..
cmake --build .
./adder_8bit_sim
```
> **Nota:** Si no tienes SystemC instalado previamente, `CMakeLists.txt` descargará automáticamente la versión oficial de Accellera SystemC 2.3.4 mediante `FetchContent`.

### Opción B: Usando g++ / Makefile
Si tienes SystemC instalado en tu sistema:
```bash
make SYSTEMC_HOME=/ruta/a/systemc
./adder_8bit_sim
```

---

## 📊 Visualización de Ondas (GTKWave)

Al ejecutar la simulación, se genera automáticamente el archivo `adder_8bit_waves.vcd`. Puedes visualizar las señales de tiempo y propagación con:
- [GTKWave](http://gtkwave.sourceforge.net/)
- Visores web en el navegador como [WaveDrom](https://wavedrom.com/) o [vc.drom.io](https://vc.drom.io/)

---

## 🗺️ Hoja de Ruta para Construir el Computador Completo

- [x] **Paso 1:** Sumador de 8 bits (Ripple-Carry Adder)
- [ ] **Paso 2:** Sumador / Restador de 8 bits (con lógica de complemento a 2)
- [ ] **Paso 3:** ALU (Unidad Aritmético Lógica) de 8 bits (ADD, SUB, AND, OR, XOR, NOT, SHL, SHR) + Banderas (Z, C, N, V)
- [ ] **Paso 4:** Banco de Registros (Register File: R0, R1, R2, R3 / Acumulador)
- [ ] **Paso 5:** Contador de Programa (PC) y Registro de Instrucciones (IR)
- [ ] **Paso 6:** Memoria de Instrucciones (ROM) y Memoria de Datos (RAM)
- [ ] **Paso 7:** Unidad de Control (FSM de estados Fetch, Decode, Execute)
- [ ] **Paso 8:** Integración final del Datapath y ejecución de programas de prueba en ensamblador
