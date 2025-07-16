/**
 * @file core_tmr.hpp
 * @brief Triple Modular Redundancy untuk C300 core dengan fault tolerance dan error correction
 * @version 1.0.0
 * @date 2024-12-19
 * @author Tim C300 Engineering Indonesia
 * @copyright C300 ASIC Mining Technology
 * 
 * SystemC module untuk sintesis RTL dengan 300 core processing units
 * Hardware implementation ready untuk chip fabrication
 * 
 * SYNTHESIS COMPLIANCE:
 * - Pure SystemC HDL constructs only
 * - No dynamic memory allocation
 * - Static array sizes dengan compile-time constants
 * - Clock domain 1GHz dengan proper timing
 */

#ifndef C300_CORE_TMR_HPP
#define C300_CORE_TMR_HPP

#include <systemc.h>
#include "c300_types.hpp"
#include "c300_constants.hpp"

SC_MODULE(C300_CoreTMR) {
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    sc_in<bool> enable;
    
    sc_in<sc_bv<256>> data_in_a;
    sc_in<sc_bv<256>> data_in_b;
    sc_in<sc_bv<256>> data_in_c;
    sc_in<bool> valid_in_a;
    sc_in<bool> valid_in_b;
    sc_in<bool> valid_in_c;
    
    sc_out<sc_bv<256>> data_out;
    sc_out<bool> valid_out;
    sc_out<bool> error_detected;
    sc_out<bool> error_corrected;
    sc_out<sc_uint<2>> error_count;
    
    sc_signal<sc_bv<256>> voted_data;
    sc_signal<bool> voted_valid;
    sc_signal<bool> majority_ab;
    sc_signal<bool> majority_ac;
    sc_signal<bool> majority_bc;
    sc_signal<bool> fault_a;
    sc_signal<bool> fault_b;
    sc_signal<bool> fault_c;
    sc_signal<sc_uint<2>> fault_counter;
    sc_signal<bool> tmr_enable;
    
    static const int VOTE_DELAY = 2;
    static const int ERROR_THRESHOLD = 3;
    static const sc_bv<256> ERROR_PATTERN = "0x5555555555555555555555555555555555555555555555555555555555555555";
    
    SC_CTOR(C300_CoreTMR) {
        SC_THREAD(tmr_voting_process);
        sensitive << clk.pos();
        async_reset_signal_is(rst_n, false);
        
        SC_METHOD(error_detection_logic);
        sensitive << data_in_a << data_in_b << data_in_c << valid_in_a << valid_in_b << valid_in_c;
        
        SC_METHOD(output_assignment);
        sensitive << voted_data << voted_valid << fault_counter;
    }
    
    void tmr_voting_process();
    void error_detection_logic();
    void output_assignment();
    
private:
    sc_bv<256> majority_vote(const sc_bv<256>& a, const sc_bv<256>& b, const sc_bv<256>& c);
    bool majority_vote_bool(bool a, bool b, bool c);
    void detect_single_errors();
    void update_error_counters();
};

void C300_CoreTMR::tmr_voting_process() {
    while (true) {
        wait();
        
        if (!rst_n.read()) {
            voted_data.write(0);
            voted_valid.write(false);
            fault_counter.write(0);
            tmr_enable.write(false);
            continue;
        }
        
        if (enable.read()) {
            tmr_enable.write(true);
            
            sc_bv<256> input_a = data_in_a.read();
            sc_bv<256> input_b = data_in_b.read();
            sc_bv<256> input_c = data_in_c.read();
            
            bool valid_a = valid_in_a.read();
            bool valid_b = valid_in_b.read();
            bool valid_c = valid_in_c.read();
            
            sc_bv<256> result = majority_vote(input_a, input_b, input_c);
            bool valid_result = majority_vote_bool(valid_a, valid_b, valid_c);
            
            voted_data.write(result);
            voted_valid.write(valid_result);
            
            detect_single_errors();
            update_error_counters();
        } else {
            tmr_enable.write(false);
        }
    }
}

