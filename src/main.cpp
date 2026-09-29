#include <systemc.h>
#include "adder_8bit.h"
#include "tb_adder_8bit.h"

int sc_main(int argc, char* argv[]) {
    // 1. Declarar senales de interconexion entre DUT y Testbench
    sc_signal<sc_uint<8>> sig_a("sig_a");
    sc_signal<sc_uint<8>> sig_b("sig_b");
    sc_signal<bool>       sig_cin("sig_cin");
    sc_signal<sc_uint<8>> sig_sum("sig_sum");
    sc_signal<bool>       sig_cout("sig_cout");

    // 2. Instanciar el Dispositivo Bajo Prueba (DUT: Device Under Test)
    // Usamos el sumador estructural de 8 bits compuesto por 8 full adders
    adder_8bit uut("uut_adder_8bit");
    uut.a(sig_a);
    uut.b(sig_b);
    uut.cin(sig_cin);
    uut.sum(sig_sum);
    uut.cout(sig_cout);

    // 3. Instanciar el Banco de Pruebas (Testbench)
    tb_adder_8bit testbench("testbench_adder_8bit");
    testbench.a(sig_a);
    testbench.b(sig_b);
    testbench.cin(sig_cin);
    testbench.sum(sig_sum);
    testbench.cout(sig_cout);

    // 4. Crear archivo de trazado de formas de onda (VCD - Value Change Dump)
    // Permite inspeccionar graficamente senales en GTKWave o visores web
    sc_trace_file *wf = sc_create_vcd_trace_file("adder_8bit_waves");
    if (wf) {
        // Fijar resolucion temporal de la traza a 1 ns
        wf->set_time_unit(1, SC_NS);

        // Registrar senales para el diagrama de tiempos
        sc_trace(wf, sig_a, "A");
        sc_trace(wf, sig_b, "B");
        sc_trace(wf, sig_cin, "Cin");
        sc_trace(wf, sig_sum, "Sum");
        sc_trace(wf, sig_cout, "Cout");
    }

    // 5. Iniciar la simulacion de SystemC
    // El testbench invoca sc_stop() al concluir todas las pruebas
    sc_start();

    // 6. Cerrar el archivo de ondas
    if (wf) {
        sc_close_vcd_trace_file(wf);
        std::cout << "[INFO] Archivo de ondas generado con exito: adder_8bit_waves.vcd\n";
        std::cout << "       Puedes visualizar las senales con GTKWave o en https://vc.drom.io/\n\n";
    }

    return 0;
}
