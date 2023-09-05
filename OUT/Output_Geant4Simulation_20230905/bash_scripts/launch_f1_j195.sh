#! /bin/bash
cd /data1/home/rnicolai
cd /data1/home/rnicolai/LEM_GDML_upgrade

MAX_ATTEMPTS=5
ATTEMPT=1

while [ $ATTEMPT -le $MAX_ATTEMPTS ]; do
    # Esegui il comando qui
/data1/home/rnicolai/LEM_GDML_upgrade/build/gdml_det /data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_20230905/GDML_src/LEM_LargerCalo-worldVOL_Parsed.gdml /data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_20230905/Geant_macros/macro_f1_j195.mac /data1/home/rnicolai/LEM_GDML_upgrade/OUT/Output_Geant4Simulation_20230905/steering_files/steering_f1_j195.txt



    if [ $? -eq 0 ]; then
        echo "Comando eseguito correttamente alla tentativo $ATTEMPT"
        break
    else
        echo "Errore nell'esecuzione del comando alla tentativo $ATTEMPT. Riprovo..."
        ATTEMPT=$((ATTEMPT + 1))
        sleep 5  # Attendi un po' prima del prossimo tentativo
    fi
done

if [ $ATTEMPT -gt $MAX_ATTEMPTS ]; then
    echo "Il comando non è stato eseguito correttamente dopo $MAX_ATTEMPTS tentativi."
fi

