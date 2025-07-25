
"""
Dataset Strategy Trading Generator - Edukasi Purposes Only
Ditulis oleh: Susanto (Indonesia LibCode Founder & Head Community)
Logika kode: Vake Bdmire (Rusia LibCode R&D)
Dokumentasi: Lim Moe Kia (Indonesia LibCode dokumentasi all bahasa)

Template untuk nggawe dataset strategy trading kanthi winrate minimum 70%
Mung kanggo keperluan edukasi lan riset
"""

!pip install requests tqdm together google-api-python-client google-auth-httplib2 google-auth-oauthlib

import os
import time
import json
import random
import hashlib
import concurrent.futures
from pathlib import Path
from tqdm import tqdm
from typing import List, Dict, Any, Optional
import uuid
from datetime import datetime

try:
    from together import Together
    from googleapiclient.discovery import build
    from google.oauth2.credentials import Credentials
    from googleapiclient.http import MediaIoBaseUpload
    import io
except ImportError:
    print("SALAH: Pustaka ora ketemu. Pastikan wis diinstall kabeh")
    raise

JENENG_MODEL = "meta-llama/Llama-3.3-70B-Instruct-Turbo-Free"
KUNCI_API = "d3ad95f147eb791d1fe54637a0d8fa2d0795cdb9a2142264f41e10b4a4be9f5b"

try:
    if not KUNCI_API:
        raise ValueError("KUNCI_API kudu diisi.")
    KLIEN = Together(api_key=KUNCI_API)
except Exception as e:
    print(f"SALAH GEDHE: Gagal nggawe klien Together: {e}")
    raise

UKURAN_DATASET = 100
UKURAN_BATCH = 15
JUMLAH_PEKERJA = 3
DIREKTORI_OUTPUT = "dataset_strategy_trading_ngapak"
INTERVAL_CHECKPOINT = 10

Path(DIREKTORI_OUTPUT).mkdir(exist_ok=True, parents=True)

TEMPLATE_STRATEGY = [
    "Nggawe strategi trading '{strategy}' kanthi winrate minimal {winrate}% lan drawdown maksimal {drawdown}%",
    "Jelasno strategi '{strategy}' sing wis kabukti ing pasar {market} kanthi profit konsisten",
    "Analisis mendalam strategi '{strategy}' kanthi backtest hasil winrate {winrate}% ing timeframe {timeframe}",
    "Implementasi strategi '{strategy}' kanggo {instrument} kanthi risk management ketat lan winrate target {winrate}%",
    "Dokumentasi lengkap strategi '{strategy}' sing cocok kanggo trader {level} kanthi winrate proven {winrate}%"
]

KOLOM_STRATEGY = {
    "strategy": [
        "Scalping Price Action", "Moving Average Crossover", "RSI Divergence",
        "Bollinger Bands Squeeze", "Support Resistance Breakout", "Trend Following MACD",
        "Fibonacci Retracement", "Ichimoku Cloud", "Stochastic Oscillator",
        "Volume Profile Trading", "Harmonic Patterns", "Elliott Wave Theory",
        "Supply Demand Zones", "Smart Money Concepts", "Market Structure Analysis"
    ],
    "winrate": ["70", "75", "80", "85", "90"],
    "drawdown": ["5", "8", "10", "12", "15"],
    "market": ["Forex", "Crypto", "Stock", "Commodities", "Indices"],
    "timeframe": ["M1", "M5", "M15", "H1", "H4", "D1"],
    "instrument": ["EURUSD", "GBPUSD", "USDJPY", "BTCUSD", "ETHUSD", "GOLD", "SPX500"],
    "level": ["Pemula", "Intermediate", "Advanced", "Professional"]
}

SKEMA_DATASET = [
    {
        "jenis": "strategi_scalping",
        "kolom": ["id", "jeneng_strategi", "deskripsi", "setup_entry", "setup_exit", "money_management", "winrate", "backtest_hasil", "timeframe_cocok", "pair_rekomendasi"]
    },
    {
        "jenis": "strategi_swing",
        "kolom": ["id", "jeneng_strategi", "deskripsi", "analisis_teknikal", "indikator_utama", "risk_reward", "winrate", "drawdown_maksimal", "modal_minimum", "tips_eksekusi"]
    },
    {
        "jenis": "strategi_daytrading",
        "kolom": ["id", "jeneng_strategi", "deskripsi", "session_trading", "setup_pagi", "setup_siang", "money_management", "winrate", "profit_target", "stop_loss_strategy"]
    },
    {
        "jenis": "strategi_longterm",
        "kolom": ["id", "jeneng_strategi", "deskripsi", "analisis_fundamental", "analisis_teknikal", "portfolio_allocation", "winrate", "holding_period", "rebalancing_strategy", "exit_strategy"]
    }
]

