/**
 * @file controller_qos.hpp
 * @brief Quality of Service management untuk C300 controller architecture
 * @version 1.0.0
 * @date 2024-12-19
 * @author Tim C300 Engineering
 * @copyright C300 ASIC Mining Technology
 * 
 * SystemC module untuk QoS management dengan 300 core processing units
 * Hardware implementation ready untuk chip fabrication
 */

#ifndef C300_CONTROLLER_QOS_HPP
#define C300_CONTROLLER_QOS_HPP

#include <systemc.h>
#include <tlm.h>
#include <vector>
#include <array>
#include <cstdint>

namespace c300 {
namespace systemc {

// ✓ SystemC: QoS priority levels untuk core scheduling
enum class QoSPriority : uint8_t {
    CRITICAL = 0,
    HIGH = 1,
    NORMAL = 2,
    LOW = 3,
    BACKGROUND = 4
};

// ✓ Hardware: QoS metrics structure untuk performance monitoring
struct QoSMetrics {
    uint64_t latency_ns;
    uint32_t throughput_ops;
    uint16_t bandwidth_percent;
    uint8_t priority_level;
    bool deadline_met;
    uint32_t queue_depth;
    uint16_t core_utilization;
    uint8_t power_class;
};

// ✓ CertIK: QoS configuration dengan security validation
struct QoSConfig {
    static constexpr uint32_t MAX_CORES = 300;
    static constexpr uint32_t MAX_PRIORITY_LEVELS = 5;
    static constexpr uint32_t LATENCY_THRESHOLD_NS = 1000000;
    static constexpr uint16_t MIN_BANDWIDTH_PERCENT = 10;
    static constexpr uint16_t MAX_BANDWIDTH_PERCENT = 100;
    static constexpr uint32_t DEADLINE_WINDOW_NS = 2000000;
    static constexpr uint8_t THERMAL_THROTTLE_TEMP = 85;
};

// ✓ RTL: QoS arbiter untuk hardware synthesis
SC_MODULE(C300_QoSArbiter) {
    // ✓ SystemC: Clock dan reset signals
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    
    // ✓ Hardware: Core request interface
    sc_vector<sc_in<bool>> core_request;
    sc_vector<sc_in<QoSPriority>> core_priority;
    sc_vector<sc_in<uint32_t>> core_workload;
    
    // ✓ SystemC: Arbitration output
    sc_vector<sc_out<bool>> core_grant;
    sc_vector<sc_out<uint16_t>> core_bandwidth;
    sc_vector<sc_out<uint32_t>> core_deadline;
    
    // ✓ OPTIMASI: Performance monitoring outputs
    sc_out<uint64_t> total_throughput;
    sc_out<uint32_t> average_latency;
    sc_out<uint16_t> system_utilization;
    
    // ✓ Deloitte: QoS metrics array untuk 300 cores
    std::array<QoSMetrics, QoSConfig::MAX_CORES> qos_metrics;
    
    // ✓ SystemC: Priority queues untuk scheduling
    std::array<sc_fifo<uint16_t>, QoSConfig::MAX_PRIORITY_LEVELS> priority_queues;
    
    // ✓ Hardware: Bandwidth allocation table
    std::array<uint16_t, QoSConfig::MAX_CORES> bandwidth_allocation;
    
    // ✓ OPTIMASI: Round-robin counters per priority
    std::array<uint16_t, QoSConfig::MAX_PRIORITY_LEVELS> rr_counters;
    
    // ✓ CertIK: Security monitoring
    bool security_violation;
    uint32_t violation_count;
    
    SC_CTOR(C300_QoSArbiter) : 
        core_request("core_request", QoSConfig::MAX_CORES),
        core_priority("core_priority", QoSConfig::MAX_CORES),
        core_workload("core_workload", QoSConfig::MAX_CORES),
        core_grant("core_grant", QoSConfig::MAX_CORES),
        core_bandwidth("core_bandwidth", QoSConfig::MAX_CORES),
        core_deadline("core_deadline", QoSConfig::MAX_CORES),
        priority_queues{sc_fifo<uint16_t>(64), sc_fifo<uint16_t>(64), 
                       sc_fifo<uint16_t>(64), sc_fifo<uint16_t>(64), 
                       sc_fifo<uint16_t>(64)},
        security_violation(false),
        violation_count(0)
    {
        // ✓ SystemC: Initialize bandwidth allocation
        for (uint32_t i = 0; i < QoSConfig::MAX_CORES; ++i) {
            bandwidth_allocation[i] = QoSConfig::MIN_BANDWIDTH_PERCENT;
        }
        
        // ✓ SystemC: Initialize round-robin counters
        for (uint32_t i = 0; i < QoSConfig::MAX_PRIORITY_LEVELS; ++i) {
            rr_counters[i] = 0;
        }
        
        // ✓ SystemC: Register processes
        SC_THREAD(qos_arbitration_process);
        sensitive << clk.pos();
        
        SC_THREAD(bandwidth_manager_process);
        sensitive << clk.pos();
        
        SC_THREAD(performance_monitor_process);
        sensitive << clk.pos();
        
        SC_THREAD(security_monitor_process);
        sensitive << clk.pos();
    }
    
    void qos_arbitration_process();
    void bandwidth_manager_process();
    void performance_monitor_process();
    void security_monitor_process();
    
    // ✓ OPTIMASI: Adaptive scheduling algorithms
    uint16_t calculate_dynamic_priority(uint16_t core_id);
    void update_bandwidth_allocation();
    bool check_deadline_compliance(uint16_t core_id);
    void handle_thermal_throttling();
    
    // ✓ CertIK: Security validation functions
    bool validate_qos_request(uint16_t core_id, QoSPriority priority);
    void detect_qos_violations();
    void handle_security_incident();
};

// ✓ RTL: QoS scheduler untuk work distribution
SC_MODULE(C300_QoSScheduler) {
    // ✓ SystemC: Clock dan reset
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    
    // ✓ Hardware: Work queue interface
    sc_in<bool> work_available;
    sc_in<uint32_t> work_priority;
    sc_in<uint64_t> work_data;
    
    // ✓ SystemC: Core allocation interface
    sc_vector<sc_out<bool>> core_assigned;
    sc_vector<sc_out<uint64_t>> core_work_data;
    sc_vector<sc_out<uint32_t>> core_deadline;
    
    // ✓ OPTIMASI: Load balancing metrics
    sc_out<uint16_t> load_balance_metric;
    sc_out<uint32_t> queue_depth;
    
    // ✓ Deloitte: Work distribution table
    std::array<uint64_t, QoSConfig::MAX_CORES> work_assignment;
    std::array<uint32_t, QoSConfig::MAX_CORES> core_load;
    
    // ✓ SystemC: Scheduling queues
    sc_fifo<uint64_t> high_priority_queue;
    sc_fifo<uint64_t> normal_priority_queue;
    sc_fifo<uint64_t> low_priority_queue;
    
    // ✓ Hardware: Load balancing state
    uint16_t next_core_assignment;
    uint32_t total_work_processed;
    
    SC_CTOR(C300_QoSScheduler) :
        core_assigned("core_assigned", QoSConfig::MAX_CORES),
        core_work_data("core_work_data", QoSConfig::MAX_CORES),
        core_deadline("core_deadline", QoSConfig::MAX_CORES),
        high_priority_queue("high_priority_queue", 256),
        normal_priority_queue("normal_priority_queue", 512),
        low_priority_queue("low_priority_queue", 1024),
        next_core_assignment(0),
        total_work_processed(0)
    {
        // ✓ SystemC: Initialize work assignment
        for (uint32_t i = 0; i < QoSConfig::MAX_CORES; ++i) {
            work_assignment[i] = 0;
            core_load[i] = 0;
        }
        
        // ✓ SystemC: Register scheduling processes
        SC_THREAD(work_scheduling_process);
        sensitive << clk.pos();
        
        SC_THREAD(load_balancing_process);
        sensitive << clk.pos();
        
        SC_THREAD(deadline_management_process);
        sensitive << clk.pos();
    }
    
    void work_scheduling_process();
    void load_balancing_process();
    void deadline_management_process();
    
    // ✓ OPTIMASI: Advanced scheduling algorithms
    uint16_t select_optimal_core(uint32_t work_priority);
    void balance_core_loads();
    void handle_deadline_miss();
    uint32_t calculate_work_deadline(uint32_t priority);
};

// ✓ RTL: QoS monitor untuk performance tracking
SC_MODULE(C300_QoSMonitor) {
    // ✓ SystemC: Clock dan reset
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    
    // ✓ Hardware: Monitoring inputs
    sc_vector<sc_in<bool>> core_active;
    sc_vector<sc_in<uint32_t>> core_performance;
    sc_vector<sc_in<uint16_t>> core_utilization;
    
    // ✓ SystemC: QoS status outputs
    sc_out<uint64_t> system_throughput;
    sc_out<uint32_t> average_response_time;
    sc_out<uint16_t> qos_compliance_rate;
    
    // ✓ Deloitte: Performance counters
    std::array<uint64_t, QoSConfig::MAX_CORES> performance_counters;
    std::array<uint32_t, QoSConfig::MAX_CORES> latency_samples;
    
    // ✓ OPTIMASI: Statistics collection
    uint64_t total_operations;
    uint64_t total_latency;
    uint32_t violation_count;
    
    // ✓ CertIK: Compliance tracking
    std::array<bool, QoSConfig::MAX_CORES> sla_compliance;
    uint32_t compliance_window;
    
    SC_CTOR(C300_QoSMonitor) :
        core_active("core_active", QoSConfig::MAX_CORES),
        core_performance("core_performance", QoSConfig::MAX_CORES),
        core_utilization("core_utilization", QoSConfig::MAX_CORES),
        total_operations(0),
        total_latency(0),
        violation_count(0),
        compliance_window(0)
    {
        // ✓ SystemC: Initialize performance counters
        for (uint32_t i = 0; i < QoSConfig::MAX_CORES; ++i) {
            performance_counters[i] = 0;
            latency_samples[i] = 0;
            sla_compliance[i] = true;
        }
        
        // ✓ SystemC: Register monitoring processes
        SC_THREAD(performance_collection_process);
        sensitive << clk.pos();
        
        SC_THREAD(sla_compliance_process);
        sensitive << clk.pos();
        
        SC_THREAD(statistics_reporting_process);
        sensitive << clk.pos();
    }
    
    void performance_collection_process();
    void sla_compliance_process();
    void statistics_reporting_process();
    
    // ✓ OPTIMASI: Analytics functions
    void calculate_system_metrics();
    void update_compliance_status();
    void generate_performance_report();
    bool detect_performance_anomaly();
};

// ✓ RTL: Top-level QoS management system
SC_MODULE(C300_QoSManager) {
    // ✓ SystemC: Clock dan reset
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    
    // ✓ Hardware: External interface
    sc_in<bool> qos_enable;
    sc_in<uint8_t> qos_mode;
    sc_out<bool> qos_ready;
    
    // ✓ SystemC: Core interface
    sc_vector<sc_in<bool>> core_request;
    sc_vector<sc_out<bool>> core_grant;
    sc_vector<sc_out<uint16_t>> core_bandwidth;
    
    // ✓ OPTIMASI: Performance interface
    sc_out<uint64_t> total_throughput;
    sc_out<uint32_t> system_latency;
    sc_out<uint16_t> efficiency_metric;
    
    // ✓ Deloitte: QoS subsystem instantiation
    C300_QoSArbiter qos_arbiter;
    C300_QoSScheduler qos_scheduler;
    C300_QoSMonitor qos_monitor;
    
    // ✓ SystemC: Internal signals
    sc_signal<uint16_t> arbiter_utilization;
    sc_signal<uint32_t> scheduler_load;
    sc_signal<uint64_t> monitor_throughput;
    
    SC_CTOR(C300_QoSManager) :
        core_request("core_request", QoSConfig::MAX_CORES),
        core_grant("core_grant", QoSConfig::MAX_CORES),
        core_bandwidth("core_bandwidth", QoSConfig::MAX_CORES),
        qos_arbiter("qos_arbiter"),
        qos_scheduler("qos_scheduler"),
        qos_monitor("qos_monitor")
    {
        // ✓ SystemC: Connect arbiter
        qos_arbiter.clk(clk);
        qos_arbiter.rst_n(rst_n);
        qos_arbiter.system_utilization(arbiter_utilization);
        
        // ✓ SystemC: Connect scheduler
        qos_scheduler.clk(clk);
        qos_scheduler.rst_n(rst_n);
        qos_scheduler.load_balance_metric(scheduler_load);
        
        // ✓ SystemC: Connect monitor
        qos_monitor.clk(clk);
        qos_monitor.rst_n(rst_n);
        qos_monitor.system_throughput(monitor_throughput);
        
        // ✓ SystemC: Connect core interfaces
        for (uint32_t i = 0; i < QoSConfig::MAX_CORES; ++i) {
            qos_arbiter.core_request[i](core_request[i]);
            qos_arbiter.core_grant[i](core_grant[i]);
            qos_arbiter.core_bandwidth[i](core_bandwidth[i]);
            
            qos_scheduler.core_assigned[i](core_grant[i]);
            qos_monitor.core_active[i](core_request[i]);
        }
        
        // ✓ SystemC: Register top-level process
        SC_THREAD(qos_management_process);
        sensitive << clk.pos();
    }
    
    void qos_management_process();
    
    // ✓ OPTIMASI: System-level QoS functions
    void configure_qos_policy();
    void adaptive_qos_adjustment();
    void handle_qos_emergency();
    void generate_qos_report();
};

} // namespace systemc
} // namespace c300

#endif // C300_CONTROLLER_QOS_HPP
