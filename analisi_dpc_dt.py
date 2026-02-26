#!/usr/bin/env python3
"""
Analisi del problema: pressure decay nel motore solid
Il problema è che dpc_dt potrebbe essere negativo, causando collasso di pressione
"""

import math

# Costanti
R_univ = 8.314462618  # J/(mol*K)

# Parametri SOLID
sol_p_c = 7.0e6  # Pa (chamber pressure)
sol_T_c = 3600.0  # K
sol_A_b = 80.0  # m^2 burn area
sol_A_t = 0.18  # m^2 throat area
sol_rho_p = 1800.0  # kg/m^3
sol_a = 0.00012
sol_n = 0.35
sol_M = 22.0 / 1000.0  # kg/mol
sol_V_c = 0.5  # m^3 chamber volume
sol_gamma = 1.22

# Calcolate c_star (formula dal codice)
R_spec_sol = R_univ / sol_M
term = 2.0 / (sol_gamma + 1.0)
exponent = (sol_gamma + 1.0) / (2.0 * (sol_gamma - 1.0))
c_star_sol = math.sqrt(R_univ * sol_T_c / sol_M) * (1.0 / sol_gamma) * (term ** exponent)

print("="*80)
print("ANALISI dpc_dt - SOLID ENGINE")
print("="*80)

print(f"\nParametri motore solid:")
print(f"  p_c = {sol_p_c/1e6:.1f} MPa")
print(f"  T_c = {sol_T_c} K")
print(f"  A_b (burn area) = {sol_A_b} m^2")
print(f"  A_t (throat area) = {sol_A_t} m^2")
print(f"  V_c (chamber volume) = {sol_V_c} m^3")
print(f"  c_star = {c_star_sol:.1f} m/s")

# Calcolo m_gen e m_noz
m_gen = sol_rho_p * sol_A_b * sol_a * (sol_p_c ** sol_n)
m_noz = (sol_p_c * sol_A_t) / c_star_sol

print(f"\nFlussi di massa:")
print(f"  m_gen (combustion) = ρ * A_b * a * p_c^n")
print(f"               = {sol_rho_p} * {sol_A_b} * {sol_a} * ({sol_p_c:.2e})^{sol_n}")
print(f"               = {m_gen:.1f} kg/s")
print(f"\n  m_noz (nozzle discharge) = p_c * A_t / c_star")
print(f"                        = {sol_p_c:.2e} * {sol_A_t} / {c_star_sol:.1f}")
print(f"                        = {m_noz:.1f} kg/s")

# Calcolo dpc_dt
dpc_dt = (R_spec_sol * sol_T_c / sol_V_c) * (m_gen - m_noz)
print(f"\ndpc_dt = (R_spec * T_c / V_c) * (m_gen - m_noz)")
print(f"       = ({R_spec_sol:.1f} * {sol_T_c} / {sol_V_c}) * ({m_gen:.1f} - {m_noz:.1f})")
print(f"       = {dpc_dt:.2e} Pa/s")
print(f"       = {dpc_dt/1e6:.3f} MPa/s")

# Tempo per crollo di pressione
if dpc_dt < 0:
    time_to_collapse = abs(sol_p_c / dpc_dt)
    print(f"\n⚠️  ATTENZIONE: dpc_dt è NEGATIVO!")
    print(f"    La pressione CALA a {abs(dpc_dt):.2e} Pa/s")
    print(f"    Tempo stimato per crollo a 0: {time_to_collapse:.3f} secondi")
    hypothesis_msg = f"Se il motore burns per più di {time_to_collapse:.1f} secondi, la pressione crollerà a zero!"
else:
    time_to_increase = 20e6 / dpc_dt
    print(f"\n✓ dpc_dt è positivo (pressione sale)")
    print(f"  Tempo per raggiungere 20 MPa: {time_to_increase:.3f} secondi")
    hypothesis_msg = "La pressione AUMENTA molto velocemente!"

print(f"\n{hypothesis_msg}")

print("\n" + "="*80)
print("DIAGNOSI")
print("="*80)

if m_gen > m_noz:
    excess = m_gen - m_noz
    print(f"""
✓ m_gen > m_noz: La pressione SALIRÀ di {dpc_dt:.2e} Pa/s
  Il motore accumula più carburante di quello che scarica.
  Questo è fisiologicamente strano per un rocket engine!
  
Possibile causa: Il parametro burn_rate_a è TOO SMALL
  - Valore attuale: {sol_a}
  - Questo causa una bassissima regression rate
  - Ma poi A_b è comunque grande (80 m^2) quindi m_gen è ancora ragionevole
  
Controllo: m_gen / m_noz = {m_gen/m_noz:.3f}
  - Se > 1: Accumulation regime (pressione sale)
  - Se < 1: Depletion regime (pressione cala)
  
CONCLUSIONE IMPORTANTE:
  - Per un motore solid a regime STAZIONARIO (pressione costante),
    bisogna che m_gen ≈ m_noz
  - Attualmente m_gen/m_noz = {m_gen/m_noz:.3f}
  - Questo significa il motore NON è in regime stazionario!
  - La camera si comporta come un "pressure tank" che si ricarica
""")
elif m_gen < m_noz:
    deficit = m_noz - m_gen
    print(f"""
✗ m_gen < m_noz: La pressione CALA di {abs(dpc_dt):.2e} Pa/s
  Il motore scarica più carburante di quello che combustione genera.
  
Possibile causa: burn_rate_a è troppo grande OPPURE area_b è troppo grande
  - Valore attuale a: {sol_a}
  - Valore attuale A_b: {sol_A_b} m^2
  
  Forse A_b = 80 m^2 è l'area TOTALE di combustione che include
  tutta la superficie del solido, ma il combustibile finisce molto velocemente?
  
CONCLUSIONE: La pressione CROLLERÀ in {time_to_collapse:.2f} secondi
  Se il razzo rimane a spinta per più di questo tempo,
  la spinta sparirà perché p_c → 0
""")
else:
    print("m_gen ≈ m_noz: Equilibrio perfetto (pressione costante)")

print("\n" + "="*80)
print("IPOTESI FINALE")
print("="*80)
print("""
Il motore SOLID probabilmente ha un problema di parametrizzazione:
- burn_rate_a è estratto dal JSON come 0.00012
- Forse l'unità è sbagliata? (Es. mm/(s*Pa^n) invece di m/(s*Pa^n))
- Oppure A_b = 80 m^2 è TOO LARGE per il carburante disponibile
- Oppure V_c = 0.5 m^3 è TOO SMALL

Raccomandazioni:
1. Verificare le unità di burn_rate_a nel JSON
2. Verificare se burn_area_m2 = 80.0 è veramente corretta
3. Verificare se chamber_volume nel codice è impostato correttamente
4. Aggiungere parametri nel JSON per: chamber_volume e altre proprietà geom.
5. IMPORTANTE: Nel JSON advanced_liquid manca "nozzle_exit_area_m2"!
   Attualmente usa "burn_area_m2" che è SBAGLIATO per un motore liquido
""")
