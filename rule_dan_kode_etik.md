# HUKUM MUTLAK ETIKA DALAM MENULIS KODE SESUAI VALIDASI DELOITTE

## PRINSIP DASAR: ZERO TOLERANCE UNTUK KODE TIDAK PRODUKSI

**DILARANG KERAS:**
- Kode dummy/placeholder/simulasi/konseptual/TODO
- Kode yang menyebabkan bottleneck/overhead/race condition/overflow/overheat
- Duplikasi modul atau fungsi
- Asumsi tanpa validasi
- Kode yang tidak dapat diintegrasikan
- Global variable dan shared memory langsung

**WAJIB:**
- Semua kode siap produksi pabrik
- Integrasi modul tanpa kendala
- Dokumentasi lengkap dalam bahasa Indonesia
- Testing otomatis via GitHub workflow
- Prinsip SOLID (Single Responsibility, Open-closed, Liskov substitution, Interface segregation, Dependency inversion)
- DRY (Don't Repeat Yourself) dan KISS (Keep It Simple, Stupid)

---

## 1. STANDAR PENAMAAN DAN STRUKTUR PROYEK

### Branding/Naming Convention dengan Prefix Proyek
```cpp
// Gunakan prefix proyek di semua modul, file, dan namespace
class ABC_KontrollerFifo;
class ABC_InterfaceMemori;
namespace ABC_Utils;
```

### Konvensi Penamaan Indonesia
- **Variabel dan fungsi:** camelCase (`kontrolMemori`, `bacaData`)
- **Modul dan kelas:** PascalCase dengan prefix (`ABC_KontrollerFifo`)
- **Konstanta:** UPPER_CASE (`UKURAN_MAKSIMAL`)
- **File:** snake_case (`abc_kontroller_fifo.h`)

### Struktur Direktori Wajib
```
/proyek_abc/
  /src/
    /modul/
    /interface/
    /utilitas/
  /include/
  /tests/
  /docs/
    /modul_doc/
  README.md
  .github/workflows/
```

---

## 2. STANDAR MODUL SystemC - ENCAPSULATION DAN HIERARKI

### Deklarasi Modul Konsisten
```cpp
SC_MODULE(ABC_NetworkAdapter) {
    // Port declarations first
    sc_in<bool> clk;
    sc_out<sc_uint<32>> data_out;
    
    // Internal signals
    sc_signal<sc_uint<8>> status_internal;
    
    // Member variables (private)
private:
    int konfigurasi_param;
    
    // Process declarations
    void proses_metode();
    
public:
    SC_CTOR(ABC_NetworkAdapter) {
        SC_METHOD(proses_metode);
        sensitive << clk.pos();
    }
};
```

### Header Guard Konsisten
```cpp
#ifndef ABC_NETWORK_ADAPTER_H
#define ABC_NETWORK_ADAPTER_H
// ... kode ...
#endif // ABC_NETWORK_ADAPTER_H
```

---

## 3. KOMUNIKASI ANTAR-MODUL YANG AMAN

### Wajib Gunakan Channel Standar SystemC
```cpp
// Thread-safe communication
sc_fifo<int> buffer;
sc_mutex pengunci;
sc_signal<bool> sinyal_status;

// Message passing bukan shared memory
void kirim_data() {
    out->write(data);  // Thread-safe
    wait(10, SC_NS);
}
```

### Sinkronisasi Clock dan Event
```cpp
SC_MODULE(ABC_Producer) {
    sc_port<sc_fifo_out_if<int>> keluar;
    
    void proses() {
        while (true) {
            keluar->write(data);
            wait(10, SC_NS);  // Proper timing
        }
    }
};
```

---

## 4. MANAJEMEN LIBRARY DAN INCLUDE

### Include Hierarchy Terstruktur
```cpp
// System libraries
#include <systemc>
#include <vector>

// External libraries
#include <gtest/gtest.h>

// Internal project headers
#include "abc_definisi_umum.h"
#include "interface/abc_memory_if.h"
```

### Template untuk Common Patterns
```cpp
template<typename T>
class ABC_GenericFifo : public sc_module {
    // Implementasi generic FIFO
};
```

---

## 5. KEAMANAN DAN ROBUSTNESS

### Validasi Parameter di Konstruktor
```cpp
SC_CTOR(ABC_SecureModule) {
    if(konfigurasi_param < 0) {
        SC_REPORT_FATAL("ABC_SecureModule", "Parameter konfigurasi tidak valid");
    }
}
```

### Bound Checking dan Memory Safety
```cpp
// Gunakan std::vector bukan array mentah
std::vector<int> memori;

void tulis(uint alamat, int data) {
    if (alamat < memori.size()) {  // Bound check
        memori[alamat] = data;
    }
}
```

### Resource Management dengan Smart Pointers
```cpp
std::unique_ptr<ABC_Resource> resource;  // RAII pattern
```

---

## 6. TESTING DAN DEBUGGING WAJIB

### Unit Testing untuk Setiap Modul
```cpp
// Gunakan Google Test framework
TEST(ABC_ModulTest, FungsiDasar) {
    ABC_Modul modul("test");
    EXPECT_TRUE(modul.status_valid());
}
```

### Error Handling Konsisten
```cpp
// Gunakan sc_report untuk error handling
if (buffer.size() > UKURAN_MAKSIMAL) {
    SC_REPORT_ERROR("Buffer", "Overflow terdeteksi");
}
```

### Logging dan Assertions
```cpp
// Assertions untuk kondisi kritis
assert(buffer_size <= UKURAN_MAKSIMAL);

// Logging dengan sc_report_handler
SC_REPORT_INFO("Sistem", "Status normal");
```

---

## 7. OPTIMISASI KOMPLEKSITAS DAN PERFORMANCE

### Pemisahan Thread dan Method
```cpp
// Gunakan SC_THREAD untuk proses panjang
SC_THREAD(proses_panjang);

// Gunakan SC_METHOD untuk logic kombinatorial
SC_METHOD(proses_cepat);
sensitive << sinyal_input;
```

### FIFO Depth Optimization
```cpp
// Sesuaikan ukuran FIFO dengan throughput sistem
const unsigned int KEDALAMAN_OPTIMAL = 64;  // Berdasarkan analisis performa
```

---

## 8. DOKUMENTASI WAJIB

### Header Dokumentasi Setiap File
```cpp
/**
 * @file abc_kontroller_fifo.h
 * @brief Kontroller FIFO untuk proyek ABC
 * @version 1.0.0
 * @copyright Copyright (C) 2024 ABC Company
 * @author Tim ABC
 * 
 * Deskripsi detail fungsi modul...
 */
```

### Dokumentasi Modul Khusus
Setiap modul harus memiliki file `namaModul_doc.md`:
```markdown
# ABC_KontrollerFifo_doc.md

## Use Case
- Buffering data antar modul
- Kontrol aliran data dengan backpressure

## Interface
- Input: clk, reset_n, data_masuk
- Output: data_keluar, status_penuh

## Konfigurasi
- kedalaman_fifo: 1-1024 (default: 64)
```

---

## 9. BUILD SYSTEM DAN INTEGRASI

### CMake/Makefile Terstandarisasi
```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.12)
project(ABC_Project)

find_package(SystemC REQUIRED)
add_executable(abc_system src/main.cpp)
target_link_libraries(abc_system SystemC::systemc)
```

### GitHub Workflow untuk CI/CD
```yaml
# .github/workflows/ci.yml
name: Validasi Kode ABC
on: [push, pull_request]
jobs:
  build-and-test:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Build dan Test
      run: |
        mkdir build && cd build
        cmake .. && make -j4
        ./run_tests
```

### Static Analysis Tools
```bash
# Wajib gunakan tools analisis
cppcheck --enable=all src/
clang-tidy src/*.cpp
```

---

## 10. KEPATUHAN STANDAR INDUSTRI

### IEEE 1666 SystemC Standard
- Patuhi standar SystemC resmi
- Gunakan hanya konstruksi SystemC yang standar

### Factory Pattern untuk Modul Kompleks
```cpp
class ABC_ModuleFactory {
public:
    static std::unique_ptr<ABC_BaseModule> createModule(const std::string& type);
};
```

### Continuous Integration
- Build otomatis setiap commit
- Test suite lengkap
- Performance benchmark

---

## 11. CHECKLIST KODE PRODUKSI

### Sebelum Commit
- [ ] Semua test unit passed
- [ ] Tidak ada TODO/FIXME/placeholder
- [ ] Validasi parameter lengkap
- [ ] Memory management aman
- [ ] Dokumentasi `namaModul_doc.md` complete
- [ ] Prinsip SOLID diterapkan

### Sebelum Merge
- [ ] Code review oleh minimal 2 engineer
- [ ] GitHub Actions passed
- [ ] Static analysis clean
- [ ] Performance benchmark passed
- [ ] Integrasi dengan modul lain tested

---

## 12. PELANGGARAN DAN SANKSI

### Pelanggaran Tingkat 1 (Warning)
- Penamaan tidak sesuai standar
- Komentar kurang lengkap

### Pelanggaran Tingkat 2 (Reject)
- Kode dummy/placeholder/TODO
- Missing unit tests
- Duplikasi kode

### Pelanggaran Tingkat 3 (Blacklist)
- Race condition/overflow/memory leak
- Kode menyebabkan system crash
- Global variable usage

---

## 13. TEMPLATE QUICK START

```cpp
// Header template minimal
#ifndef ABC_MODUL_BARU_H
#define ABC_MODUL_BARU_H

#include <systemc>
#include "abc_definisi_umum.h"

SC_MODULE(ABC_ModulBaru) {
    sc_in<bool> clk;
    sc_in<bool> reset_n;
    
private:
    sc_signal<bool> status_internal;
    
    void proses_utama();
    
public:
    SC_CTOR(ABC_ModulBaru) {
        SC_METHOD(proses_utama);
        sensitive << clk.pos();
        async_reset_signal_is(reset_n, false);
    }
};

#endif // ABC_MODUL_BARU_H
```

---

## 14. HUKUM PENULISAN HEADER (.h)

### Struktur Header Wajib
```cpp
#ifndef ABC_NAMA_MODUL_H
#define ABC_NAMA_MODUL_H

// 1. Include system libraries
#include <systemc>
#include <vector>

// 2. Include external libraries
#include <gtest/gtest.h>

// 3. Include project headers
#include "abc_definisi_umum.h"
#include "interface/abc_base_interface.h"

// 4. Forward declarations
class ABC_HelperClass;

/**
 * @brief Deskripsi singkat modul
 * @details Penjelasan detail fungsi dan tujuan modul
 */
SC_MODULE(ABC_NamaModul) {
public:
    // 5. Port declarations - selalu public dan pertama
    sc_in<bool>         clk;
    sc_in<bool>         reset_n;
    sc_out<sc_uint<32>> data_keluar;
    
    // 6. Public interface methods
    void konfigurasi(const ABC_Config& config);
    bool status_siap() const;
    
    // 7. Constructor
    SC_HAS_PROCESS(ABC_NamaModul);
    ABC_NamaModul(sc_module_name nama, const ABC_Config& config);

private:
    // 8. Internal signals
    sc_signal<sc_uint<8>> status_internal;
    
    // 9. Private member variables
    ABC_Config konfigurasi_modul;
    
    // 10. Process declarations
    void proses_utama();
    void proses_monitor();
    
    // 11. Helper methods
    void validasi_parameter();
    void inisialisasi_hardware();
};

#endif // ABC_NAMA_MODUL_H
```

### Aturan Header Mutlak
- **Satu class per header file**
- **Tidak ada implementasi dalam header** (kecuali inline sederhana)
- **Include guard wajib** dengan format `ABC_NAMA_MODUL_H`
- **Forward declaration** untuk mengurangi dependency
- **Doxygen comments** untuk semua public interface
- **Urutan deklarasi tetap**: public ports → public methods → constructor → private members

---

## 15. HUKUM PENULISAN IMPLEMENTASI (.cpp)

### Struktur Implementasi Wajib
```cpp
// 1. Include header corresponding
#include "abc_nama_modul.h"

// 2. Include additional headers jika diperlukan
#include "abc_helper_class.h"
#include <iostream>

// 3. Constructor implementation
ABC_NamaModul::ABC_NamaModul(sc_module_name nama, const ABC_Config& config)
    : sc_module(nama)
    , konfigurasi_modul(config)
{
    // Validasi parameter wajib
    validasi_parameter();
    
    // Inisialisasi hardware
    inisialisasi_hardware();
    
    // Process registration
    SC_METHOD(proses_utama);
    sensitive << clk.pos();
    async_reset_signal_is(reset_n, false);
    
    SC_THREAD(proses_monitor);
    sensitive << clk.pos();
}

// 4. Public interface implementation
void ABC_NamaModul::konfigurasi(const ABC_Config& config) {
    // Validasi input
    if (!config.valid()) {
        SC_REPORT_FATAL("ABC_NamaModul", "Konfigurasi tidak valid");
    }
    
    konfigurasi_modul = config;
}

bool ABC_NamaModul::status_siap() const {
    return status_internal.read() == STATUS_SIAP;
}

// 5. Process implementation
void ABC_NamaModul::proses_utama() {
    // Reset condition
    if (!reset_n.read()) {
        status_internal.write(STATUS_RESET);
        data_keluar.write(0);
        return;
    }
    
    // Main logic - selalu siap produksi
    switch (status_internal.read()) {
        case STATUS_IDLE:
            // Logic untuk idle state
            break;
        case STATUS_ACTIVE:
            // Logic untuk active state
            break;
        default:
            SC_REPORT_ERROR("ABC_NamaModul", "Status tidak dikenal");
    }
}

// 6. Helper methods implementation
void ABC_NamaModul::validasi_parameter() {
    if (konfigurasi_modul.timeout < 1) {
        SC_REPORT_FATAL("ABC_NamaModul", "Timeout harus > 0");
    }
    
    if (konfigurasi_modul.buffer_size == 0) {
        SC_REPORT_FATAL("ABC_NamaModul", "Buffer size tidak boleh 0");
    }
}

void ABC_NamaModul::inisialisasi_hardware() {
    // Inisialisasi register dan hardware
    status_internal.write(STATUS_IDLE);
    data_keluar.write(0);
}
```

### Aturan Implementasi Mutlak
- **Include header sendiri pertama** untuk mendeteksi missing dependencies
- **Semua method harus diimplementasi** - tidak ada stub/TODO
- **Validasi parameter di constructor** dan public methods
- **Reset condition wajib** di semua SC_METHOD
- **Error handling dengan SC_REPORT_**
- **Tidak ada magic numbers** - gunakan konstanta bernama
- **Implementasi state machine** jika diperlukan harus lengkap
- **Resource cleanup** di destructor jika ada alokasi manual

### Template Error Handling
```cpp
// Validasi input yang konsisten
void ABC_NamaModul::metode_publik(int parameter) {
    if (parameter < 0) {
        SC_REPORT_FATAL("ABC_NamaModul", "Parameter tidak boleh negatif");
    }
    
    if (parameter > NILAI_MAKSIMAL) {
        SC_REPORT_ERROR("ABC_NamaModul", "Parameter melebihi batas");
        return;
    }
    
    // Implementasi actual
}
```

---

**INGAT: Setiap baris kode harus siap deploy ke pabrik. Tidak ada toleransi untuk kode eksperimen, placeholder, atau tidak selesai. Semua modul harus dapat diintegrasikan tanpa kendala dan mengikuti prinsip SOLID.**
