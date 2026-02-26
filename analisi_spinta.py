#!/usr/bin/env python3
"""
Analisi numerica della spinta dei motori del razzo
Confronto tra Advanced Solid Engine e Advanced Liquid Engine
"""

import math
import json

# ------ COSTANTI FISICHE ------
R_univ = 8.314462618  # J/(mol*K)
g0 = 9.80665  # m/s^2
pa_ambient = 101325  # Pa

# ------ PARAMETRI DA JSON ------
# ROCKET
rocket_mass_struct = 10000.0  # kg
solid_fuel = 50000.0  # kg
liquid_fuel = 100000.0  # kg
solid_container = 5000.0  # kg
liquid_container = 10000.0  # kg
total_mass = rocket_mass_struct + solid_fuel + liquid_fuel + solid_container + liquid_container
print(f"Massa totale razzo: {total_mass} kg")

# ADVANCED SOLID ENGINE (parametri dal JSON)
sol_burn_area = 80.0  # m^2
sol_throat_area = 0.18  # m^2
sol_exit_area = 2.5  # m^2
sol_chamber_temp = 3600.0  # K
sol_grain_density = 1800.0  # kg/m^3
sol_burn_rate_a = 0.00012  # [m/(s*Pa^n)]
sol_burn_rate_n = 0.35  # [-]
sol_molar_mass = 22.0 / 1000.0  # kg/mol (convertito da g/mol)
sol_chamber_pressure_default = 7.0e6  # Pa (default nel codice)
sol_gamma = 1.22  # (da engine.h)

# ADVANCED LIQUID ENGINE (parametri dal JSON)
liq_chamber_pressure = 18000000.0  # Pa
liq_chamber_temp = 3600.0  # K
liq_throat_area = 0.06  # m^2
# !! PROBLEMA: nel JSON manca "nozzle_exit_area_m2", usa "burn_area_m2": 2.5
liq_exit_area = 2.5  # m^2  <-- POTENZIALE ERRORE! Dovrebbe essere l'exit area dell'ugello
liq_molar_mass = 18.0 / 1000.0  # kg/mol
liq_gamma = 1.22

print("\n" + "="*80)
print("ANALISI ADVANCED SOLID ENGINE")
print("="*80)

# Calcolo REGRESSION RATE
regression_rate_sol = sol_burn_rate_a * (sol_chamber_pressure_default ** sol_burn_rate_n)
print(f"Chamber pressure: {sol_chamber_pressure_default/1e6:.1f} MPa")
print(f"Regression rate: r_dot = {sol_burn_rate_a} * ({sol_chamber_pressure_default:.2e})^{sol_burn_rate_n}")
print(f"r_dot = {regression_rate_sol:.6f} m/s")

# Calcolo MASS FLOW RATE (solido)
mdot_sol = sol_grain_density * sol_burn_area * regression_rate_sol
print(f"\nMass flow rate:")
print(f"  mdot = ρ * A_b * r_dot")
print(f"  mdot = {sol_grain_density} * {sol_burn_area} * {regression_rate_sol:.6f}")
print(f"  mdot = {mdot_sol:.1f} kg/s")

# Calcolo c_star (solid) - usando formula dal costruttore
R_spec_sol = R_univ / sol_molar_mass
term_sol = 2.0 / (sol_gamma + 1.0)
exponent_sol = (sol_gamma + 1.0) / (2.0 * (sol_gamma - 1.0))
c_star_sol = math.sqrt(R_univ * sol_chamber_temp / sol_molar_mass) * (1.0 / sol_gamma) * (term_sol ** exponent_sol)
print(f"\nCaracteristic velocity (c_star):")
print(f"  Formula del codice: c* = sqrt(R*T/M) * (1/γ) * (2/(γ+1))^((γ+1)/(2(γ-1)))")
print(f"  c_star = {c_star_sol:.1f} m/s")

# Area ratio per calcolo exit pressure e Mach
area_ratio_sol = sol_exit_area / sol_throat_area
print(f"\nArea ratio: A_e/A_t = {sol_exit_area}/{sol_throat_area} = {area_ratio_sol:.1f}")

