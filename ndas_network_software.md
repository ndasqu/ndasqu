# PANDUAN PENGEMBANGAN MODUL SERTA ATURAN YANG WAJIB DIPATUHI OLEH SELURUH MITRA DIINDONESIA

## 🏗️ ARSITEKTUR DIRECTORY

```
nambang_ndas/
├── src/
│   ├── sha256/
│   │   ├── nambang_sha256.py
│   │   └── __init__.py
│   │
│   ├── midstate/
│   │   ├── nambang_midstate.py
│   │   └── __init__.py
│   │
│   ├── headergen/
│   │   ├── nambang_headergen.py
│   │   └── __init__.py
│   │
│   ├── coinbase/
│   │   ├── nambang_coinbase.py
│   │   └── __init__.py
│   │
│   ├── merkle/
│   │   ├── nambang_merkle.py
│   │   └── __init__.py
│   │
│   ├── rpc/
│   │   ├── nambang_rpc_client.py
│   │   └── __init__.py
│   │
│   ├── gbt/
│   │   ├── nambang_gbt_parser.py
│   │   └── __init__.py
│   │
│   ├── submit/
│   │   ├── nambang_block_submit.py
│   │   └── __init__.py
│   │
│   ├── util/
│   │   ├── nambang_util.py
│   │   └── __init__.py
│   │
│   ├── types/
│   │   ├── nambang_types.py
│   │   └── __init__.py
│   │
│   ├── nambang/
│   │   ├── nambang_main.py
│   │   ├── nambang_config.py
│   │   └── __init__.py
│   │
│   ├── communication/
│   │   ├── nambang_ip_comm.py
│   │   ├── nambang_usb_comm.py
│   │   └── __init__.py
│   │
│   └── utek_integration/
│       ├── nambang_utek_client.py
│       ├── nambang_utek_auth.py
│       └── __init__.py
│
├── build/
│   └── bin/
│       └── nambang_ndas.exe
│
├── config/
│   └── nambang_config.json
│
├── requirements.txt
├── setup.py
├── Makefile
└── README.md
```

## 🔒 STANDAR PENGKODEAN PYTHON - ENTERPRISE ASIC NDAS GRADE

### 📜 PEP COMPLIANCE MATRIX (WAJIB)

| Standard | Level | Implementasi untuk ASIC NDAS |
|----------|-------|--------------------------------|
| PEP 8: Style Guide | **MANDATORY** | Konsistensi code untuk team development |
| PEP 257: Docstring | **MANDATORY** | Dokumentasi untuk maintenance bahasa indonesia |
| PEP 484: Type Hints | **MANDATORY** | Safety untuk ASIC communication |
| PEP 526: Variable Annotations | **MANDATORY** | Clarity untuk ndas parameters |
| PEP 563: Postponed Evaluation | **MANDATORY** | Performance untuk real-time ndas |
| PEP 585: Generic Types | **RECOMMENDED** | Clean code untuk collections |
| PEP 604: Union Types (X \| Y) | **RECOMMENDED** | Modern Python syntax |
| PEP 613: Type Aliases | **RECOMMENDED** | Readable type definitions |

### 📜 STANDAR KEAMANAN KODE (SECURITY - WAJIB)

#### 1. **UTEK.V.26 INTEGRATION SECURITY**
- **WAJIB**: Type hints untuk semua fungsi RPC
- **WAJIB**: Input validation untuk ASIC communication data
- **WAJIB**: Secure credential management untuk UTEK authentication
- **WAJIB**: Path validation untuk library loading
- **LARANGAN**: `eval()`, `exec()`, `pickle.load()` dari untrusted sources
- **LARANGAN**: Hardcoded credentials dalam source code

#### 2. **NDAS OPERATION SECURITY**
- **WAJIB**: Validation untuk block templates dari RPC
- **WAJIB**: Sanitization untuk ASIC response data
- **WAJIB**: Secure handling untuk private keys (jika ada)
- **LARANGAN**: Plain text credential storage
- **LARANGAN**: Unvalidated data dari network sources

### 📜 STANDAR THREAD SAFETY & CONCURRENCY (WAJIB)