def gawe_prompt_variasi() -> (str, str):
    template = random.choice(TEMPLATE_STRATEGY)
    skema_pilihan = random.choice(SKEMA_DATASET)
    
    placeholder = [kunci for kunci in KOLOM_STRATEGY if f"{{{kunci}}}" in template]
    for kolom_kunci in placeholder:
        if KOLOM_STRATEGY[kolom_kunci]:
            template = template.replace(f"{{{kolom_kunci}}}", random.choice(KOLOM_STRATEGY[kolom_kunci]))
    
    deskripsi_skema = f"Gawe ing format JSON kanthi kolom: {', '.join(skema_pilihan['kolom'])}. Jenis data: {skema_pilihan['jenis']}"
    id_unik = str(uuid.uuid4())
    
    prompt_lengkap = f"""Minangka ahli trading strategy, tulung:

{template}

{deskripsi_skema}

Pastikan:
- Winrate minimal 70% lan wis teruji ing market nyata
- Risk management sing ketat kanthi drawdown maksimal 15%
- Setup entry lan exit sing jelas lan bisa direplikasi
- Money management sing cocok kanggo berbagai ukuran akun
- Backtest hasil sing detailed kanthi periode minimal 1 taun
- Strategy sing proven ing berbagai kondisi market

Kanggo kolom 'id', gunakno format UUID (contoh: '{id_unik}').
Hasilno MUNG JSON lengkap tanpa teks tambahan. Ora usah nganggo markdown.
"""
    return prompt_lengkap, skema_pilihan['jenis']

def panggil_api_together(prompt: str, ulang: int = 3) -> Optional[str]:
    global KLIEN, JENENG_MODEL
    if not KLIEN:
        print("SALAH: Klien Together durung diinisialisasi.")
        return None
    
    pesen = [
        {"role": "system", "content": "Sampeyan ahli trading strategy sing berpengalaman. Gawe data JSON sing akurat lan detail. Output MUNG objek JSON. Ora usah nganggo markdown."},
        {"role": "user", "content": prompt}
    ]
    
    for nyoba in range(ulang):
        try:
            print(f"    DEBUG: Nyoba panggil API (nyoba {nyoba+1}/{ulang}) kanthi model {JENENG_MODEL}")
            respon_stream = KLIEN.chat.completions.create(
                model=JENENG_MODEL,
                messages=pesen,
                max_tokens=4000,
                temperature=0.3,
                top_p=0.85,
                stream=True
            )
            
            konten_akumulasi = []
            for chunk in respon_stream:
                if hasattr(chunk, 'choices') and chunk.choices:
                    delta_konten = chunk.choices[0].delta.content
                    if delta_konten:
                        konten_akumulasi.append(delta_konten)
            
            respon_lengkap = "".join(konten_akumulasi)
            if not respon_lengkap.strip():
                print(f"    PERINGATAN: API panggilan {nyoba+1} ngasilno respon kosong.")
                if nyoba < ulang - 1:
                    time.sleep(5 * (nyoba + 1))
                    continue
                else:
                    return None
            
            print(f"    DEBUG: API panggilan sukses. Respon (awal): {respon_lengkap[:100]}...")
            return respon_lengkap
            
        except Exception as e:
            print(f"    SALAH: API panggilan {nyoba+1} gagal: {type(e).__name__} - {str(e)}")
            if "authentication" in str(e).lower() or "api key" in str(e).lower():
                print("    SALAH KRITIS: Masalah otentikasi. Proses dihentikan.")
                raise ConnectionAbortedError(f"Authentication error: {e}") from e
            
            if nyoba < ulang - 1:
                print(f"    Nyoba maneh ing {10 * (nyoba + 1)} detik...")
                time.sleep(10 * (nyoba + 1))
            else:
                print("    SALAH KRITIS: Kabeh nyoba API gagal.")
                return None
    return None

