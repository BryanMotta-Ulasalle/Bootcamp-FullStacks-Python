# advisor/views.py
from django.http import JsonResponse
from owlready2 import get_ontology
from django.views.decorators.csrf import csrf_exempt
import json

# -----------------------------
# Cargar ontología
# -----------------------------
ONTO_PATH = r"C:\Users\user\Documents\Bootcamp-FullStacks-Python\Web Semantica\academicadvisor\advisor\ontologia\AppAcademico.owl"
ONTO_PATH = ONTO_PATH.replace("\\", "/")
onto = get_ontology(f"file://{ONTO_PATH}").load()
print("Ontología cargada correctamente.")

# -----------------------------
# Función para recomendar cursos lineales hasta la habilidad deseada
# -----------------------------
def recomendar_cursos_lineales_habilidad(ontologia, estudiante, habilidad_obj):
    # Buscar la clase Curso
    Curso = ontologia.search_one(iri="*Curso")
    if Curso is None:
        raise ValueError("No se encontró la clase Curso en la ontología.")

    # Obtener cursos que poseen la habilidad deseada
    cursos_habilidad = [
        c for c in Curso.instances()
        if hasattr(c, "habilidadCurso") and habilidad_obj in c.habilidadCurso
    ]

    if not cursos_habilidad:
        return []

    # Tomamos el primer curso que posee la habilidad deseada
    curso_final = cursos_habilidad[0]

    # Función recursiva para obtener prerequisitos lineales
    def obtener_prerequisitos_lineales(curso, lista=None):
        if lista is None:
            lista = []
        if curso in lista:
            return lista
        prereqs = getattr(curso, "tienePreRequisito", [])
        for p in prereqs:
            obtener_prerequisitos_lineales(p, lista)
        lista.append(curso)
        return lista

    # Todos los prerequisitos + curso final
    cursos_lineales = obtener_prerequisitos_lineales(curso_final)

    # Filtrar los cursos que el estudiante ya aprobó
    aprobados = getattr(estudiante, "aproboCurso", [])
    cursos_recomendados = [c for c in cursos_lineales if c not in aprobados]

    # Ordenar por semestre
    cursos_recomendados.sort(key=lambda x: getattr(x, "semestreCurso", 0))
    return cursos_recomendados

# -----------------------------
# Vista Django
# -----------------------------
def recomendaciones(request, nombre_persona):
    # Buscar la clase Estudiante
    Estudiante = onto.search_one(iri="*Estudiante")
    if Estudiante is None:
        return JsonResponse({"error": "No se encontró la clase Estudiante"}, status=500)

    # Buscar al estudiante por nombrePersona
    estudiante = None
    for e in Estudiante.instances():
        nombre = getattr(e, "nombrePersona", [])
        if nombre and nombre[0].lower() == nombre_persona.lower():
            estudiante = e
            break

    if estudiante is None:
        return JsonResponse({"error": f"No se encontró el estudiante {nombre_persona}"}, status=404)

    # Obtener habilidades deseadas
    deseadas = getattr(estudiante, "deseaHabilidad", [])
    recomendaciones_finales = []

    for habilidad_obj in deseadas:
        cursos_recomendados = recomendar_cursos_lineales_habilidad(onto, estudiante, habilidad_obj)
        for c in cursos_recomendados:
            nombre_curso = getattr(c, "nombreCurso", [])
            if nombre_curso:
                recomendaciones_finales.append(nombre_curso[0])

    # Habilidades en formato legible
    habilidades_list = []
    for h in deseadas:
        nombre_habilidad = getattr(h, "nombreCompetencia", [])
        if nombre_habilidad:
            habilidades_list.append(nombre_habilidad[0])

    return JsonResponse({
        "estudiante": estudiante.nombrePersona[0] if getattr(estudiante, "nombrePersona", []) else "",
        "habilidades_deseadas": habilidades_list,
        "recomendaciones": recomendaciones_finales
    })



@csrf_exempt
def crear_estudiante(request):
    if request.method != "POST":
        return JsonResponse({"error": "Solo se permiten solicitudes POST"}, status=405)

    try:
        data = json.loads(request.body)
        nombre = data.get("nombre")
        carrera_nombre = data.get("carrera")
        habilidad_nombre = data.get("habilidad_deseada")

        if not all([nombre, carrera_nombre, habilidad_nombre]):
            return JsonResponse({"error": "Faltan datos (nombre, carrera o habilidad_deseada)"}, status=400)

        # Obtener clases de la ontología
        Estudiante = onto.search_one(iri="*Estudiante")
        Carrera = onto.search_one(iri="*Carrera")
        Habilidad = onto.search_one(iri="*Habilidad")

        if not Estudiante or not Carrera or not Habilidad:
            return JsonResponse({"error": "No se encontraron clases necesarias en la ontología"}, status=500)

        # Buscar carrera y habilidad existentes
        carrera = next(
            (c for c in Carrera.instances()
            if hasattr(c, "nombreCarrera") and len(c.nombreCarrera) > 0 and c.nombreCarrera[0].lower() == carrera_nombre.lower()),
            None
        )

        habilidad = next(
            (h for h in Habilidad.instances()
            if hasattr(h, "nombreCompetencia") and len(h.nombreCompetencia) > 0 and h.nombreCompetencia[0].lower() == habilidad_nombre.lower()),
            None
        )


        if not carrera or not habilidad:
            return JsonResponse({"error": "Carrera o habilidad no encontrada en la ontología"}, status=404)

        # Crear nuevo estudiante
        nuevo_estudiante = Estudiante(f"Est_{nombre}")
        nuevo_estudiante.nombrePersona = [nombre]
        nuevo_estudiante.estudiaCarrera = [carrera]
        nuevo_estudiante.deseaHabilidad = [habilidad]

        onto.save(file=ONTO_PATH)
        return JsonResponse({
            "mensaje": "Estudiante creado correctamente",
            "nombre": nombre,
            "carrera": carrera.nombreCarrera[0],
            "habilidad_deseada": habilidad.nombreCompetencia[0]
        })
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)