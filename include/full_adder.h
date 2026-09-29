#ifndef FULL_ADDER_H
#define FULL_ADDER_H

#include <systemc.h>

// ============================================================================
// Modulo: full_adder (Sumador Completo de 1 Bit)
// Descripcion:
//   Implementa un sumador completo de 1 bit con acarreo de entrada (cin)
//   y acarreo de salida (cout).
//   Ecuaciones booleanas:
//     sum  = a ^ b ^ cin
//     cout = (a & b) | (cin & (a ^ b))
// ============================================================================
SC_MODULE(full_adder) {
    // Puertos de entrada de 1 bit
    sc_in<bool> a;
    sc_in<bool> b;
    sc_in<bool> cin;

    // Puertos de salida de 1 bit
    sc_out<bool> sum;
    sc_out<bool> cout;

    // Proceso combinacional sensible a las entradas
    void do_add() {
        bool a_val   = a.read();
        bool b_val   = b.read();
        bool cin_val = cin.read();

        // Suma: XOR de las 3 senales
        sum.write(a_val ^ b_val ^ cin_val);

        // Acarreo: Se genera acarreo si ambos a y b son 1, o si cin es 1 y (a ^ b) es 1
        cout.write((a_val & b_val) | (cin_val & (a_val ^ b_val)));
    }

    SC_CTOR(full_adder) {
        SC_METHOD(do_add);
        sensitive << a << b << cin;
    }
};

#endif // FULL_ADDER_H
