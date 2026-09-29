#include <systemc.h>
#include "add_sub_8bit.h"
#include "tb_add_sub_8bit.h"

int sc_main(int argc, char* argv[]) {
    // 1. Declarar senales de interconexion entre DUT y Testbench
    sc_signal<sc_uint<8>> sig_a("sig_a");
    sc_signal<sc_uint<8>> sig_b("sig_b");
    sc_signal<bool>       sig_sub("sig_sub");

    sc_signal<sc_uint<8>> sig_result("sig_result");
    sc_signal<bool>       sig_overflow("sig_overflow");
    sc_signal<bool>       sig_carry("sig_carry");
    sc_signal<bool>       sig_zero("sig_zero");
    sc_signal<bool>       sig_negative("sig_negative");

    // 2. Instanciar el Dispositivo Bajo Prueba (DUT: Add/Sub 8-bit)
    add_sub_8bit uut("uut_add_sub_8bit");
    uut.a(sig_a);
    uut.b(sig_b);
    uut.sub(sig_sub);
    uut.result(sig_result);
    uut.overflow(sig_overflow);
    uut.carry(sig_carry);
    uut.zero(sig_zero);
    uut.negative(sig_negative);

    // 3. Instanciar el Testbench
    tb_add_sub_8bit testbench("tb_add_sub_8bit");
    testbench.a(sig_a);
    testbench.b(sig_b);
    testbench.sub(sig_sub);
    testbench.result(sig_result);
    testbench.overflow(sig_overflow);
    testbench.carry(sig_carry);
    testbench.zero(sig_zero);
    testbench.negative(sig_negative);

    // 4. Archivo de trazas de formas de onda (VCD)
    sc_trace_file *wf = sc_create_vcd_trace_file("add_sub_8bit_waves");
    if (wf) {
        wf->set_time_unit(1, SC_NS);
        sc_trace(wf, sig_a, "A");
        sc_trace(wf, sig_b, "B");
        sc_trace(wf, sig_sub, "Sub_Control");
        sc_trace(wf, sig_result, "Result");
        sc_trace(wf, sig_overflow, "Flag_V_Overflow");
        sc_trace(wf, sig_carry, "Flag_C_Carry");
        sc_trace(wf, sig_zero, "Flag_Z_Zero");
        sc_trace(wf, sig_negative, "Flag_N_Negative");
    }

    // 5. Iniciar la simulacion
    sc_start();

    // 6. Cerrar el archivo de ondas
    if (wf) {
        sc_close_vcd_trace_file(wf);
        std::cout << "[INFO] Formas de onda del Sumador/Restador guardadas en: add_sub_8bit_waves.vcd\n\n";
    }

    return 0;
}
