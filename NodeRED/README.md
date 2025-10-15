# Tableau de bord Node-RED – Contrôle Température & Ventilation

## Prérequis

Avant d’importer ce flow dans **Node-RED**, assurez-vous d’avoir installé les modules suivants :

```bash
npm install node-red-dashboard
npm install node-red-node-serialport
npm install node-red-contrib-ui-led
```

Ces modules permettent respectivement :

* **node-red-dashboard** : création d’interfaces graphiques (boutons, jauges, sliders, graphiques, etc.)
* **node-red-node-serialport** : communication série avec une carte microcontrôleur (Arduino, ESP, etc.)
* **node-red-contrib-ui-led** : affichage d’indicateurs lumineux (LED virtuelles) dans le dashboard.

---

## Importer le flux

1. Sur NodeRED : Clic droit -> Insérer -> Importer
2. Sélectionnez ensuite le fichier regul.json ou copier/coller son contenu dans l'espace prévu à cet effet

---

## Configuration du port série

Si le flow ne parvient pas à se connecter au port série :

1. Ouvrez le **noeud "Serial Port"** dans Node-RED.
2. Cliquez sur le crayon à côté de **Serial Port**.
3. Modifiez le champ **Serial Port** selon votre configuration
4. Déployez à nouveau le flow.


---

## Accès au Dashboard

Une fois Node-RED lancé, le dashboard est par défaut disponible à l’adresse suivante :
**[http://localhost:1880/ui](http://localhost:1880/ui)**

---

## Description du Dashboard

Le tableau de bord comporte plusieurs sections :

### **Graphiques**

* **Température (°C)** : affiche l’évolution de la température mesurée dans le temps.
* **Lumière** : courbe du niveau de luminosité reçu.

### **Données du ventilateur**

* **Jauge “RPM”** : indique la vitesse du ventilateur (en %).

### **Limites de température**

* **Sliders “Température Minimum / Maximum”** : permettent de définir les seuils de contrôle.
* Les valeurs choisies sont envoyées via le port série au microcontrôleur (`MIN:` / `MAX:`).
* Les valeurs actuelles s’affichent également sous forme de texte.

### **Status des contrôleurs de température**

* **LED “Radiateur”** : s’allume lorsque le chauffage est activé.
* **LED “Clim”** : s’allume lorsque la climatisation est active.

### **Feu détecté ?**

* **LED “Status”** : s’allume en vert si un feu est détecté (`ON`), rouge sinon (`OFF`).

---

## Fonctionnement général

Le flow reçoit en continu des données sous forme de JSON provenant du port série, les décode et les distribue vers :

* les graphiques** (température, lumière)
* les indicateurs de statut (chauffage, climatisation, feu)
* la jauge du ventilateur
* les seuils de température définis via le dashboard, renvoyés ensuite vers le port série.

---

