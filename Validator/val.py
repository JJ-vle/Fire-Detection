import json
from jsonschema import validate, ValidationError

pathfile = "../../test.json"

# lecture JSON
try:
    with open(pathfile, "r", encoding="utf-8") as f:
        data = json.load(f)
except FileNotFoundError:
    print(f"Fichier introuvable : {pathfile}")
    exit(1)
except json.JSONDecodeError as e:
    print(f"Erreur de syntaxe JSON dans le fichier : {e}")
    exit(1)

schema = {
    "type": "object",
    "properties": {
        "timestamp_ms": {"type": "number"},
        "temperature_c": {"type": "number"},
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
            "required": ["clim", "heat", "fan_pwm"]
        },
        "thresholds": {
            "type": "object",
            "properties": {
                "low": {"type": "number"},
                "high": {"type": "number"}
            },
            "required": ["low", "high"]
        }
    },
    "required": [
        "timestamp_ms", "temperature_c", "light_raw",
        "light_avg", "fire_detected", "actuators", "thresholds"
    ]
}

# validation JSON
try:
    validate(instance=data, schema=schema)
    print("JSON valide selon le schéma.")
except ValidationError as e:
    print("JSON invalide :")
    print(f"-> {e.message}")
