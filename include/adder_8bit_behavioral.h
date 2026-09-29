#ifndef ADDER_8BIT_BEHAVIORAL_H
#define ADDER_8BIT_BEHAVIORAL_H

#include <systemc.h>

// ============================================================================
// Modulo: adder_8bit_behavioral (Sumador de 8 Bits - Comportamental)
// Descripcion:
//   Implementacion a nivel RTL / Comportamental usando directamente la
//   capacidad aritmetica del tipo sc_uint. Es mas conciso y simula mas rapido,
//   util para cuando modelemos la ALU completa del computador.
// ============================================================================
SC_MODULE(adder_8bit_behavioral) {
    sc_in<sc_uint<8>> a;
    sc_in<sc_uint<8>> b;
    sc_in<bool>       cin;
    sc_out<sc_uint<8>> sum;
    sc_out<bool>       cout;

    void compute() {
        // Variable de 9 bits para capturar el acarreo en el bit 8
        sc_uint<9> result = a.read() + b.read() + (cin.read() ? 1 : 0);
        sum.write(result.range(7, 0));
        cout.write((bool)result[8]);
    }

    SC_CTOR(adder_8bit_behavioral) {
        SC_METHOD(compute);
        sensitive << a << b << cin;
    }
};

#endif // ADDER_8BIT_BEHAVIORAL_H
