/**
 * @file c300_system.hpp
 * @brief Top-level SystemC integration dengan system-on-chip architecture
 * @version 1.0.0
 * @date 2024-12-19
 * @author Tim C300 Engineering
 * @copyright C300 ASIC Mining Technology
 * 
 * SystemC module untuk sintesis RTL dengan 300 core processing units
 * Hardware implementation ready untuk chip fabrication
 */

#ifndef C300_SYSTEM_HPP
#define C300_SYSTEM_HPP

#include <systemc.h>
#include <tlm.h>
#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include "../core/c300_core.hpp"
#include "../engine/c300_engine.hpp"
#include "../controller/c300_controller.hpp"
#include "../network/c300_network.hpp"
#include "../optimization/compute_optimization.hpp"
#include "../optimization/throughput_optimization.hpp"
#include "../optimization/memory_pool.hpp"
#include "../optimization/simd_acceleration.hpp"

namespace c300::systemc {

    class C300_SystemPowerMonitor : public sc_module {
    public:
        sc_in<bool> clk;
        sc_in<bool> rst_n;
        sc_out<bool> power_good;
        sc_out<bool> thermal_throttle;
        
        static constexpr uint32_t THERMAL_THRESHOLD = 85;
        static constexpr uint32_t POWER_BUDGET_W = 240;
        
        SC_CTOR(C300_SystemPowerMonitor) {
            SC_THREAD(power_monitor_process);
            sensitive << clk.pos();
            async_reset_signal_is(rst_n, false);
            
            power_good.write(false);
            thermal_throttle.write(false);
        }
        
        void power_monitor_process();
        
    private:
        std::atomic<uint32_t> current_power_w{0};
        std::atomic<uint32_t> current_temp_c{25};
        std::atomic<uint64_t> power_cycles{0};
    };

    class C300_SystemClockManager : public sc_module {
    public:
        sc_in<bool> ext_clk;
        sc_in<bool> rst_n;
        sc_out<bool> core_clk;
        sc_out<bool> engine_clk;
        sc_out<bool> controller_clk;
        sc_out<bool> network_clk;
        sc_out<bool> system_clk;
        sc_out<bool> pll_locked;
        
        SC_CTOR(C300_SystemClockManager) {
            SC_THREAD(clock_generation_process);
            sensitive << ext_clk.pos();
            async_reset_signal_is(rst_n, false);
            
            SC_THREAD(pll_lock_process);
            sensitive << rst_n.pos();
            
            core_clk.write(false);
            engine_clk.write(false);
            controller_clk.write(false);
            network_clk.write(false);
            system_clk.write(false);
            pll_locked.write(false);
        }
        
        void clock_generation_process();
        void pll_lock_process();
        
    private:
        bool pll_stable = false;
        uint32_t lock_counter = 0;
        static constexpr uint32_t PLL_LOCK_CYCLES = 1000;
    };

    class C300_SystemResetController : public sc_module {
    public:
        sc_in<bool> por_rst_n;
        sc_in<bool> soft_rst_n;
        sc_in<bool> pll_locked;
        sc_out<bool> core_rst_n;
        sc_out<bool> engine_rst_n;
        sc_out<bool> controller_rst_n;
        sc_out<bool> network_rst_n;
        sc_out<bool> system_rst_n;
        
        SC_CTOR(C300_SystemResetController) {
            SC_THREAD(reset_sequence_process);
            sensitive << por_rst_n.neg() << soft_rst_n.neg() << pll_locked.pos();
            
            core_rst_n.write(false);
            engine_rst_n.write(false);
            controller_rst_n.write(false);
            network_rst_n.write(false);
            system_rst_n.write(false);
        }
        
        void reset_sequence_process();
        
    private:
        enum reset_state_t {
            RESET_ASSERT,
            RESET_WAIT_PLL,
            RESET_SEQUENCE,
            RESET_RELEASE
        };
        
        reset_state_t reset_state = RESET_ASSERT;
        uint32_t sequence_counter = 0;
    };

    class C300_SystemInterconnect : public sc_module {
    public:
        sc_port<tlm::tlm_blocking_transport_if<>> core_ports[300];
        sc_port<tlm::tlm_blocking_transport_if<>> engine_port;
        sc_port<tlm::tlm_blocking_transport_if<>> controller_port;
        sc_port<tlm::tlm_blocking_transport_if<>> network_port;
        
        SC_CTOR(C300_SystemInterconnect) {
            SC_THREAD(interconnect_process);
        }
        
        void interconnect_process();
        
    private:
        C300_MemoryPool memory_pool;
        std::atomic<uint64_t> transaction_count{0};
    };

    class C300_SystemProfiler : public sc_module {
    public:
        sc_in<bool> clk;
        sc_in<bool> rst_n;
        sc_in<bool> core_active[300];
        sc_in<uint32_t> hash_count;
        sc_out<bool> profile_enable;
        