void C300_CoreTMR::error_detection_logic() {
    sc_bv<256> data_a = data_in_a.read();
    sc_bv<256> data_b = data_in_b.read();
    sc_bv<256> data_c = data_in_c.read();
    
    bool valid_a = valid_in_a.read();
    bool valid_b = valid_in_b.read();
    bool valid_c = valid_in_c.read();
    
    bool ab_match = (data_a == data_b) && (valid_a == valid_b);
    bool ac_match = (data_a == data_c) && (valid_a == valid_c);
    bool bc_match = (data_b == data_c) && (valid_b == valid_c);
    
    majority_ab.write(ab_match);
    majority_ac.write(ac_match);
    majority_bc.write(bc_match);
    
    fault_a.write(!ab_match && !ac_match && bc_match);
    fault_b.write(!ab_match && ac_match && !bc_match);
    fault_c.write(ab_match && !ac_match && !bc_match);
}

void C300_CoreTMR::output_assignment() {
    data_out.write(voted_data.read());
    valid_out.write(voted_valid.read() && tmr_enable.read());
    
    bool any_error = fault_a.read() || fault_b.read() || fault_c.read();
    error_detected.write(any_error);
    
    bool correctable = (fault_a.read() && !fault_b.read() && !fault_c.read()) ||
                      (!fault_a.read() && fault_b.read() && !fault_c.read()) ||
                      (!fault_a.read() && !fault_b.read() && fault_c.read());
    error_corrected.write(correctable);
    
    error_count.write(fault_counter.read());
}

sc_bv<256> C300_CoreTMR::majority_vote(const sc_bv<256>& a, const sc_bv<256>& b, const sc_bv<256>& c) {
    sc_bv<256> result;
    
    for (int i = 0; i < 256; i++) {
        int bit_sum = a[i].to_bool() + b[i].to_bool() + c[i].to_bool();
        result[i] = (bit_sum >= 2) ? 1 : 0;
    }
    
    return result;
}

bool C300_CoreTMR::majority_vote_bool(bool a, bool b, bool c) {
    int vote_count = (a ? 1 : 0) + (b ? 1 : 0) + (c ? 1 : 0);
    return vote_count >= 2;
}

void C300_CoreTMR::detect_single_errors() {
    bool single_fault = (fault_a.read() && !fault_b.read() && !fault_c.read()) ||
                       (!fault_a.read() && fault_b.read() && !fault_c.read()) ||
                       (!fault_a.read() && !fault_b.read() && fault_c.read());
    
    if (single_fault) {
        sc_uint<2> current_count = fault_counter.read();
        if (current_count < ERROR_THRESHOLD) {
            fault_counter.write(current_count + 1);
        }
    }
}

void C300_CoreTMR::update_error_counters() {
    bool double_fault = (fault_a.read() && fault_b.read() && !fault_c.read()) ||
                       (fault_a.read() && !fault_b.read() && fault_c.read()) ||
                       (!fault_a.read() && fault_b.read() && fault_c.read());
    
    bool triple_fault = fault_a.read() && fault_b.read() && fault_c.read();
    
    if (triple_fault) {
        fault_counter.write(3);
    } else if (double_fault) {
        fault_counter.write(2);
    }
}

