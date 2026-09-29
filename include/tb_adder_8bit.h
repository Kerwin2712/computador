#ifndef TB_ADDER_8BIT_H
#define TB_ADDER_8BIT_H

#include <systemc.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>

// ============================================================================
// Modulo: tb_adder_8bit (Banco de Pruebas / Testbench Auto-verificable)
// Descripcion:
//   Aplica vectores de prueba dirigidos y aleatorios al sumador de 8 bits.
//   Mide los tiempos con sc_time_stamp(), verifica los resultados comparando
//   contra el modelo matematico esperado, y reporta estadísticas.
// ============================================================================
SC_MODULE(tb_adder_8bit) {
    // Puertos hacia el DUT (direccion opuesta al sumador)
    sc_out<sc_uint<8>> a;
    sc_out<sc_uint<8>> b;
    sc_out<bool>       cin;
    sc_in<sc_uint<8>>  sum;
    sc_in<bool>        cout;

    int total_tests;
    int passed_tests;

    // Metodo auxiliar para aplicar estimulos y verificar respuesta
    void apply_and_verify(uint8_t val_a, uint8_t val_b, bool val_cin, const char* descripcion) {
        a.write(val_a);
        b.write(val_b);
        cin.write(val_cin);

        // Esperar 10 nanosegundos para permitir propagacion de senales
        wait(10, SC_NS);

        // Calculo del resultado esperado
        uint16_t expected_total = (uint16_t)val_a + (uint16_t)val_b + (val_cin ? 1 : 0);
        uint8_t  expected_sum   = (uint8_t)(expected_total & 0xFF);
        bool     expected_cout  = (bool)((expected_total >> 8) & 0x01);

        uint8_t  actual_sum  = (uint8_t)sum.read().to_uint();
        bool     actual_cout = cout.read();

        bool ok = (actual_sum == expected_sum) && (actual_cout == expected_cout);
        total_tests++;
        if (ok) passed_tests++;

        std::cout << std::setw(8) << sc_time_stamp()
                  << " | " << std::setw(3) << (int)val_a
                  << " + " << std::setw(3) << (int)val_b
                  << " + " << std::setw(1) << (int)val_cin
                  << " | Sum=" << std::setw(3) << (int)actual_sum
                  << " Cout=" << actual_cout
                  << " | Esp(Sum=" << std::setw(3) << (int)expected_sum
                  << " Cout=" << expected_cout << ")"
                  << " | [" << (ok ? "  OK  " : "FALLO") << "]"
                  << " - " << descripcion << "\n";
    }

    void test_process() {
        total_tests = 0;
        passed_tests = 0;

        std::cout << "\n========================================================================================\n";
        std::cout << "                  TESTBENCH: SUMADOR DE 8 BITS (SystemC IEEE 1666)                      \n";
        std::cout << "========================================================================================\n";
        std::cout << std::setw(8) << "Tiempo"
                  << " | Operacion       | Salida Obtenida   | Salida Esperada       | Estado | Descripcion\n";
        std::cout << "----------------------------------------------------------------------------------------\n";

        // 1. Casos dirigidos (Corner Cases y pruebas clave)
        apply_and_verify(0, 0, false, "Cero mas cero");
        apply_and_verify(5, 3, false, "Suma basica sin acarreo");
        apply_and_verify(10, 20, true, "Suma con acarreo inicial Cin=1");
        apply_and_verify(127, 128, false, "Suma maximo valor positivo signed 8-bit");
        apply_and_verify(254, 1, false, "Limite 255 sin desbordar");
        apply_and_verify(255, 1, false, "Desbordamiento 8 bits (Cout=1, Sum=0)");
        apply_and_verify(255, 255, false, "Maximo A y B (510 -> Cout=1, Sum=254)");
        apply_and_verify(255, 255, true, "Maximo absoluto (511 -> Cout=1, Sum=255)");
        apply_and_verify(0xAA, 0x55, false, "Patrones alternados 10101010 + 01010101");
        apply_and_verify(0x0F, 0x01, false, "Acarreo entre nibbles (bit 3 a bit 4)");

        // 2. Pruebas pseudo-aleatorias
        std::srand(12345);
        for (int i = 1; i <= 10; ++i) {
            uint8_t ra = std::rand() % 256;
            uint8_t rb = std::rand() % 256;
            bool rc = (std::rand() % 2) != 0;
            char desc[32];
            std::snprintf(desc, sizeof(desc), "Prueba aleatoria #%d", i);
            apply_and_verify(ra, rb, rc, desc);
        }

        std::cout << "----------------------------------------------------------------------------------------\n";
        std::cout << "RESUMEN: " << passed_tests << " de " << total_tests << " pruebas pasaron exitosamente.\n";
        if (passed_tests == total_tests) {
            std::cout << ">>> ESTADO: TODAS LAS PRUEBAS FUNCIONARON CORRECTAMENTE <<<\n";
        } else {
            std::cout << ">>> ALERTA: SE DETECTARON FALLOS EN LA SIMULACION <<<\n";
        }
        std::cout << "========================================================================================\n\n";

        sc_stop(); // Finalizar el bucle de eventos de SystemC
    }

    SC_CTOR(tb_adder_8bit) {
        SC_THREAD(test_process);
    }
};

#endif // TB_ADDER_8BIT_H
