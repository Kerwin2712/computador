#ifndef ADD_SUB_8BIT_H
#define ADD_SUB_8BIT_H

#include <systemc.h>
#include "adder_8bit.h"

// ============================================================================
// Modulo: add_sub_8bit (Sumador / Restador de 8 Bits con Banderas de Estado)
// Descripcion:
//   Implementa una unidad sumadora/restadora configurable mediante la senal 'sub':
//     - sub = 0: SUMA  -> Result = A + B
//     - sub = 1: RESTA -> Result = A - B (Complemento a 2: A + ~B + 1)
//
//   Utiliza internamente el modulo 'adder_8bit' y genera las banderas de CPU:
//     - overflow (V): Desbordamiento en aritmetica con signo (Two's Complement).
//     - carry    (C): Acarreo saliente del bit 7 (Cout del sumador).
//     - zero     (Z): Activo si el resultado es igual a cero.
//     - negative (N): Bit de signo del resultado (MSB / bit 7).
// ============================================================================
SC_MODULE(add_sub_8bit) {
    // Puertos de entrada
    sc_in<sc_uint<8>> a;    // Operando A (8 bits)
    sc_in<sc_uint<8>> b;    // Operando B (8 bits)
    sc_in<bool>       sub;  // Control: 0 = Sumar, 1 = Restar

    // Puertos de salida
    sc_out<sc_uint<8>> result;   // Resultado de la operacion (8 bits)
    sc_out<bool>       overflow; // Bandera V (Desbordamiento con signo)
    sc_out<bool>       carry;    // Bandera C (Acarreo / Cout del sumador)
    sc_out<bool>       zero;     // Bandera Z (Cero)
    sc_out<bool>       negative; // Bandera N (Negativo / Signo)

    // Senales internas de conexion hacia el sumador de 8 bits
    sc_signal<sc_uint<8>> b_eff;        // Operando B condicionado (B ^ sub)
    sc_signal<bool>       cin_internal; // Cin para el sumador (igual a sub)
    sc_signal<sc_uint<8>> sum_internal; // Salida Sum del sumador
    sc_signal<bool>       cout_internal;// Salida Cout del sumador

    // Instancia del sumador estructural de 8 bits
    adder_8bit* adder_inst;

    // Proceso combinacional: acondiciona el operando B aplicando compuertas XOR
    // y fija el Cin en 1 si es resta (para completar el complemento a 2).
    void condition_operands() {
        sc_uint<8> b_val = b.read();
        bool is_sub = sub.read();

        sc_uint<8> b_xor = 0;
        for (int i = 0; i < 8; ++i) {
            b_xor[i] = (bool)b_val[i] ^ is_sub;
        }

        b_eff.write(b_xor);
        cin_internal.write(is_sub);
    }

    // Proceso combinacional: calcula el resultado final y las banderas de estado (Z, N, C, V)
    void compute_flags() {
        sc_uint<8> res = sum_internal.read();
        bool cout_val  = cout_internal.read();

        // 1. Asignar resultado
        result.write(res);

        // 2. Bandera Zero (Z): 1 si todos los bits son 0
        zero.write(res == 0);

        // 3. Bandera Negative (N): bit 7 del resultado
        bool res_sign = (bool)res[7];
        negative.write(res_sign);

        // 4. Bandera Carry (C): acarreo del sumador
        carry.write(cout_val);

        // 5. Bandera Overflow (V): Desbordamiento en complemento a 2
        // Ocurre si sumamos dos operandos con el mismo signo efectivo y el resultado
        // tiene un signo diferente: V = (A7 == B'_eff7) and (Res7 != A7)
        bool a_sign     = (bool)a.read()[7];
        bool b_eff_sign = (bool)b_eff.read()[7];

        bool v_flag = (a_sign == b_eff_sign) && (res_sign != a_sign);
        overflow.write(v_flag);
    }

    SC_CTOR(add_sub_8bit) {
        // Instanciar el sumador estructural
        adder_inst = new adder_8bit("internal_adder_8bit");

        // Conectar puertos del sumador
        adder_inst->a(a);
        adder_inst->b(b_eff);
        adder_inst->cin(cin_internal);
        adder_inst->sum(sum_internal);
        adder_inst->cout(cout_internal);

        // Procesos combinacionales
        SC_METHOD(condition_operands);
        sensitive << b << sub;

        SC_METHOD(compute_flags);
        sensitive << sum_internal << cout_internal << a << b_eff;
    }

    ~add_sub_8bit() {
        delete adder_inst;
    }
};

#endif // ADD_SUB_8BIT_H