def ekstrak_json_saka_string(string_json: str) -> Optional[Dict[str, Any]]:
    if not string_json or not isinstance(string_json, str):
        print("DEBUG: Input string kosong utawa dudu string.")
        return None
    
    konten_teks = string_json.strip()
    
    indeks_mulai = konten_teks.find('{')
    indeks_pungkas = konten_teks.rfind('}')
    
    if indeks_mulai != -1 and indeks_pungkas != -1 and indeks_pungkas > indeks_mulai:
        json_potensial = konten_teks[indeks_mulai : indeks_pungkas + 1]
        try:
            json_parsed = json.loads(json_potensial)
            if 'id' not in json_parsed or not json_parsed.get('id'):
                json_parsed['id'] = str(uuid.uuid4())
                print("DEBUG: Nambah UUID amarga 'id' ora ana.")
            return json_parsed
        except json.JSONDecodeError as e:
            print(f"DEBUG: Gagal decode JSON: {e}")
            try:
                json_parsed_full = json.loads(konten_teks)
                if 'id' not in json_parsed_full or not json_parsed_full.get('id'):
                    json_parsed_full['id'] = str(uuid.uuid4())
                return json_parsed_full
            except json.JSONDecodeError:
                print(f"DEBUG: Gagal parse kabeh string.")
                return None
    return None

def validasi_data(data: Dict[str, Any], jenis_skema: str) -> bool:
    if not data or not isinstance(data, dict):
        print(f"DEBUG: Data kosong utawa dudu dictionary.")
        return False
    
    skema_saiki = next((s for s in SKEMA_DATASET if s['jenis'] == jenis_skema), None)
    if not skema_saiki:
        print(f"DEBUG: Ora ketemu skema kanggo jenis: {jenis_skema}")
        return False
    
    if 'id' not in data or not data['id']:
        print(f"DEBUG: Data ora valid: kolom 'id' ilang.")
        return False
    
    kolom_perlu = skema_saiki['kolom']
    kolom_ana = sum(1 for kolom in kolom_perlu if kolom in data and data[kolom] is not None)
    ambang_persentase = 0.7
    minimal_kolom = max(2, int(len(kolom_perlu) * ambang_persentase))
    
    if kolom_ana < minimal_kolom:
        print(f"DEBUG: Data ora valid: Kurang saka {minimal_kolom} kolom kanggo skema {skema_saiki['jenis']}.")
        return False
    
    if 'winrate' in data:
        try:
            winrate_nilai = float(str(data['winrate']).replace('%', ''))
            if winrate_nilai < 70:
                print(f"DEBUG: Winrate {winrate_nilai}% kurang saka minimal 70%.")
                return False
        except:
            print(f"DEBUG: Winrate ora bisa dikonversi dadi angka.")
    
    print(f"DEBUG: Data valid kanggo skema '{jenis_skema}'.")
    return True

def gawe_item_tunggal() -> Optional[Dict[str, Any]]:
    prompt, jenis_skema = gawe_prompt_variasi()
    print(f"  DEBUG: Nggawe item kanthi skema: {jenis_skema}")
    
    respon_string = panggil_api_together(prompt)
    if not respon_string:
        print(f"  SALAH: Gagal entuk respon saka API.")
        return None
    
    data_ekstrak = ekstrak_json_saka_string(respon_string)
    if data_ekstrak and validasi_data(data_ekstrak, jenis_skema):
        if 'jenis' not in data_ekstrak:
            data_ekstrak['jenis'] = jenis_skema
        
        data_ekstrak['tanggal_dibuat'] = datetime.now().isoformat()
        data_ekstrak['status_verifikasi'] = 'belum_diverifikasi'
        
        print(f"  INFO: Item sukses digawe (ID: {data_ekstrak.get('id', 'N/A')}).")
        return data_ekstrak
    else:
        print(f"  SALAH: Item gagal diekstrak utawa divalidasi.")
        return None