#### 1. **ASIC COMMUNICATION CONCURRENCY**
- **WAJIB**: `threading.Lock()` untuk shared ndas state
- **WAJIB**: `asyncio` untuk concurrent ASIC communication
- **WAJIB**: `queue.Queue` untuk job distribution ke multiple ASIC
- **WAJIB**: Context managers untuk network resource management
- **LARANGAN**: Race conditions dalam ndas job assignment
- **LARANGAN**: Global mutable state tanpa synchronization

#### 2. **RPC CONNECTION MANAGEMENT**
- **WAJIB**: Connection pooling untuk UTEK RPC calls
- **WAJIB**: Thread-safe credential sharing
- **WAJIB**: Atomic operations untuk block submission
- **LARANGAN**: Concurrent access ke single RPC connection
- **LARANGAN**: Blocking operations pada main ndas thread

### 📜 STANDAR PERFORMANCE & RELIABILITY (WAJIB)

#### 1. **NDAS PERFORMANCE REQUIREMENTS**
- **WAJIB**: Exception handling untuk hardware failures
- **WAJIB**: Connection retry mechanism untuk ASIC communication
- **WAJIB**: Timeout handling untuk network operations
- **WAJIB**: Resource cleanup untuk failed operations
- **LARANGAN**: Bare `except:` clauses
- **LARANGAN**: Memory leaks dari unclosed connections
- **LARANGAN**: Bottleneck overhead dalam critical paths

#### 2. **OPERATIONAL RELIABILITY**
- **WAJIB**: Structured logging untuk troubleshooting
- **WAJIB**: Health checks untuk ASIC connectivity
- **WAJIB**: Graceful degradation pada partial failures
- **WAJIB**: Automatic recovery mechanisms
- **LARANGAN**: Silent failures dalam ndas operations
- **LARANGAN**: Unhandled exceptions yang crash nambang

### 📜 STANDAR MAINTAINABILITY (WAJIB)

#### 1. **CODE STRUCTURE UNTUK ASIC NDAS**
- **WAJIB**: Docstrings untuk public ndas methods bahasa indonesia
- **WAJIB**: Return type annotations untuk all functions
- **WAJIB**: Configuration validation dengan schema
- **WAJIB**: Single responsibility per module
- **LARANGAN**: `print()` statements dalam production code
- **LARANGAN**: Magic numbers untuk ndas parameters
- **LARANGAN**: TODO, simulasi, dummy, placeholder, fake kode, konseptual

#### 2. **MODULARITY & INTERFACE DESIGN**
- **WAJIB**: Abstract base classes untuk communication interfaces
- **WAJIB**: Dependency injection untuk testable code
- **WAJIB**: Interface segregation untuk ASIC types
- **WAJIB**: Clean separation antara RPC dan hardware communication
- **LARANGAN**: Tight coupling antara modules
- **LARANGAN**: Functions dengan lebih dari 25 lines

### 📜 HEADER INFORMASI STANDARD (WAJIB - SIMPLIFIED)

```python
"""
@file: [nama_file].py
@coder: Susanto - Libcode Indonesia
@dependency: [Dependencies]
@function: [Core function]
"""
```

### 📜 NAMESPACE & NAMING CONVENTION (WAJIB)

#### 1. **ASIC NDAS PACKAGE STRUCTURE**
```python
# NDAS-specific package hierarchy
nambang.ndas.sha256        # SHA256 untuk block hashing
nambang.ndas.midstate      # Midstate optimization untuk ASIC
nambang.ndas.headergen     # Block header generation
nambang.ndas.coinbase      # Coinbase transaction builder
nambang.ndas.merkle        # Merkle tree untuk block validation
nambang.ndas.rpc           # UTEK RPC client
nambang.ndas.gbt           # GetBlockTemplate parser
nambang.ndas.submit        # Block submission ke network
nambang.ndas.util          # NDAS utilities
nambang.ndas.comm          # ASIC communication layer
nambang.ndas.utek          # UTEK.V.26 integration
```