        SC_CTOR(C300_SystemProfiler) {
            SC_THREAD(profiler_process);
            sensitive << clk.pos();
            async_reset_signal_is(rst_n, false);
            
            profile_enable.write(true);
        }
        
        void profiler_process();
        
    private:
        std::atomic<uint64_t> total_hashes{0};
        std::atomic<uint64_t> total_cycles{0};
        std::atomic<uint32_t> active_cores{0};
        std::chrono::high_resolution_clock::time_point start_time;
        
        static constexpr double TARGET_THROUGHPUT = 144e12;
        static constexpr double MIN_EFFICIENCY = 0.9;
    };

    SC_MODULE(C300_SystemModule) {
    public:
        sc_in<bool> ext_clk;
        sc_in<bool> por_rst_n;
        sc_in<bool> soft_rst_n;
        sc_out<bool> system_ready;
        sc_out<uint32_t> hash_rate;
        sc_out<bool> error_status;
        
        tlm::tlm_initiator_socket<> network_socket;
        
        SC_CTOR(C300_SystemModule) : 
            power_monitor("power_monitor"),
            clock_manager("clock_manager"),
            reset_controller("reset_controller"),
            interconnect("interconnect"),
            profiler("profiler"),
            memory_pool()
        {
            initialize_cores();
            initialize_engines();
            initialize_controllers();
            initialize_network();
            connect_system();
            
            SC_THREAD(system_orchestration_process);
            sensitive << internal_clk.pos();
            async_reset_signal_is(internal_rst_n, false);
            
            SC_THREAD(lifecycle_management_process);
            sensitive << internal_clk.pos();
            async_reset_signal_is(internal_rst_n, false);
            
            system_ready.write(false);
            hash_rate.write(0);
            error_status.write(false);
        }
        
        void system_orchestration_process();
        void lifecycle_management_process();
        
    private:
        C300_SystemPowerMonitor power_monitor;
        C300_SystemClockManager clock_manager;
        C300_SystemResetController reset_controller;
        C300_SystemInterconnect interconnect;
        C300_SystemProfiler profiler;
        
        sc_vector<C300_CoreModule> cores;
        sc_vector<C300_EngineModule> engines;
        C300_ControllerModule controller;
        C300_NetworkModule network;
        
        C300_MemoryPool memory_pool;
        C300_SIMDProcessor simd_processor;
        C300_PerformanceMonitor performance_monitor;
        
        sc_signal<bool> internal_clk;
        sc_signal<bool> internal_rst_n;
        sc_signal<bool> core_clk;
        sc_signal<bool> engine_clk;
        sc_signal<bool> controller_clk;
        sc_signal<bool> network_clk;
        sc_signal<bool> pll_locked;
        sc_signal<bool> power_good;
        sc_signal<bool> thermal_throttle;
        
        sc_signal<bool> core_rst_n[300];
        sc_signal<bool> engine_rst_n[300];
        sc_signal<bool> controller_rst_n;
        sc_signal<bool> network_rst_n;
        
        sc_signal<bool> core_active[300];
        sc_signal<uint32_t> core_hash_count[300];
        sc_signal<bool> core_error[300];
        
        void initialize_cores();
        void initialize_engines();
        void initialize_controllers();
        void initialize_network();
        void connect_system();
        
        enum system_state_t {
            SYSTEM_RESET,
            SYSTEM_INIT,
            SYSTEM_CALIBRATION,
            SYSTEM_ACTIVE,
            SYSTEM_THROTTLE,
            SYSTEM_ERROR
        };
        
        system_state_t system_state = SYSTEM_RESET;
        uint32_t init_counter = 0;
        std::atomic<uint64_t> system_cycles{0};
        std::atomic<uint32_t> total_hash_rate{0};
        std::atomic<bool> adaptive_mode{false};
    };

    class C300_SystemIntegration : public sc_module {
    public:
        sc_in<bool> board_clk;
        sc_in<bool> board_rst_n;
        sc_out<bool> chip_ready;
        sc_out<uint32_t> chip_performance;
        sc_out<uint32_t> chip_power;
        sc_out<bool> chip_error;
        
        SC_CTOR(C300_SystemIntegration) :
            system_module("system_module")
        {
            system_module.ext_clk(board_clk);
            system_module.por_rst_n(board_rst_n);
            system_module.soft_rst_n(board_rst_n);
            system_module.system_ready(chip_ready);
            system_module.hash_rate(chip_performance);
            system_module.error_status(chip_error);
            
            SC_THREAD(integration_process);
            sensitive << board_clk.pos();
            async_reset_signal_is(board_rst_n, false);
            
            chip_ready.write(false);
            chip_performance.write(0);
            chip_power.write(0);
            chip_error.write(false);
        }
        
        void integration_process();
        
    private:
        C300_SystemModule system_module;
        std::atomic<uint64_t> integration_cycles{0};
        std::atomic<uint32_t> power_consumption{0};
    };

}

#endif
