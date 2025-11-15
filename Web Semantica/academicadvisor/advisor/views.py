# advisor/views.py
# advisor/views.py
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from owlready2 import *
from .ontology_manager import onto

import json

# Importar funciones del ontology_manager
from .ontology_manager import (
    buscar_estudiante,
    obtener_habilidades_deseadas,
    obtener_habilidades_poseidas,
    recomendar_todos_cursos,
    recomendar_por_objetivo,
    obtener_cursos_recomendados_swrl,
    obtener_cursos_no_recomendados_swrl,
    crear_estudiante as crear_est_onto,
    listar_todas_carreras,
    listar_todas_habilidades,
    listar_todos_cursos,
    obtener_cursos_aprobados,
    obtener_cursos_matriculados,
    obtener_carrera_estudiante,
    obtener_objetivo_estudiante,
    obtener_estilo_aprendizaje,
    obtener_recursos_recomendados,
    obtener_ruta_aprendizaje,
    obtener_cursos_en_ruta,
    obtener_recursos_en_ruta,
    inferir_recursos_recomendados,
    inferir_cursos_aprobados,
    inferir_cursos_recomendados,
    obtener_ruta_estudiante,
    crear_ruta_aprendizaje
)

# -----------------------
# Helpers
# -----------------------
def first_or_default(value, default=""):
    """
    Si value es lista -> devuelve value[0] si existe, sino default.
    Si value es None -> default.
    Si value no es lista -> devuelve value.
    """
    if value is None:
        return default
    if isinstance(value, list):
        return value[0] if value else default
    return value

def list_names_from_attr(obj, attr_name):
    """
    Devuelve lista de nombres seguros a partir de una propiedad que podría ser lista de instancias.
    Intenta usar nombreCurso/nombreRecurso/nombreHabilidad, si no existe usa .name.
    """
    items = getattr(obj, attr_name, [])
    result = []
    for it in items:
        # Priorizar propiedades comunes
        if hasattr(it, "nombreCurso"):
            nombre = first_or_default(getattr(it, "nombreCurso", []), "")
        elif hasattr(it, "nombreRecurso"):
            nombre = first_or_default(getattr(it, "nombreRecurso", []), "")
        elif hasattr(it, "nombreHabilidad"):
            nombre = first_or_default(getattr(it, "nombreHabilidad", []), "")
        else:
            nombre = getattr(it, "name", "")
        result.append(nombre)
    return result

# =========================================
# ENDPOINT: Obtener recomendaciones
# =========================================
def recomendaciones(request, nombre_persona):
    """
    GET /recomendaciones/<nombre_persona>/
    Retorna recomendaciones de cursos y recursos usando inferencia basada en reglas Python.
    """
    try:
        # --------------------------
        # Buscar estudiante
        # --------------------------
        estudiante = buscar_estudiante(nombre_persona)

        # Datos básicos
        nombre = first_or_default(getattr(estudiante, "nombrePersona", []), "")
        carrera = obtener_carrera_estudiante(estudiante)
        carrera_nombre = first_or_default(getattr(carrera, "nombreCarrera", []), "Sin carrera") if carrera else "Sin carrera"
        habilidades = obtener_habilidades_poseidas(estudiante)

        # --------------------------
        # INFERENCIA 1:
        # Cursos aprobados basados en habilidades del estudiante
        # --------------------------
        cursos_aprobados = inferir_cursos_aprobados(estudiante)
        cursos_aprobados_json = [
            first_or_default(getattr(c, "nombreCurso", []), "Desconocido")
            for c in cursos_aprobados
        ]

        # --------------------------
        # INFERENCIA 2:
        # Recursos recomendados según cursos NO aprobados y estilo del estudiante
        # --------------------------
        recursos_inferidos = inferir_recursos_recomendados(estudiante)

        recursos_json = []
        for recurso in recursos_inferidos:
            # nombre_recurso seguro
            nombre_recurso = first_or_default(getattr(recurso, "nombreRecurso", []), "") if hasattr(recurso, "nombreRecurso") else getattr(recurso, "name", "")

            # formatos (lista de instancias -> usar .name o nombre de propiedad)
            formatos_objs = getattr(recurso, "recursoTieneFormato", []) or []
            formatos = []
            for f in formatos_objs:
                # si f tiene nombre (p.ej. nombreFormato) intentar usarlo; si no usar .name
                if hasattr(f, "name"):
                    formatos.append(getattr(f, "name"))
                else:
                    formatos.append(first_or_default(getattr(f, "nombreFormato", []), ""))

            # cursos relacionados (protegemos accesos)
            cursos_asociados = getattr(recurso, "utilizadoEnCurso", []) or []
            cursos_asociados_json = [
                first_or_default(getattr(c, "nombreCurso", []), "") for c in cursos_asociados
            ]

            recursos_json.append({
                "recurso": nombre_recurso,
                "formatos": formatos,
                "usado_en_cursos": cursos_asociados_json
            })

        # --------------------------
        # INFERENCIA 3: recomendaciones según el objetivo (cursos NO aprobados requeridos por el objetivo)
        # --------------------------
        cursos_recomendados_inferidos = inferir_cursos_recomendados(estudiante)
        cursos_recomendados_json = [
            first_or_default(getattr(c, "nombreCurso", []), "Desconocido")
            for c in cursos_recomendados_inferidos
        ]

        # --------------------------
        # Respuesta final
        # --------------------------
        return JsonResponse({
            "estudiante": nombre,
            "carrera": carrera_nombre,
            "habilidades_poseidas": [h["nombre"] for h in habilidades] if habilidades else [],
            "cursos_aprobados": cursos_aprobados_json,
            "cursos_recomendados": cursos_recomendados_json,
            "recursos_recomendados": recursos_json
        })

    except ValueError as e:
        return JsonResponse({"error": str(e)}, status=404)

    except Exception as e:
        return JsonResponse({"error": f"Error interno: {str(e)}"}, status=500)