#### 2. **NAMING CONVENTION UNTUK NDAS**
- **NDAS Variables**: snake_case (`hash_target`, `block_height`, `asic_count`)
- **NDAS Functions**: snake_case (`calculate_midstate`, `submit_solution`, `connect_asic`)
- **NDAS Classes**: PascalCase (`NambangNdas`, `AsicController`, `BlockTemplate`)
- **NDAS Constants**: UPPER_SNAKE_CASE (`DEFAULT_DIFFICULTY`, `MAX_ASIC_COUNT`)
- **ASIC Files**: snake_case (`nambang_asic_comm.py`, `asic_ip_manager.py`)
- **Private Methods**: _single_underscore (`_validate_asic_response`)

### 📜 ERROR HANDLING UNTUK ASIC NDAS (WAJIB)

#### 1. **NDAS-SPECIFIC EXCEPTION HIERARCHY**
```python
class NambangNdasError(Exception):
    """Base exception untuk semua NAMBANG ndas errors"""
    pass

class AsicCommunicationError(NambangNdasError):
    """Error untuk ASIC hardware communication"""
    pass

class UtekIntegrationError(NambangNdasError):
    """Error untuk UTEK.V.26 integration issues"""
    pass

class NdasJobError(NambangNdasError):
    """Error untuk ndas job processing"""
    pass

class BlockSubmissionError(NambangNdasError):
    """Error untuk block submission failures"""
    pass

class NetworkConnectionError(NambangNdasError):
    """Error untuk network connectivity issues"""
    pass
```

#### 2. **NDAS ERROR HANDLING PATTERN**
```python
# Required pattern untuk ndas operations
try:
    ndas_result = execute_ndas_job()
    return ndas_result
except AsicCommunicationError as e:
    logger.error(f"ASIC communication failed: {e}")
    # Attempt ASIC reconnection
    self._reconnect_asic()
    raise
except UtekIntegrationError as e:
    logger.error(f"UTEK integration error: {e}")
    # Attempt RPC reconnection
    self._reconnect_utek()
    raise
except Exception as e:
    logger.error(f"Unexpected ndas error: {e}")
    raise NambangNdasError(f"NDAS operation failed: {e}")
```

### 📜 LOGGING UNTUK ASIC NDAS (WAJIB)

#### 1. **NDAS-SPECIFIC LOGGING**
```python
import logging
import json
from datetime import datetime

class NambangNdasFormatter(logging.Formatter):
    def format(self, record):
        return json.dumps({
            'timestamp': datetime.utcnow().isoformat(),
            'level': record.levelname,
            'component': record.module,
            'message': record.getMessage(),
            'asic_id': getattr(record, 'asic_id', None),
            'ndas_job_id': getattr(record, 'job_id', None),
            'block_height': getattr(record, 'block_height', None)
        })

# NDAS logging configuration
logging.basicConfig(
    level=logging.INFO,
    handlers=[
        logging.FileHandler('/var/log/nambang_ndas.log'),
        logging.StreamHandler()
    ]
)
```

#### 2. **MONITORING UNTUK NDAS OPERATION**
- **WAJIB**: ASIC health status monitoring
- **WAJIB**: NDAS job completion tracking
- **WAJIB**: Hash rate monitoring per ASIC
- **WAJIB**: Network latency tracking untuk RPC
- **LARANGAN**: Logging sensitive data (private keys, credentials)
- **LARANGAN**: Excessive logging yang impact performance
- **LARANGAN**: Bottleneck overhead dalam monitoring

### 📜 MEMORY MANAGEMENT UNTUK NDAS (WAJIB)

#### 1. **ASIC COMMUNICATION MEMORY**
- **WAJIB**: Context managers untuk ASIC connections
- **WAJIB**: Buffer management untuk large ndas data
- **WAJIB**: Memory profiling untuk continuous operations
- **WAJIB**: Garbage collection monitoring untuk long-running process
- **LARANGAN**: Memory leaks dari unclosed ASIC connections
- **LARANGAN**: Excessive buffering untuk ndas jobs
- **LARANGAN**: Bottleneck overhead dalam memory management

#### 2. **RESOURCE MANAGEMENT PATTERN UNTUK NDAS**
```python
from contextlib import contextmanager

@contextmanager
def asic_connection_manager(asic_config):
    connection = None
    try:
        connection = establish_asic_connection(asic_config)
        yield connection
    finally:
        if connection:
            safely_close_asic_connection(connection)
```