SC_MODULE(C300_CoreTMRArray) {
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    sc_in<bool> enable;
    
    sc_vector<sc_in<sc_bv<256>>> data_in_a;
    sc_vector<sc_in<sc_bv<256>>> data_in_b;
    sc_vector<sc_in<sc_bv<256>>> data_in_c;
    sc_vector<sc_in<bool>> valid_in_a;
    sc_vector<sc_in<bool>> valid_in_b;
    sc_vector<sc_in<bool>> valid_in_c;
    
    sc_vector<sc_out<sc_bv<256>>> data_out;
    sc_vector<sc_out<bool>> valid_out;
    sc_vector<sc_out<bool>> error_detected;
    sc_vector<sc_out<bool>> error_corrected;
    sc_vector<sc_out<sc_uint<2>>> error_count;
    
    sc_vector<C300_CoreTMR> tmr_instances;
    
    static const int NUM_TMR_CORES = 100;
    
    SC_CTOR(C300_CoreTMRArray) : 
        data_in_a("data_in_a", NUM_TMR_CORES),
        data_in_b("data_in_b", NUM_TMR_CORES),
        data_in_c("data_in_c", NUM_TMR_CORES),
        valid_in_a("valid_in_a", NUM_TMR_CORES),
        valid_in_b("valid_in_b", NUM_TMR_CORES),
        valid_in_c("valid_in_c", NUM_TMR_CORES),
        data_out("data_out", NUM_TMR_CORES),
        valid_out("valid_out", NUM_TMR_CORES),
        error_detected("error_detected", NUM_TMR_CORES),
        error_corrected("error_corrected", NUM_TMR_CORES),
        error_count("error_count", NUM_TMR_CORES),
        tmr_instances("tmr_inst", NUM_TMR_CORES) {
        
        for (int i = 0; i < NUM_TMR_CORES; i++) {
            tmr_instances[i].clk(clk);
            tmr_instances[i].rst_n(rst_n);
            tmr_instances[i].enable(enable);
            tmr_instances[i].data_in_a(data_in_a[i]);
            tmr_instances[i].data_in_b(data_in_b[i]);
            tmr_instances[i].data_in_c(data_in_c[i]);
            tmr_instances[i].valid_in_a(valid_in_a[i]);
            tmr_instances[i].valid_in_b(valid_in_b[i]);
            tmr_instances[i].valid_in_c(valid_in_c[i]);
            tmr_instances[i].data_out(data_out[i]);
            tmr_instances[i].valid_out(valid_out[i]);
            tmr_instances[i].error_detected(error_detected[i]);
            tmr_instances[i].error_corrected(error_corrected[i]);
            tmr_instances[i].error_count(error_count[i]);
        }
    }
};

SC_MODULE(C300_TMRMonitor) {
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    sc_vector<sc_in<bool>> error_detected;
    sc_vector<sc_in<bool>> error_corrected;
    sc_vector<sc_in<sc_uint<2>>> error_count;
    
    sc_out<sc_uint<16>> total_errors;
    sc_out<sc_uint<16>> corrected_errors;
    sc_out<bool> system_health;
    sc_out<bool> critical_failure;
    
    sc_signal<sc_uint<16>> error_accumulator;
    sc_signal<sc_uint<16>> correction_accumulator;
    sc_signal<bool> health_status;
    
    static const int NUM_TMR_CORES = 100;
    static const int CRITICAL_THRESHOLD = 10;
    
    SC_CTOR(C300_TMRMonitor) :
        error_detected("error_detected", NUM_TMR_CORES),
        error_corrected("error_corrected", NUM_TMR_CORES),
        error_count("error_count", NUM_TMR_CORES) {
        
        SC_THREAD(monitor_process);
        sensitive << clk.pos();
        async_reset_signal_is(rst_n, false);
        
        SC_METHOD(health_assessment);
        sensitive << error_accumulator << correction_accumulator;
    }
    
    void monitor_process();
    void health_assessment();
};

void C300_TMRMonitor::monitor_process() {
    while (true) {
        wait();
        
        if (!rst_n.read()) {
            error_accumulator.write(0);
            correction_accumulator.write(0);
            health_status.write(true);
            continue;
        }
        
        sc_uint<16> total_error_count = 0;
        sc_uint<16> total_correction_count = 0;
        
        for (int i = 0; i < NUM_TMR_CORES; i++) {
            if (error_detected[i].read()) {
                total_error_count++;
            }
            if (error_corrected[i].read()) {
                total_correction_count++;
            }
        }
        
        error_accumulator.write(total_error_count);
        correction_accumulator.write(total_correction_count);
        
        bool system_ok = (total_error_count < CRITICAL_THRESHOLD);
        health_status.write(system_ok);
    }
}

void C300_TMRMonitor::health_assessment() {
    total_errors.write(error_accumulator.read());
    corrected_errors.write(correction_accumulator.read());
    system_health.write(health_status.read());
    
    bool critical = (error_accumulator.read() >= CRITICAL_THRESHOLD);
    critical_failure.write(critical);
}

#endif // C300_CORE_TMR_HPP