# =========================================
# ENDPOINT: Crear estudiante
# =========================================
@csrf_exempt
@require_http_methods(["POST"])
def crear_estudiante(request):
    """
    POST /api/crear_estudiante/
    Body JSON:
    {
        "nombre": "Juan Perez",
        "carrera": "Ingenieria de Software",
        "habilidad_poseidas": [
            "Crear y dar estilo a paginas web estaticas y responsivas.",
            "Construir interfaces modernas y escalables con componentes reutilizables."
        ],
        "objetivo": "Desarrollo Web",
        "estilo": "Practico"
    }
    """
    try:
        data = json.loads(request.body)

        nombre = data.get("nombre")
        carrera_nombre = data.get("carrera")
        habilidades = data.get("habilidad_poseidas")
        objetivo = data.get("objetivo")
        estilo = data.get("estilo")

        # Validar datos mínimos requeridos
        if not all([nombre, carrera_nombre, habilidades]):
            return JsonResponse({
                "error": "Faltan datos requeridos",
                "requeridos": ["nombre", "carrera", "habilidad_poseidas"]
            }, status=400)

        # Asegurar que habilidades sea siempre lista
        if not isinstance(habilidades, list):
            habilidades = [habilidades]

        # 1️⃣ Crear estudiante en la ontología
        nuevo_estudiante = crear_est_onto(
            nombre,
            carrera_nombre,
            habilidades,
            objetivo,
            estilo
        )

        if nuevo_estudiante is None:
            return JsonResponse({"error": "No se pudo crear el estudiante"}, status=400)

        # 2️⃣ Crear automáticamente la ruta del estudiante
        ruta = crear_ruta_aprendizaje(nuevo_estudiante)

        # 3️⃣ Guardar ontología
        onto.save()

        return JsonResponse({
            "mensaje": "Estudiante y ruta creados exitosamente",
            "estudiante": {
                "nombre": nombre,
                "carrera": carrera_nombre,
                "habilidades": habilidades,
                "objetivo": objetivo,
                "estilo": estilo
            },
            "ruta": str(ruta.name) if ruta else "No generada"
        }, status=201)

    except ValueError as e:
        return JsonResponse({"error": str(e)}, status=404)

    except json.JSONDecodeError:
        return JsonResponse({"error": "JSON invalido en el body"}, status=400)

    except Exception as e:
        return JsonResponse({"error": f"Error interno: {str(e)}"}, status=500)


