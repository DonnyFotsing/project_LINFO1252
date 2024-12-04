import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def extract_seconds(time_str):
    if 'm' in time_str and 's' in time_str:
        minutes, seconds = time_str.split('m')
        seconds = seconds.replace('s', '') 
        return float(seconds) + int(minutes) * 60
    elif 's' in time_str:
        return float(time_str.replace('s', ''))
    return 0.0
df = pd.read_csv('prcon.csv', sep=',')
measures = {2: [], 4: [], 8: [], 16: [], 32: []}
for _, row in df.iterrows():
    try:
        nb_threads = int(row['Nb_threads'])
        measure_str = row['Measure_1']
        measure_list = measure_str.split(';')
        for time_str in measure_list:
            time_str = time_str.strip() 
            if time_str and time_str != "Measure file does not exist." and time_str != "Erreur lors de la mesure":
                time = extract_seconds(time_str)
                measures[nb_threads].append(time)
    except ValueError:
        print(f"Skipping row with invalid Nb_threads value: {row['Nb_threads']}")  
mean_times = []
std_times = []
for nb_threads in sorted(measures.keys()):
    times = measures[nb_threads]
    if times: 
        mean_times.append(np.mean(times))
        std_times.append(np.std(times))
    else:
        mean_times.append(np.nan) 
        std_times.append(np.nan)

results_df = pd.DataFrame({
    'Nb_threads': list(measures.keys()),
    'Mean_time': mean_times,
    'Std_time': std_times
})
print(results_df)
valid_results = results_df.dropna()
if valid_results.empty:
    print("Aucun résultat valide pour le graphique.")
else:
    x = valid_results['Nb_threads']  
    y = valid_results['Mean_time']  
    yerr = valid_results['Std_time']  
   
    plt.figure(figsize=(8, 6))
    plt.errorbar(x, y, yerr=yerr, fmt='-o', capsize=5, color='blue', label='Temps moyen avec écart-type')
    plt.title('Temps d\'exécution en fonction du nombre de threads producteur-consommaateur ')
    plt.xlabel('Nombre de threads')
    plt.ylabel('Temps d\'exécution (secondes)')
    plt.xticks(valid_results['Nb_threads'])
    plt.yticks(np.arange(0, max(valid_results['Mean_time']) , 5)) 
   
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()