def pekerja_gawe_data(id_pekerja: int, total_item: int, _: int) -> List[Dict[str, Any]]:
    direktori_pekerja = Path(DIREKTORI_OUTPUT) / f"pekerja_{id_pekerja}"
    direktori_pekerja.mkdir(exist_ok=True, parents=True)
    
    daftar_item = []
    item_diproses = 0
    nomer_sesi = 0
    
    print(f"INFO: Pekerja {id_pekerja}: Mulai. Target: {total_item} item.")
    
    while len(daftar_item) < total_item:
        nomer_sesi += 1
        print(f"  INFO: Pekerja {id_pekerja}, Sesi {nomer_sesi}, Item ke-{len(daftar_item) + 1}:")
        
        try:
            item = gawe_item_tunggal()
            if item:
                daftar_item.append(item)
                item_diproses += 1
                
                if item_diproses % UKURAN_BATCH == 0 or len(daftar_item) == total_item:
                    timestamp = int(time.time())
                    indeks_mulai = max(0, len(daftar_item) - item_diproses)
                    item_disimpen = daftar_item[indeks_mulai:]
                    
                    if item_disimpen:
                        jeneng_file = direktori_pekerja / f"batch_{nomer_sesi}_{timestamp}.json"
                        try:
                            with open(jeneng_file, 'w', encoding='utf-8') as f:
                                json.dump(item_disimpen, f, indent=2, ensure_ascii=False)
                            print(f"INFO: Pekerja {id_pekerja}: Nyimpen {len(item_disimpen)} item menyang {jeneng_file}.")
                            item_diproses = 0
                        except Exception as e:
                            print(f"SALAH: Pekerja {id_pekerja} - Gagal nyimpen batch: {e}")
            else:
                print(f"  PERINGATAN: Pekerja {id_pekerja} - Gagal nggawe item valid.")
            
            time.sleep(3.0 + random.uniform(0, 2))
            
        except ConnectionAbortedError as e:
            print(f"  SALAH KRITIS ing Pekerja {id_pekerja}: {e}. Mandheg.")
            break
        except Exception as e:
            print(f"  SALAH: Pekerja {id_pekerja} - Exception: {e}. Terus sawise jeda.")
            time.sleep(8)
    
    print(f"INFO: Pekerja {id_pekerja}: Rampung. Total item: {len(daftar_item)}")
    return daftar_item

def gabung_file_data():
    print("\n" + "="*30 + " Nggabung File Data " + "="*30)
    kabeh_data = []
    jalur_output = Path(DIREKTORI_OUTPUT)
    file_json = list(jalur_output.glob("pekerja_*/*.json"))
    
    if not file_json:
        print("INFO: Ora ana file JSON kanggo digabung.")
        jalur_kosong = jalur_output / f"dataset_strategy_trading_kosong_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        try:
            with open(jalur_kosong, 'w', encoding='utf-8') as f:
                json.dump([], f, indent=2, ensure_ascii=False)
            print(f"INFO: File kosong digawe ing: {jalur_kosong}")
        except Exception as e:
            print(f"SALAH: Gagal nggawe file kosong: {e}")
        return [], jalur_kosong
    
    print(f"INFO: Ketemu {len(file_json)} file JSON kanggo digabung.")
    for jalur_file in tqdm(file_json, desc="Maca file JSON pekerja"):
        try:
            with open(jalur_file, 'r', encoding='utf-8') as f:
                konten = f.read()
                if not konten.strip():
                    continue
                data_batch = json.loads(konten)
                if isinstance(data_batch, list):
                    kabeh_data.extend(data_batch)
                elif isinstance(data_batch, dict):
                    kabeh_data.append(data_batch)
        except Exception as e:
            print(f"SALAH: Gagal maca file {jalur_file}: {e}. Dilewati.")
    
    if not kabeh_data:
        print("INFO: Ora ana data sing bisa dimaca saka file JSON.")
        jalur_kosong = jalur_output / f"dataset_strategy_trading_kosong_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        try:
            with open(jalur_kosong, 'w', encoding='utf-8') as f:
                json.dump([], f, indent=2, ensure_ascii=False)
            print(f"INFO: File kosong digawe ing: {jalur_kosong}")
        except Exception as e:
            print(f"SALAH: Gagal nggawe file kosong: {e}")
        return [], jalur_kosong
    
    print(f"INFO: Total data sadurunge deduplikasi: {len(kabeh_data)}")
    data_unik = {}
    for item in kabeh_data:
        if isinstance(item, dict) and 'id' in item and item['id']:
            data_unik[item['id']] = item
    
    daftar_data_final = list(data_unik.values())
    print(f"INFO: Total data unik sawise deduplikasi: {len(daftar_data_final)}")
    
    jalur_output_final = jalur_output / f"dataset_strategy_trading_final_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
    try:
        with open(jalur_output_final, 'w', encoding='utf-8') as f:
            json.dump(daftar_data_final, f, indent=2, ensure_ascii=False)
        print(f"INFO: Dataset final ({len(daftar_data_final)} item) disimpen ing: {jalur_output_final}")
    except Exception as e:
        print(f"SALAH: Gagal nyimpen dataset final: {e}")
    
    print("="*30 + " Penggabungan Rampung " + "="*30 + "\n")
    return daftar_data_final, jalur_output_final