# Stimare velocità di uscita (approssimazione isentropica)
# v_e = sqrt(2*gamma/(gamma-1) * R_spec * T_c * (1 - (pe/pc)^((gamma-1)/gamma)))
# Per una prima stima, assumo pe basso
term_ve_sol = 2.0 * sol_gamma / (sol_gamma - 1.0) * R_spec_sol * sol_chamber_temp
v_e_sol_approx = math.sqrt(term_ve_sol)
print(f"\nVelocità uscita (stima, assumendo pe << pc):")
print(f"  v_e ≈ sqrt(2γ/(γ-1) * R_spec * T_c)")
print(f"  v_e ≈ {v_e_sol_approx:.1f} m/s")

# Spinta (solo momentum term, ignorando pressure term per ora)
thrust_momentum_sol = mdot_sol * v_e_sol_approx
print(f"\nSpinta momenti (ignorando pressure term):")
print(f"  F_momentum = mdot * v_e = {mdot_sol:.1f} * {v_e_sol_approx:.1f}")
print(f"  F_momentum = {thrust_momentum_sol:.2e} N = {thrust_momentum_sol/1e6:.1f} MN")

# Numero di motori solidi
n_sol_eng = 2
total_thrust_sol = n_sol_eng * thrust_momentum_sol
print(f"\nSpinta totale solida ({n_sol_eng} motori):")
print(f"  F_total_solid = {n_sol_eng} * {thrust_momentum_sol:.2e} N")
print(f"  F_total_solid = {total_thrust_sol:.2e} N = {total_thrust_sol/1e6:.1f} MN")

print("\n" + "="*80)
print("ANALISI ADVANCED LIQUID ENGINE")
print("="*80)

print(f"Chamber pressure: {liq_chamber_pressure/1e6:.1f} MPa")
print(f"Chamber temperature: {liq_chamber_temp} K")
print(f"Molar mass: {liq_molar_mass*1000:.1f} g/mol")
print(f"Throat area A_t: {liq_throat_area} m^2")
print(f"Exit area A_e: {liq_exit_area} m^2")

# Calcolo c_star (liquid) - usando la stessa formula del codice
R_spec_liq = R_univ / liq_molar_mass
term_liq = 2.0 / (liq_gamma + 1.0)
exponent_liq = (liq_gamma + 1.0) / (2.0 * (liq_gamma - 1.0))
c_star_liq = math.sqrt(R_univ * liq_chamber_temp / liq_molar_mass) * (1.0 / liq_gamma) * (term_liq ** exponent_liq)
print(f"\nCaracteristic velocity (c_star):")
print(f"  Formula del codice: c* = sqrt(R*T/M) * (1/γ) * (2/(γ+1))^((γ+1)/(2(γ-1)))")
print(f"  c_star = {c_star_liq:.1f} m/s")

# Calcolo MASS FLOW RATE (liquido)
# mdot = p_c * A_t / c_star
mdot_liq = (liq_chamber_pressure * liq_throat_area) / c_star_liq
print(f"\nMass flow rate:")
print(f"  mdot = p_c * A_t / c_star")
print(f"  mdot = {liq_chamber_pressure:.2e} * {liq_throat_area} / {c_star_liq:.1f}")
print(f"  mdot = {mdot_liq:.1f} kg/s")

# Area ratio
area_ratio_liq = liq_exit_area / liq_throat_area
print(f"\nArea ratio: A_e/A_t = {liq_exit_area}/{liq_throat_area} = {area_ratio_liq:.1f}")

# Stimare velocità di uscita
term_ve_liq = 2.0 * liq_gamma / (liq_gamma - 1.0) * R_spec_liq * liq_chamber_temp
v_e_liq_approx = math.sqrt(term_ve_liq)
print(f"\nVelocità uscita (stima, assumendo pe << pc):")
print(f"  v_e ≈ sqrt(2γ/(γ-1) * R_spec * T_c)")
print(f"  v_e ≈ {v_e_liq_approx:.1f} m/s")

# Spinta liquido
thrust_momentum_liq = mdot_liq * v_e_liq_approx
print(f"\nSpinta momenti (ignorando pressure term):")
print(f"  F_momentum = mdot * v_e = {mdot_liq:.1f} * {v_e_liq_approx:.1f}")
print(f"  F_momentum = {thrust_momentum_liq:.2e} N = {thrust_momentum_liq/1e6:.2f} MN")