def ver_ruta_estudiante(request, nombre_persona):
    """
    GET /ruta/<nombre_persona>/
    Devuelve la ruta de aprendizaje asignada al estudiante,
    incluyendo cursos (ordenados por semestre) y recursos.
    """
    try:
        estudiante = buscar_estudiante(nombre_persona)

        # Obtener la ruta asignada
        ruta = obtener_ruta_estudiante(estudiante)
        if not ruta:
            return JsonResponse({
                "mensaje": f"El estudiante {nombre_persona} no tiene una ruta asignada."
            }, status=404)

        ruta_nombre = ruta.name

        # ---------------------------
        # Obtener cursos de la ruta
        # ---------------------------
        cursos = obtener_cursos_en_ruta(ruta)

        def obtener_semestre(curso):
            sem = getattr(curso, "semestreCurso", [999])
            return sem[0]

        cursos_ordenados = sorted(cursos, key=obtener_semestre)

        cursos_json = [
            {
                "nombre": getattr(c, "nombreCurso", ["Desconocido"])[0],
                "semestre": obtener_semestre(c)
            }
            for c in cursos_ordenados
        ]

        # ---------------------------
        # Obtener recursos usados en la ruta
        # ---------------------------
        recursos = obtener_recursos_en_ruta(ruta)

        recursos_json = [
            {
                "nombre": getattr(r, "nombreRecurso", ["Desconocido"])[0]
            }
            for r in recursos
        ]

        return JsonResponse({
            "ruta": ruta_nombre,
            "estudiante": nombre_persona,
            "total_cursos": len(cursos),
            "total_recursos": len(recursos),
            "cursos": cursos_json,
            "recursos": recursos_json
        })

    except ValueError as e:
        return JsonResponse({"error": str(e)}, status=404)
    except Exception as e:
        return JsonResponse({"error": f"Error interno: {str(e)}"}, status=500)

# =========================================
# ENDPOINT: Listar carreras
# =========================================
def listar_carreras(request):
    """
    GET /carreras/
    Retorna todas las carreras disponibles.
    """
    try:
        carreras = listar_todas_carreras()
        return JsonResponse({
            "carreras": carreras,
            "total": len(carreras)
        })
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)

# =========================================
# ENDPOINT: Listar habilidades
# =========================================
def listar_habilidades(request):
    """
    GET /habilidades/
    Retorna todas las habilidades disponibles.
    """
    try:
        habilidades = listar_todas_habilidades()
        return JsonResponse({
            "habilidades": habilidades,
            "total": len(habilidades)
        })
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)

# =========================================
# ENDPOINT: Listar cursos
# =========================================
def listar_cursos(request):
    """
    GET /cursos/
    Retorna todos los cursos disponibles.
    """
    try:
        cursos = listar_todos_cursos()
        return JsonResponse({
            "cursos": cursos,
            "total": len(cursos)
        })
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)

# =========================================
# ENDPOINT: Perfil del estudiante
# =========================================
def perfil_estudiante(request, nombre_persona):
    """
    GET /estudiante/<nombre_persona>/
    Retorna información completa del estudiante.
    """
    try:
        estudiante = buscar_estudiante(nombre_persona)
        
        # Información básica
        nombre = first_or_default(getattr(estudiante, "nombrePersona", []), "")
        
        # Carrera
        carrera = obtener_carrera_estudiante(estudiante)
        carrera_nombre = first_or_default(getattr(carrera, "nombreCarrera", []), "Sin carrera") if carrera else "Sin carrera"
        
        # Cursos aprobados
        aprobados = obtener_cursos_aprobados(estudiante)
        cursos_aprobados = [
            {
                "nombre": first_or_default(getattr(c, "nombreCurso", []), "Desconocido"),
                "semestre": first_or_default(getattr(c, "semestreCurso", []), 0) if hasattr(c, "semestreCurso") else 0
            }
            for c in aprobados
        ]
        
        # Habilidades poseidas
        habilidades = obtener_habilidades_poseidas(estudiante)
        
        return JsonResponse({
            "estudiante": {
                "nombre": nombre,
                "carrera": carrera_nombre,
                "cursos_aprobados": cursos_aprobados,
                "habilidades_poseidas": [h["nombre"] for h in habilidades] if habilidades else [],
                "total_cursos_aprobados": len(cursos_aprobados)
            }
        })
        
    except ValueError as e:
        return JsonResponse({"error": str(e)}, status=404)
    except Exception as e:
        return JsonResponse({"error": f"Error interno: {str(e)}"}, status=500)

# =========================================
# ENDPOINT: Healthcheck
# =========================================
def healthcheck(request):
    """
    GET /api/health/
    Verifica que la API y la ontología estén funcionando.
    """
    try:
        from .ontology_manager import cargar_ontologia
        onto = cargar_ontologia()
        
        return JsonResponse({
            "status": "OK",
            "mensaje": "API y ontología funcionando correctamente",
            "ontologia": str(onto.base_iri)
        })
    except Exception as e:
        return JsonResponse({
            "status": "ERROR",
            "error": str(e)
        }, status=500)
