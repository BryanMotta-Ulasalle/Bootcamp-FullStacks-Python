from .ontology_manager import onto

def probar_ontologia():
    print("Clases disponibles:")
    for cls in onto.classes():
        print("-", cls)

    print("\nIndividuos:")
    for ind in onto.individuals():
        print("-", ind)

if __name__ == "__main__":
    probar_ontologia()
