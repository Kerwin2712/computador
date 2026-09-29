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

1. **Sumador Completo de 1 Bit (`full_adder.h`)**:
   - Entradas: `a`, `b`, `cin`
   - Salidas: `sum`, `cout`
   - Ecuaciones lógicas:
     $$\text{sum} = a \oplus b \oplus c_{in}$$
     $$c_{out} = (a \cdot b) + (c_{in} \cdot (a \oplus b))$$

2. **Sumador de 8 Bits (`adder_8bit.h`)**:
   - Conecta **8 sumadores completos** en cascada pasando el acarreo de cada etapa a la siguiente ($C_0 = C_{in}$, $C_1$, ..., $C_8 = C_{out}$).
   - Entradas: `a` (`sc_uint<8>`), `b` (`sc_uint<8>`), `cin` (`bool`).
   - Salidas: `sum` (`sc_uint<8>`), `cout` (`bool`).

---

## 📌 Paso 2: Sumador / Restador de 8 Bits con Banderas (*Add/Sub 8-bit*)

A partir del sumador anterior, se construye la unidad sumadora/restadora configurable mediante la señal de control **`sub`**:

```
                       Control SUB (0 = ADD, 1 = SUB)
                             │          │
                 ┌───────────┘          └───────────────┐
                 │                                      ▼ (Cin = SUB)
                 ▼                                ┌───────────┐
   B[7:0] ──► [ XOR ] ──► B_eff[7:0] ────────────►│           │
                                                  │ adder_8bit│──► Result[7:0]
   A[7:0] ───────────────────────────────────────►│           │──► Cout (Carry)
                                                  └───────────┘
                                                        │
                                                        ▼
                                                Lógica de Banderas
                                              ┌────────────────────┐
                                              │ Z (Zero)           │
                                              │ N (Negative)       │
                                              │ C (Carry / Cout)   │
                                              │ V (Overflow Signo) │
                                              └────────────────────┘
```

### Principio de Complemento a 2
- **Suma (`sub = 0`)**: $B_{eff} = B \oplus 0 = B$, $C_{in} = 0 \implies \text{Result} = A + B$.
- **Resta (`sub = 1`)**: $B_{eff} = B \oplus 1 = \sim B$, $C_{in} = 1 \implies \text{Result} = A + \sim B + 1 = A - B$.

### Detección de Desbordamiento y Banderas de CPU
1. **$V$ (Overflow con Signo)**: Detecta si el resultado excede el rango $[-128, +127]$ en complemento a 2:
   $$V = (A_7 == B_{eff,7}) \land (Result_7 \neq A_7)$$
2. **$C$ (Carry / Cout)**: Acarreo saliente del bit más significativo.
3. **$Z$ (Zero)**: Se activa si $\text{Result} == 0$.
4. **$N$ (Negative)**: Bit más significativo del resultado ($Result_7$).

---

## 📌 Módulo Acumulador (`accumulator.h`)
Demuestra el uso del sumador en un **Datapath secuencial sincrónico**:
- Registro acumulador (`acc_reg`) de 8 bits con reloj `clk`, `rst` y `load`.
- Conectado en bucle con `adder_8bit` para realizar sumas acumulativas:
  $$\text{ACC} \leftarrow \text{ACC} + \text{DataIn}$$

---

## 📂 Estructura del Proyecto

```
computador/
├── include/
│   ├── full_adder.h              # Sumador completo de 1 bit (compuertas lógicas)
│   ├── adder_8bit.h              # Sumador estructural de 8 bits (Ripple Carry)
│   ├── adder_8bit_behavioral.h   # Alternativa comportamental con sc_uint<9>
│   ├── add_sub_8bit.h            # Sumador/Restador de 8 bits con banderas (Z, N, C, V)
│   ├── accumulator.h             # Módulo secuencial con registro acumulador + sumador
│   ├── tb_adder_8bit.h           # Testbench del sumador de 8 bits
│   └── tb_add_sub_8bit.h         # Testbench exhaustivo del sumador/restador
├── src/
│   ├── main.cpp                  # Simulación del sumador de 8 bits
│   └── main_add_sub.cpp          # Simulación del sumador/restador de 8 bits
├── CMakeLists.txt                # Configuración de compilación con CMake
├── Makefile                      # Configuración de compilación con Make/g++
└── README.md                     # Documentación y hoja de ruta
```

---

## 🚀 Cómo Compilar y Ejecutar

### Usando CMake (Recomendado)
```powershell
cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
```

### Ejecutables de Simulación Generados:
1. **Sumador de 8 Bits:**
   ```powershell
   .\adder_8bit_sim.exe
   ```
2. **Sumador / Restador con Banderas (Z, N, C, V):**
   ```powershell
   .\add_sub_8bit_sim.exe
   ```

---

## 📊 Visualización de Ondas (GTKWave)

- `adder_8bit_waves.vcd`: Ondas digitales de la simulación del sumador.
- `add_sub_8bit_waves.vcd`: Ondas del sumador/restador con control `SUB` y banderas de estado.
- Visualizable en [vc.drom.io](https://vc.drom.io/) o [GTKWave](http://gtkwave.sourceforge.net/).

---

## 🗺️ Hoja de Ruta para Construir el Computador Completo

- [x] **Paso 1:** Sumador de 8 bits (Ripple-Carry Adder)
- [x] **Paso 2:** Sumador / Restador de 8 bits (Complemento a 2 con control ADD/SUB y banderas Z, N, C, V)
- [ ] **Paso 3:** ALU (Unidad Aritmético Lógica) de 8 bits (ADD, SUB, AND, OR, XOR, NOT, SHL, SHR)
- [ ] **Paso 4:** Banco de Registros (Register File: R0, R1, R2, R3 / Acumulador)
- [ ] **Paso 5:** Contador de Programa (PC) y Registro de Instrucciones (IR)
- [ ] **Paso 6:** Memoria de Instrucciones (ROM) y Memoria de Datos (RAM)
- [ ] **Paso 7:** Unidad de Control (FSM de estados Fetch, Decode, Execute)
- [ ] **Paso 8:** Integración final del Datapath y ejecución de programas de prueba en ensamblador
