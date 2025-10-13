# JSON Validator

Ce script Python permet de valider un fichier JSON généré par **regul.ino** selon un **schéma strict**, afin de garantir que les champs attendus sont présents et que **aucun champ supplémentaire n’est autorisé**.

---

## Fonctionnalités

* Valide que tous les champs requis sont présents :

  * `timestamp_ms`, `temperature_c`, `light_raw`, `light_avg`, `fire_detected`, `actuators`, `thresholds`
* Vérifie le type de chaque champ (`number`, `boolean`, ou `null` pour `temperature_c`)
* Vérifie que **aucun champ supplémentaire** n’est présent à la racine ou dans les objets imbriqués (`actuators`, `thresholds`)
* Supporte un chemin de fichier JSON **optionnel**. Si aucun chemin n’est fourni, le script utilise `test.json` par défaut.

---

## Dépendances

* Python 3.7+
* [jsonschema](https://pypi.org/project/jsonschema/)

Installation :

```bash
pip install jsonschema
```

---

## Utilisation

### Avec le fichier par défaut (`test.json`) :

```bash
python val.py
```

### Avec un fichier spécifique :

```bash
python val.py chemin/vers/fichier.json
```

### Exemple de sortie

* JSON valide :

```
JSON valide selon le schéma.
```

* JSON invalide (champ manquant ou type incorrect) :

```
JSON invalide :
-> 'fan_pwm' is a required property
Chemin de la clé invalide : actuators
```

* JSON invalide (champ supplémentaire non autorisé) :

```
JSON invalide :
-> Additional properties are not allowed ('extra_field' was unexpected)
Chemin de la clé invalide :
```

---

## Schéma JSON attendu

### Racine

| Champ         | Type          | Requis |
| ------------- | ------------- | ------ |
| timestamp_ms  | number        | Oui    |
| temperature_c | number / null | Oui    |
| light_raw     | number        | Oui    |
| light_avg     | number        | Oui    |
| fire_detected | boolean       | Oui    |
| actuators     | object        | Oui    |
| thresholds    | object        | Oui    |

### `actuators`

| Champ   | Type    | Requis |
| ------- | ------- | ------ |
| clim    | boolean | Oui    |
| heat    | boolean | Oui    |
| fan_pwm | number  | Oui    |

### `thresholds`

| Champ | Type   | Requis |
| ----- | ------ | ------ |
| low   | number | Oui    |
| high  | number | Oui    |

> Tous les objets interdisent les champs supplémentaires.


