#ifndef TB_ADD_SUB_8BIT_H
#define TB_ADD_SUB_8BIT_H

#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>

// ============================================================================
// Modulo: tb_add_sub_8bit (Testbench para Sumador / Restador de 8 Bits)
// Descripcion:
//   Aplica vectores de prueba de suma y resta, tanto en interpretacion sin signo
//   (unsigned) como con signo (signed - Complemento a 2).
//   Verifica exhaustivamente las 4 banderas:
//     - Z (Zero)
//     - N (Negative)
//     - C (Carry / Cout)
//     - V (Overflow)
// ============================================================================
SC_MODULE(tb_add_sub_8bit) {
    // Puertos hacia el DUT
    sc_out<sc_uint<8>> a;
    sc_out<sc_uint<8>> b;
    sc_out<bool>       sub;

    sc_in<sc_uint<8>>  result;
    sc_in<bool>        overflow;
    sc_in<bool>        carry;
    sc_in<bool>        zero;
    sc_in<bool>        negative;

    int total_tests;
    int passed_tests;

    void apply_and_verify(uint8_t val_a, uint8_t val_b, bool is_sub, const char* descripcion) {
        a.write(val_a);
        b.write(val_b);
        sub.write(is_sub);

        // Esperar tiempo de propagacion en SystemC
        wait(10, SC_NS);

        // --- Calculo del modelo teorico esperado ---
        uint8_t b_eff = is_sub ? (uint8_t)(~val_b) : val_b;
        uint16_t cin_val = is_sub ? 1 : 0;
        uint16_t full_sum = (uint16_t)val_a + (uint16_t)b_eff + cin_val;

        uint8_t exp_result = (uint8_t)(full_sum & 0xFF);
        bool    exp_c      = (bool)((full_sum >> 8) & 0x01);
        bool    exp_z      = (exp_result == 0);
        bool    exp_n      = (bool)((exp_result >> 7) & 0x01);

        // Deteccion teorica de desbordamiento en complemento a 2 (V)
        int8_t s_a = (int8_t)val_a;
        int8_t s_b = (int8_t)val_b;
        int16_t s_res_exact = is_sub ? ((int16_t)s_a - (int16_t)s_b) : ((int16_t)s_a + (int16_t)s_b);
        bool exp_v = (s_res_exact < -128) || (s_res_exact > 127);

        // Lectura de los valores reales producidos por el hardware
        uint8_t act_result = (uint8_t)result.read().to_uint();
        bool    act_v      = overflow.read();
        bool    act_c      = carry.read();
        bool    act_z      = zero.read();
        bool    act_n      = negative.read();

        bool match = (act_result == exp_result) &&
                     (act_v == exp_v) &&
                     (act_c == exp_c) &&
                     (act_z == exp_z) &&
                     (act_n == exp_n);

        total_tests++;
        if (match) passed_tests++;

        std::cout << std::setw(6) << sc_time_stamp()
                  << " | " << (is_sub ? "SUB " : "ADD ")
                  << std::setw(3) << (int)val_a << " (" << std::setw(4) << (int)s_a << ")"
                  << (is_sub ? " - " : " + ")
                  << std::setw(3) << (int)val_b << " (" << std::setw(4) << (int)s_b << ")"
                  << " | Res=" << std::setw(3) << (int)act_result
                  << " [Z=" << act_z << " N=" << act_n << " C=" << act_c << " V=" << act_v << "]"
                  << " | Esp=" << std::setw(3) << (int)exp_result
                  << " [Z=" << exp_z << " N=" << exp_n << " C=" << exp_c << " V=" << exp_v << "]"
                  << " | [" << (match ? "  OK  " : "FALLO") << "] "
                  << descripcion << "\n";
    }

    void test_process() {
        total_tests = 0;
        passed_tests = 0;

        std::cout << "\n========================================================================================================\n";
        std::cout << "                 TESTBENCH: SUMADOR / RESTADOR DE 8 BITS (SystemC IEEE 1666)                           \n";
        std::cout << "========================================================================================================\n";
        std::cout << "Tiempo | Operacion [Signed]             | Salida Obtenida + Banderas   | Salida Esperada + Banderas   | Estado | Descripcion\n";
        std::cout << "--------------------------------------------------------------------------------------------------------\n";

        // --- PRUEBAS DE SUMA (ADD: sub = 0) ---
        apply_and_verify(10, 20, false, "ADD: Suma normal positiva sin desbordamiento");
        apply_and_verify(0, 0, false, "ADD: Cero mas cero (Activa flag Zero)");
        apply_and_verify(200, 100, false, "ADD: Desbordamiento sin signo (Activa Carry C=1)");
        apply_and_verify(100, 50, false, "ADD: OVERFLOW con signo (+100 + +50 = +150 > +127 -> V=1)");
        apply_and_verify((uint8_t)(-100), (uint8_t)(-50), false, "ADD: OVERFLOW con signo (-100 + -50 = -150 < -128 -> V=1)");
        apply_and_verify((uint8_t)(-20), 50, false, "ADD: Negativo mas positivo sin desbordamiento");

        // --- PRUEBAS DE RESTA (SUB: sub = 1) ---
        apply_and_verify(50, 20, true, "SUB: Resta normal positiva (50 - 20 = 30, C=1, V=0)");
        apply_and_verify(42, 42, true, "SUB: Resta con resultado cero (42 - 42 = 0, Z=1)");
        apply_and_verify(10, 30, true, "SUB: Resta que da negativo (10 - 30 = -20, N=1)");
        apply_and_verify(100, (uint8_t)(-50), true, "SUB: OVERFLOW resta (+100 - -50 = +150 > +127 -> V=1)");
        apply_and_verify((uint8_t)(-100), 50, true, "SUB: OVERFLOW resta (-100 - +50 = -150 < -128 -> V=1)");
        apply_and_verify((uint8_t)(-50), (uint8_t)(-20), true, "SUB: Resta entre dos negativos (-50 - -20 = -30)");

        // --- PRUEBAS ALEATORIAS ---
        std::srand(54321);
        for (int i = 1; i <= 10; ++i) {
            uint8_t ra = std::rand() % 256;
            uint8_t rb = std::rand() % 256;
            bool rsub = (std::rand() % 2) != 0;
            char desc[32];
            std::snprintf(desc, sizeof(desc), "Prueba aleatoria #%d", i);
            apply_and_verify(ra, rb, rsub, desc);
        }

        std::cout << "--------------------------------------------------------------------------------------------------------\n";
        std::cout << "RESUMEN: " << passed_tests << " de " << total_tests << " pruebas pasaron exitosamente.\n";
        if (passed_tests == total_tests) {
            std::cout << ">>> ESTADO: TODAS LAS OPERACIONES Y BANDERAS (Z, N, C, V) FUNCIONARON CORRECTAMENTE <<<\n";
        } else {
            std::cout << ">>> ALERTA: SE DETECTARON FALLOS EN LA SIMULACION <<<\n";
        }
        std::cout << "========================================================================================================\n\n";

        sc_stop();
    }

    SC_CTOR(tb_add_sub_8bit) {
        SC_THREAD(test_process);
    }
};

#endif // TB_ADD_SUB_8BIT_H