# Numero di motori liquidi
n_liq_eng = 1
total_thrust_liq = n_liq_eng * thrust_momentum_liq
print(f"\nSpinta totale liquida ({n_liq_eng} motori):")
print(f"  F_total_liquid = {n_liq_eng} * {thrust_momentum_liq:.2e} N")
print(f"  F_total_liquid = {total_thrust_liq:.2e} N = {total_thrust_liq/1e6:.2f} MN")

print("\n" + "="*80)
print("CONFRONTO SPINTA vs PESO")
print("="*80)

total_thrust = total_thrust_sol + total_thrust_liq
weight = total_mass * g0
thrust_to_weight = total_thrust / weight

print(f"\nSpinta totale sistemi: {total_thrust:.2e} N = {total_thrust/1e6:.1f} MN")
print(f"Peso razzo: {weight:.2e} N = {weight/1e6:.2f} MN")
print(f"Rapporto spinta/peso: {thrust_to_weight:.2f}")

if thrust_to_weight > 1:
    print(f"✓ SUFFICIENTE per il distacco (T/W = {thrust_to_weight:.2f} > 1)")
else:
    print(f"✗ INSUFFICIENTE per il distacco! (T/W = {thrust_to_weight:.2f} < 1)")

print("\n" + "="*80)
print("ANALISI DETTAGLIATA DELLA FORMULA c_star")
print("="*80)

print(f"\nFormula NEL CODICE:")
print(f"  c_star = sqrt(R*T/M) * (1/γ) * (2/(γ+1))^((γ+1)/(2*(γ-1)))")

print(f"\nFormula CORRETTA:")
print(f"  c_star = sqrt((2/(γ+1))^((γ+1)/(γ-1)) * R_spec * T)")

# Calcolo con formula corretta (per liquid)
correct_term = (2.0 / (liq_gamma + 1.0)) ** ((liq_gamma + 1.0) / (liq_gamma - 1.0))
c_star_liq_correct = math.sqrt(correct_term * R_spec_liq * liq_chamber_temp)
print(f"\nPer liquid engine:")
print(f"  c_star (formula nel codice): {c_star_liq:.1f} m/s")
print(f"  c_star (formula corretta): {c_star_liq_correct:.1f} m/s")
print(f"  Differenza: {abs(c_star_liq - c_star_liq_correct):.1f} m/s ({abs(c_star_liq - c_star_liq_correct)/c_star_liq_correct*100:.1f}%)")

# Ricalcolo mdot con c_star corretto
mdot_liq_correct = (liq_chamber_pressure * liq_throat_area) / c_star_liq_correct
thrust_momentum_liq_correct = mdot_liq_correct * v_e_liq_approx
total_thrust_liq_correct = n_liq_eng * thrust_momentum_liq_correct
total_thrust_correct = total_thrust_sol + total_thrust_liq_correct
thrust_to_weight_correct = total_thrust_correct / weight

print(f"\nCon c_star corretto:")
print(f"  mdot = {mdot_liq_correct:.1f} kg/s (era {mdot_liq:.1f})")
print(f"  F_liquid = {total_thrust_liq_correct:.2e} N (era {total_thrust_liq:.2e})")
print(f"  F_totale = {total_thrust_correct:.2e} N")
print(f"  T/W = {thrust_to_weight_correct:.2f}")

print("\n" + "="*80)
print("CONCLUSIONI")
print("="*80)

print(f"""
❌ PROBLEMA IDENTIFICATO:

1. La formula di c_star nel codice è SBAGLIATA:
   - Formula nel codice: c* = sqrt(R*T/M) * (1/γ) * (2/(γ+1))^((γ+1)/(2*(γ-1)))
   - Formula corretta:   c* = sqrt((2/(γ+1))^((γ+1)/(γ-1)) * R_spec * T)
   
   Questo causa un c_star SOTTOSTIMATO di ~{abs(c_star_liq - c_star_liq_correct)/c_star_liq_correct*100:.1f}%

2. Il parametro "nozzle_exit_area_m2" MANCA nel JSON per advanced_liquid:
   - Si usa "burn_area_m2": 2.5 come exit area
   - Questo dovrebbe essere verificato: è l'area corretta?

3. Risultato: spinta insufficiente per il distacco

Rapporto spinta/peso ATTUALE: {thrust_to_weight:.3f}
Rapporto spinta/peso CON CORREZIONE: {thrust_to_weight_correct:.3f}
""")