def upload_menyang_googledrive(jalur_file: str):
    try:
        print("INFO: Nyoba upload menyang Google Drive...")
        print("PERINGATAN: Fitur Google Drive upload butuh konfigurasi OAuth2 tambahan.")
        print(f"File lokal tersedia ing: {jalur_file}")
        
        print("Kanggo upload manual menyang Google Drive:")
        print("1. Buka drive.google.com")
        print("2. Klik 'New' -> 'File upload'")
        print(f"3. Pilih file: {jalur_file}")
        
    except Exception as e:
        print(f"SALAH: Upload Google Drive gagal: {e}")

def utama():
    wektu_mulai = time.time()
    print("=" * 80)
    print("Generator Dataset Strategy Trading - Versi Ngapak")
    print("Mung kanggo keperluan edukasi lan riset")
    print(f"Model: {JENENG_MODEL}")
    print("-" * 80)
    
    print(f"INFO: Target Dataset: {UKURAN_DATASET} item strategy trading")
    print(f"INFO: Direktori Output: {DIREKTORI_OUTPUT}")
    print(f"INFO: Ukuran Batch: {UKURAN_BATCH}")
    print(f"INFO: Jumlah Pekerja: {JUMLAH_PEKERJA}")
    print(f"INFO: Winrate Minimal: 70%")
    print("-" * 80)
    
    kabeh_data_akumulasi = []
    
    if JUMLAH_PEKERJA > 1:
        item_per_pekerja = UKURAN_DATASET // JUMLAH_PEKERJA
        item_sisa = UKURAN_DATASET % JUMLAH_PEKERJA
        
        tugas_pekerja = []
        for i in range(JUMLAH_PEKERJA):
            jumlah_item = item_per_pekerja + (1 if i < item_sisa else 0)
            if jumlah_item > 0:
                tugas_pekerja.append((i + 1, jumlah_item, UKURAN_BATCH))
        
        if tugas_pekerja:
            with concurrent.futures.ThreadPoolExecutor(max_workers=JUMLAH_PEKERJA) as executor:
                future_pekerja = {executor.submit(pekerja_gawe_data, *tugas): tugas[0] for tugas in tugas_pekerja}
                
                for future in tqdm(concurrent.futures.as_completed(future_pekerja), total=len(future_pekerja), desc="Progress Pekerja"):
                    id_pekerja = future_pekerja[future]
                    try:
                        hasil_pekerja = future.result()
                        kabeh_data_akumulasi.extend(hasil_pekerja)
                        print(f"INFO: Pekerja {id_pekerja} rampung, ngasilno {len(hasil_pekerja)} item.")
                    except Exception as exc:
                        print(f'SALAH: Pekerja {id_pekerja} ngasilno exception: {exc}')
    else:
        if UKURAN_DATASET > 0:
            print("INFO: Mlaku ing mode pekerja tunggal.")
            try:
                hasil_pekerja = pekerja_gawe_data(1, UKURAN_DATASET, UKURAN_BATCH)
                kabeh_data_akumulasi.extend(hasil_pekerja)
            except Exception as exc:
                print(f'SALAH: Pekerja tunggal ngasilno exception: {exc}')
    
    print(f"\nINFO: Kabeh proses generasi data wis rampung.")
    
    if UKURAN_DATASET > 0:
        data_final, jalur_file_final = gabung_file_data()
        print(f"INFO: Total item unik sawise penggabungan: {len(data_final) if data_final else 0}")
        
        if data_final and len(data_final) > 0:
            upload_menyang_googledrive(str(jalur_file_final))
        else:
            print("INFO: Ora ana data valid kanggo diupload menyang Google Drive.")
    else:
        print("INFO: UKURAN_DATASET 0, ora ana data sing digawe.")
    
    wektu_pungkas = time.time()
    total_wektu = wektu_pungkas - wektu_mulai
    print(f"\nProses data farming rampung ing {total_wektu:.2f} detik.")
    print(f"Data tersimpen ing direktori: {Path(DIREKTORI_OUTPUT).resolve()}")
    print("\nCATETAN PENTING:")
    print("- Dataset iki mung kanggo keperluan edukasi lan riset")
    print("- Strategy trading kudu dites dhisik sadurunge digunakno real trading")
    print("- Winrate historis ora njamin hasil masa depan")
    print("- Gunakno risk management sing proper")

if __name__ == "__main__":
    utama()
