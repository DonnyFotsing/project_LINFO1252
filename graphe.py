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

df = pd.read_csv('measures.csv', sep=',')

measures = {2: [], 4: [], 8: [], 16: [], 32: []}
print("Contenu du fichier CSV :")
print(df)
for _, row in df.iterrows():
    try:
        nb_threads = int(row['Nb_threads']) 
        measure_str = row['Measure_1']
        measure_list = measure_str.split(';')
        for time_str in measure_list:
            time_str = time_str.strip() 
            if time_str != "Measure file does not exist." and time_str != "Erreur lors de la mesure" and time_str != "":
                time = extract_seconds(time_str) 
                measures[nb_threads].append(time)
    except ValueError:
        print(f"Skipping row with invalid Nb_threads value: {row['Nb_threads']}") 


print("Mesures collectées :")
for nb_threads, times in measures.items():
    print(f"Nb_threads: {nb_threads}, Times: {times}")


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

print("DataFrame des résultats :")
print(results_df)
valid_results = results_df.dropna()


if valid_results.empty:
    print("Aucun résultat valide pour le graphique.")
else:
    x = valid_results['Nb_threads']  # Axe x
    y = valid_results['Mean_time']  # Axe y
    yerr = valid_results['Std_time']  # Barres d'erreur
    plt.figure(figsize=(8, 6))
    #pour la representation des ecartypes et moyennes sur le graphe
    plt.errorbar(x, y, yerr=yerr, fmt='-o', capsize=50)
    plt.title('Temps d\'exécution en fonction du nombre de threads')
    plt.xlabel('Nombre de threads')
    plt.ylabel('Temps d\'exécution (secondes)')
    #on modifie l'axe des x et y pour afin de pouvoir avoir un schema bien clair
    plt.xticks(valid_results['Nb_threads'])
    plt.yticks(np.arange(0, max(valid_results['Mean_time']) + 1, 0.5))
    plt.grid(True)
    plt.show()
