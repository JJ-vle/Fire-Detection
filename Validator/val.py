import json
import argparse
from jsonschema import validate, ValidationError

# verif qu'il y a pas de doublon de clé dans JSON
def no_duplicate_object_pairs_hook(pairs):
    obj = {}
    for key, value in pairs:
        if key in obj:
            raise ValueError(f"Clé JSON dupliquée détectée : '{key}'")
        obj[key] = value
    return obj

# parser arguments
parser = argparse.ArgumentParser(description="Valider un fichier JSON selon le schema attendu.")
parser.add_argument(
    "json_file",
    type=str,
    nargs="?",
    default="test.json",
    help="Chemin vers le fichier JSON à valider (par défaut: test.json)"
)
args = parser.parse_args()

# lecture JSON
try:
    with open(args.json_file, "r", encoding="utf-8") as f:
        data = json.load(f, object_pairs_hook=no_duplicate_object_pairs_hook)
except FileNotFoundError:
    print(f"Fichier introuvable : {args.json_file}")
    exit(1)
except json.JSONDecodeError as e:
    print(f"Erreur de syntaxe JSON dans le fichier : {e}")
    exit(1)
except ValueError as e:  # doublons détectés
    print(f"Erreur : {e}")
    exit(1)

# schema JSON
schema = {
    "type": "object",
    "properties": {
        "timestamp_ms": {"type": "number"},
        "temperature_c": {"type": ["number", "null"]},
        "light_raw": {"type": "number"},
        "light_avg": {"type": "number"},
        "fire_detected": {"type": "boolean"},
        "actuators": {
            "type": "object",
            "properties": {
                "clim": {"type": "boolean"},
                "heat": {"type": "boolean"},
                "fan_pwm": {"type": "number"}
            },
            "required": ["clim", "heat", "fan_pwm"],
            "additionalProperties": False
        },
        "thresholds": {
            "type": "object",
            "properties": {
                "low": {"type": "number"},
                "high": {"type": "number"}
            },
            "required": ["low", "high"],
            "additionalProperties": False
        }
    },
    "required": [
        "timestamp_ms", "temperature_c", "light_raw",
        "light_avg", "fire_detected", "actuators", "thresholds"
    ],
    "additionalProperties": False
}


# validation JSON
try:
    validate(instance=data, schema=schema)
    print("JSON valide selon le schéma.")
except ValidationError as e:
    print("JSON invalide :")
    print(f"-> {e.message}")
    if e.path:
        print(f"Chemin de la clé invalide : {' -> '.join(map(str, e.path))}")
