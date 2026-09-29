#ifndef ACCUMULATOR_H
#define ACCUMULATOR_H

#include <systemc.h>
#include "adder_8bit.h"

// ============================================================================
// Modulo: accumulator (Registro Acumulador de 8 Bits con Sumador Integrado)
// Descripcion:
//   Demuestra el uso del sumador de 8 bits (adder_8bit) dentro de una estructura
//   secuencial sincrona clásica de CPU (Datapath de Acumulador).
//   En cada flanco de subida de reloj (posedge clk):
//     - Si rst == 1  -> ACC = 0
//     - Si load == 1 -> ACC = ACC + data_in
// ============================================================================
SC_MODULE(accumulator) {
    // Puertos externos
    sc_in<bool>       clk;      // Senal de reloj
    sc_in<bool>       rst;      // Reset sincronico activo en alto
    sc_in<bool>       load;     // Habilitacion de carga / suma
    sc_in<sc_uint<8>> data_in;  // Operando a acumular

    sc_out<sc_uint<8>> acc_out; // Salida con el valor acumulado actual
    sc_out<bool>       cout;    // Acarreo de la ultima suma

    // Senales internas
    sc_signal<sc_uint<8>> reg_val;      // Registro interno que almacena el valor del acumulador
    sc_signal<bool>       cin_zero;     // Cin fijo en 0 para suma simple
    sc_signal<sc_uint<8>> next_sum;     // Salida del sumador conectada de regreso al registro
    sc_signal<bool>       sum_cout;     // Acarreo generado por el sumador

    // Instancia del sumador estructural de 8 bits
    adder_8bit* adder_inst;

    // Proceso secuencial sensible al flanco positivo del reloj
    void clock_process() {
        if (rst.read()) {
            reg_val.write(0);
            acc_out.write(0);
            cout.write(false);
        } else if (load.read()) {
            // Cargar el resultado de la suma en el acumulador
            reg_val.write(next_sum.read());
            acc_out.write(next_sum.read());
            cout.write(sum_cout.read());
        }
    }

    SC_CTOR(accumulator) {
        // Fijar cin interno siempre en 0
        cin_zero.write(false);

        // Instanciar el sumador de 8 bits
        adder_inst = new adder_8bit("acc_adder");

        // Entrada A: el valor actual del registro acumulador
        adder_inst->a(reg_val);
        // Entrada B: el nuevo dato a sumar
        adder_inst->b(data_in);
        // Cin: 0
        adder_inst->cin(cin_zero);
        // Salida Sum: conectada al proximo valor a guardar
        adder_inst->sum(next_sum);
        // Salida Cout: acarreo del sumador
        adder_inst->cout(sum_cout);

        // Proceso secuencial en flanco positivo del reloj
        SC_METHOD(clock_process);
        sensitive << clk.pos();
    }

    ~accumulator() {
        delete adder_inst;
    }
};

#endif // ACCUMULATOR_H
