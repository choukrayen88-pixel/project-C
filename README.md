# 🌡️ Système de Surveillance de Température

Ce projet est un **système de surveillance de température en C**. Il simule des relevés de température, évalue chaque mesure selon des **seuils configurables**, et attribue un **niveau d’alerte** allant de normal à critique. Toutes les mesures sont horodatées et stockées dans une **liste chaînée dynamique**, constituant un journal complet des données.  

Le programme permet :  
- 📝 **Journaliser** toutes les mesures dans `journal.txt`  
- 📊 **Générer un rapport quotidien** (`rapport_journalier.txt`) avec température minimale, maximale, moyenne et nombre d’alertes  
- ⚠️ **Lister les incidents critiques** dans `incidents.txt`  
- 🔔 **Notifier en temps réel** les alertes critiques  

Les paramètres (seuils et intervalle de mesure) sont gérés via `config.txt`, avec des valeurs par défaut si le fichier est absent.  

Ce projet met en pratique des concepts essentiels de C : **gestion dynamique de la mémoire, fichiers, listes chaînées, gestion du temps et modularité**, tout en offrant un système réaliste et extensible.  
