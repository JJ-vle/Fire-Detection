# Application de régulation de température – Arduino & Node-RED

## GROUPE_C
Github : https://github.com/JJ-vle/Fire-Detection

- Tom DA COSTA
- Jean-Jacques VIALE

## Aperçu

Cette application permet de surveiller et contrôler la température, la luminosité et la ventilation d’un environnement en temps réel.
Elle repose sur une carte ESP32 pour les mesures et le pilotage, et sur une interface Node-RED pour l’affichage et la configuration.
Un script Python optionnel permet de valider les données JSON échangées.

---

## Architecture générale

### 1. Arduino (`regul.ino`)

Le programme embarqué sur la carte :

* lit la température et la luminosité
* détecte un départ de feu
* pilote les actionneurs : chauffage, climatisation, ventilateur, LED
* envoie les données sous forme JSON sur le port série
* reçoit les seuils de température (`MIN`, `MAX`) envoyés depuis le dashboard Node-RED

### 2. Interface Node-RED (`regul.json`)

Le flow Node-RED :

* lit les données JSON issues du port série
* affiche les mesures en graphes et jauges interactives
* montre l’état des contrôleurs via des LED virtuelles (chauffage, clim, feu)
* permet de modifier les seuils de température grâce à des sliders
* renvoie les nouvelles valeurs de seuils à l’Arduino via le port série

### 3. Validateur JSON (`val.py`)

Le script vérifie que les messages JSON générés :

* respectent un schéma strict (tous les champs requis, bons types, pas de champs supplémentaires)
* facilitent le débogage et la fiabilité de la communication série

Il pourrait à terme être appelé par le serveur Node-RED pour ne traiter que les communications validées et donc renforcer la sécurité.
