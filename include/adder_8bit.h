#ifndef ADDER_8BIT_H
#define ADDER_8BIT_H

#include <systemc.h>
#include "full_adder.h"

// ============================================================================
// Modulo: adder_8bit (Sumador de 8 Bits - Ripple Carry Adder)
// Descripcion:
//   Suma dos palabras de 8 bits (a y b) mas un bit de acarreo de entrada (cin).
//   Genera el resultado de 8 bits (sum) y el acarreo final de salida (cout).
//   Estructuralmente interconecta 8 sumadores completos de 1 bit (full_adder)
//   encadenando la senal de acarreo (carry).
// ============================================================================
SC_MODULE(adder_8bit) {
    // Puertos externos del sumador
    sc_in<sc_uint<8>> a;    // Operando A (8 bits)
    sc_in<sc_uint<8>> b;    // Operando B (8 bits)
    sc_in<bool>       cin;  // Acarreo inicial (1 bit)
    sc_out<sc_uint<8>> sum; // Resultado de la suma (8 bits)
    sc_out<bool>       cout;// Acarreo de desbordamiento (1 bit)

    // Senales internas para conectar bit a bit los submódulos
    sc_signal<bool> a_bits[8];
    sc_signal<bool> b_bits[8];
    sc_signal<bool> sum_bits[8];
    sc_signal<bool> carry[9]; // carry[0] es cin, carry[8] es cout

    // Arreglo de apuntadores a las 8 instancias de full_adder
    full_adder* fa[8];

    // Proceso combinacional: desempaqueta los buses de 8 bits en senales individuales
    void unpack_inputs() {
        sc_uint<8> val_a = a.read();
        sc_uint<8> val_b = b.read();

        for (int i = 0; i < 8; ++i) {
            a_bits[i].write((bool)val_a[i]);
            b_bits[i].write((bool)val_b[i]);
        }
        carry[0].write(cin.read());
    }

    // Proceso combinacional: empaqueta los bits individuales en el bus de salida sum
    void pack_outputs() {
        sc_uint<8> val_sum = 0;
        for (int i = 0; i < 8; ++i) {
            if (sum_bits[i].read()) {
                val_sum |= (1 << i);
            }
        }
        sum.write(val_sum);
        cout.write(carry[8].read());
    }

    SC_CTOR(adder_8bit) {
        // Desempaquetador sensible a cambios en los puertos de entrada
        SC_METHOD(unpack_inputs);
        sensitive << a << b << cin;

        // Empaquetador sensible a cambios en las salidas de los sumadores individuales
        SC_METHOD(pack_outputs);
        for (int i = 0; i < 8; ++i) {
            sensitive << sum_bits[i];
        }
        sensitive << carry[8];

        // Instanciacion y encadenamiento en cascada (Ripple-Carry) de los 8 full_adder
        for (int i = 0; i < 8; ++i) {
            char mod_name[32];
            std::snprintf(mod_name, sizeof(mod_name), "fa_bit_%d", i);
            fa[i] = new full_adder(mod_name);

            // Conexion de puertos
            fa[i]->a(a_bits[i]);
            fa[i]->b(b_bits[i]);
            fa[i]->cin(carry[i]);       // Recibe acarreo del bit anterior
            fa[i]->sum(sum_bits[i]);    // Escribe el bit resultante
            fa[i]->cout(carry[i + 1]);  // Pasa el acarreo al siguiente bit
        }
    }

    // Destructor para liberar memoria de los submódulos instanciados dinamicamente
    ~adder_8bit() {
        for (int i = 0; i < 8; ++i) {
            delete fa[i];
        }
    }
};

#endif // ADDER_8BIT_H