### 📜 UTEK.V.26 INTEGRATION STANDARD (CRITICAL)

#### 1. **UTEK LIBRARY INTEGRATION REQUIREMENTS**
- **MUTLAK WAJIB**: Dynamic loading dari `libutek_v26.so`
- **MUTLAK WAJIB**: Secure credential management dari factory config
- **MUTLAK WAJIB**: License validation dengan factory token
- **MUTLAK WAJIB**: Connection pooling untuk RPC performance
- **MUTLAK WAJIB**: Retry mechanism untuk network failures
- **LARANGAN MUTLAK**: Credential hardcoding
- **LARANGAN MUTLAK**: Unencrypted credential transmission
- **LARANGAN MUTLAK**: Bottleneck overhead dalam RPC calls

#### 2. **UTEK AUTHENTICATION FLOW (CRITICAL)**
- **MUTLAK WAJIB**: Load metadata dari `/opt/utek_v26_rpc_lib/metadata.json`
- **MUTLAK WAJIB**: Validate license UUID dengan factory token
- **MUTLAK WAJIB**: Secure credential retrieval dari `/etc/utek_v26/utek_v26.conf`
- **MUTLAK WAJIB**: Connection retry dengan exponential backoff
- **LARANGAN MUTLAK**: Authentication bypass mechanisms
- **LARANGAN MUTLAK**: Plain text credential storage

### 📜 TESTING STANDARD UNTUK ASIC NDAS (RECOMMENDED)

#### 1. **UNIT TESTING UNTUK NDAS COMPONENTS**
```python
import pytest
from unittest.mock import Mock, patch

class TestNambangNdas:
    def test_asic_communication_success(self):
        # Test successful ASIC communication
        pass
    
    def test_asic_connection_failure(self):
        # Test ASIC connection failure handling
        pass
    
    def test_ndas_job_processing(self):
        # Test ndas job creation dan distribution
        pass
    
    @patch('nambang.ndas.utek.UTEKClient')
    def test_utek_integration(self, mock_utek):
        # Test UTEK.V.26 integration
        pass
```

#### 2. **INTEGRATION TESTING UNTUK ENTERPRISE**
- **RECOMMENDED**: Hardware integration testing dengan mock ASIC
- **RECOMMENDED**: UTEK integration testing dengan test credentials
- **RECOMMENDED**: Performance testing untuk ndas throughput
- **RECOMMENDED**: Stress testing untuk continuous operation
- **LARANGAN**: TODO, simulasi, dummy, placeholder tests
- **LARANGAN**: Bottleneck overhead dalam test execution

## 🎯 MODUL FUNCTIONALITY MAPPING

### 1. **SHA256 MODULE**
- **Input**: bytes data untuk hashing
- **Output**: 32-byte hash digest
- **Fungsi**: Implementasi SHA-256 dengan Python optimization
- **Target**: Hardware-accelerated computation

### 2. **MIDSTATE MODULE**
- **Input**: Bitcoin block header components
- **Output**: Midstate hash untuk optimization
- **Fungsi**: Pre-computation untuk ndas efficiency
- **Target**: 50% reduction dalam computation overhead

### 3. **HEADER GENERATOR MODULE**
- **Input**: Block template parameters
- **Output**: Complete 80-byte block header
- **Fungsi**: Assembling header untuk ndas process
- **Target**: Real-time header generation

### 4. **COINBASE MODULE**
- **Input**: NDAS address, reward, extra nonce
- **Output**: Valid coinbase transaction
- **Fungsi**: Generate coinbase transaction untuk block
- **Target**: BIP compliant transaction generation

### 5. **MERKLE MODULE**
- **Input**: List of transaction hashes
- **Output**: 32-byte merkle root
- **Fungsi**: Efficient merkle tree construction
- **Target**: O(n) complexity untuk tree building

### 6. **RPC CLIENT MODULE**
- **Input**: RPC method calls
- **Output**: JSON responses dari Bitcoin node
- **Fungsi**: UTEK.V.26 integration layer
- **Target**: Secure authenticated communication

