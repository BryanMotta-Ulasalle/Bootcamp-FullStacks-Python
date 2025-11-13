from owlready2 import get_ontology
import os

# Cambia la ruta según tu proyecto
ONTO_PATH = os.path.join(os.path.dirname(__file__), "ontologia/AppAcademico.owl")
ONTO_PATH = ONTO_PATH.replace("\\", "/")
onto = get_ontology(f"file://{ONTO_PATH}").load()
print("Ontología cargada correctamente.")

def get_estudiante(nombre_persona):
    estudiante = onto.search_one(nombrePersona=nombre_persona)
    if estudiante is None:
        raise ValueError(f"No se encontró el estudiante {nombre_persona} en la ontología.")
    return estudiante

def listar_cursos_propiedad(estudiante, propiedad):
    if hasattr(estudiante, propiedad):
        cursos = getattr(estudiante, propiedad)
        return [c.nombreCurso.first() or "Desconocido" for c in cursos]
    return []

def listar_habilidades_deseadas(estudiante):
    if hasattr(estudiante, "deseaHabilidad"):
        habilidades = getattr(estudiante, "deseaHabilidad")
        return [h.nombreCompetencia.first() or "Desconocida" for h in habilidades]
    return []

def cursos_lineales_para_habilidad(estudiante):
    """
    Recomienda cursos lineales hasta alcanzar la habilidad deseada.
    """
    # Obtener cursos que el estudiante ya aprobó
    aprobados = getattr(estudiante, "aproboCurso") if hasattr(estudiante, "aproboCurso") else []

    # Obtener cursos que poseen la habilidad deseada
    deseadas = getattr(estudiante, "deseaHabilidad") if hasattr(estudiante, "deseaHabilidad") else []
    cursos_objetivo = []
    for habilidad in deseadas:
        for curso in onto.Curso.instances():
            if hasattr(curso, "habilidadCurso") and habilidad in curso.habilidadCurso:
                cursos_objetivo.append(curso)

    # Función para generar la cadena lineal de prerequisitos
    def construir_linea(curso):
        linea = []
        current = curso
        prereq_chain = []
        while True:
            prereqs = getattr(current, "tienePreRequisito") if hasattr(current, "tienePreRequisito") else []
            if not prereqs:
                break
            current = prereqs[0]
            prereq_chain.insert(0, current)  # insertamos al inicio
        return prereq_chain + [curso]

    # Filtrar solo los cursos no aprobados
    recomendados = []
    for curso in cursos_objetivo:
        linea = construir_linea(curso)
        for c in linea:
            if c not in aprobados and c not in recomendados:
                recomendados.append(c)

    return aprobados, cursos_objetivo, recomendados