### 7. **GBT PARSER MODULE**
- **Input**: JSON block template dari RPC
- **Output**: Structured ndas parameters
- **Fungsi**: Parse dan validate block template
- **Target**: BIP 22 dan BIP 23 compliance

### 8. **BLOCK SUBMIT MODULE**
- **Input**: Solved block dengan valid nonce
- **Output**: Submission status dan confirmation
- **Fungsi**: Submit blocks ke network via UTEK
- **Target**: Fast and reliable block submission

### 9. **COMMUNICATION MODULE**
- **Input**: NDAS jobs dan hardware commands
- **Output**: Results dari ASIC hardware
- **Fungsi**: Bridge antara software dan hardware
- **Target**: Low-latency bidirectional communication

### 10. **UTEK INTEGRATION MODULE**
- **Input**: UTEK credentials dan configuration
- **Output**: Authenticated RPC connection
- **Fungsi**: Secure integration dengan UTEK.V.26
- **Target**: Enterprise-grade Bitcoin node access

## 📋 DEPENDENCY MATRIX PYTHON

| Modul | Dependencies | UTEK Integration | Performance Target |
|-------|-------------|------------------|-------------------|
| SHA256 | hashlib, util | No | Hardware accelerated |
| Midstate | sha256, util | No | 50% computation reduction |
| Header Gen | coinbase, merkle, util | No | Real-time generation |
| Coinbase | util, types | No | BIP compliant |
| Merkle | hashlib, util | No | O(n) complexity |
| RPC Client | utek_integration | **YES** | Secure communication |
| GBT Parser | rpc, util | Via RPC | BIP 22/23 compliant |
| Block Submit | rpc, util | Via RPC | Fast submission |
| Communication | asyncio, util | No | Low latency |
| UTEK Integration | ctypes, json | **CORE** | Enterprise grade |

## 🔧 UTEK.V.26 INTEGRATION EXAMPLE

```python
"""
@file: nambang_utek_client.py
@coder: Susanto - Libcode Indonesia
@dependency: ctypes, json, threading
@function: Secure UTEK.V.26 RPC integration
"""

import ctypes
import json
import hashlib
import base64
import threading
from typing import Dict, Any, Optional
from dataclasses import dataclass
from pathlib import Path

@dataclass
class UTEKCredentials:
    username: str
    password: str
    license_uuid: str
    factory_token: str
    rpc_port: int = 8332

class NambangUTEKClient:
    """
    Secure client untuk UTEK.V.26 RPC integration
    Implements factory requirements untuk Bitcoin node access
    """
    
    def __init__(self, config_path: str = '/etc/utek_v26/utek_v26.conf'):
        self._lock = threading.Lock()
        self._connection_pool = {}
        self._credentials = None
        self._library = None
        self._load_utek_library()
        self._load_credentials(config_path)
        self._validate_license()
    
    def get_block_template(self) -> Dict[str, Any]:
        """Get block template untuk ndas"""
        with self._lock:
            return self._make_rpc_call('getblocktemplate', [])
    
    def submit_block(self, block_hex: str) -> Dict[str, Any]:
        """Submit solved block ke network"""
        with self._lock:
            return self._make_rpc_call('submitblock', [block_hex])
```

## 🚀 DEPLOYMENT STRATEGY

### Python Package Structure:
```
nambang_ndas/
├── setup.py              # Package installation
├── requirements.txt       # Dependencies
├── config/
│   └── nambang_config.json  # Application configuration
├── src/                  # Source modules
└── tests/                # Test suite
```

### Integration Flow:
1. **Initialization** → Load UTEK credentials dari factory installation
2. **Authentication** → Validate license UUID dan factory token
3. **RPC Connection** → Establish secure connection ke UTEK.V.26
4. **NDAS Loop** → Get template → Generate work → Submit results
5. **Hardware Communication** → Send jobs via IP/USB ke ASIC
6. **Monitoring** → Health checks dan performance metrics

**🎯 KESIMPULAN**: Semua masalah yang ditimbulkan oleh kode buatan mitra, khususnya LibCode Indonesia akan berakhir tuntutan hukum.
