# advisor/views.py
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
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
    obtener_recursos_en_ruta
)

# =========================================
# ENDPOINT: Obtener recomendaciones
# =========================================
def recomendaciones(request, nombre_persona):
    """
    GET /recomendaciones/<nombre_persona>/
    Retorna recomendaciones de cursos basadas en habilidades deseadas.
    """
    try:
        # Buscar estudiante
        estudiante = buscar_estudiante(nombre_persona)
        
        # Obtener información del estudiante
        nombre = getattr(estudiante, "nombrePersona", [""])[0]
        habilidades = obtener_habilidades_poseidas(estudiante)
        carrera = obtener_carrera_estudiante(estudiante)
        carrera_nombre = getattr(carrera, "nombreCarrera", ["Sin carrera"])[0] if carrera else "Sin carrera"
        
        # Obtener cursos aprobados
        aprobados = obtener_cursos_aprobados(estudiante)
        cursos_aprobados = [
            getattr(c, "nombreCurso", ["Desconocido"])[0]
            for c in aprobados
        ]
        
        # Obtener recomendaciones por habilidad
        recomendaciones_dict = recomendar_todos_cursos(estudiante)
        
        return JsonResponse({
            "estudiante": nombre,
            "carrera": carrera_nombre,
            "cursos_aprobados": cursos_aprobados,
            "habilidades_deseadas": [h["nombre"] for h in habilidades],
            "recomendaciones_por_habilidad": recomendaciones_dict,
            "total_cursos_recomendados": sum(len(cursos) for cursos in recomendaciones_dict.values())
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
    POST /crear_estudiante/
    Body JSON:
    {
        "nombre": "Juan Perez",
        "carrera": "Ingenieria de Sistemas",
        "habilidad_deseada": ["HTML", "CSS"],
        "objetivo": "Desarrollo Web",
        "estilo": "Practico"
    }
    """
    try:
        data = json.loads(request.body)

        nombre = data.get("nombre")
        carrera_nombre = data.get("carrera")
        habilidades = data.get("habilidad_deseada")
        objetivo = data.get("objetivo")
        estilo = data.get("estilo")

        # Validar datos mínimos requeridos
        if not all([nombre, carrera_nombre, habilidades]):
            return JsonResponse({
                "error": "Faltan datos requeridos",
                "requeridos": ["nombre", "carrera", "habilidad_deseada"]
            }, status=400)

        # Asegurar que habilidades sea siempre lista
        if not isinstance(habilidades, list):
            habilidades = [habilidades]

        # Crear estudiante (se ajusta la funcion interna)
        nuevo_estudiante = crear_est_onto(
            nombre,
            carrera_nombre,
            habilidades,
            objetivo,
            estilo
)

        return JsonResponse({
            "mensaje": "Estudiante creado exitosamente",
            "estudiante": {
                "nombre": nombre,
                "carrera": carrera_nombre,
                "habilidades": habilidades,
                "objetivo": objetivo,
                "estilo": estilo
            }
        }, status=201)

    except ValueError as e:
        return JsonResponse({"error": str(e)}, status=404)
    except json.JSONDecodeError:
        return JsonResponse({"error": "JSON invalido en el body"}, status=400)
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
        nombre = getattr(estudiante, "nombrePersona", [""])[0]
        
        # Carrera
        carrera = obtener_carrera_estudiante(estudiante)
        carrera_nombre = getattr(carrera, "nombreCarrera", ["Sin carrera"])[0] if carrera else "Sin carrera"
        
        # Cursos aprobados
        aprobados = obtener_cursos_aprobados(estudiante)
        cursos_aprobados = [
            {
                "nombre": getattr(c, "nombreCurso", ["Desconocido"])[0],
                "semestre": getattr(c, "semestreCurso", [0])[0] if hasattr(c, "semestreCurso") else 0
            }
            for c in aprobados
        ]
        
        # Cursos matriculados
        matriculados = obtener_cursos_matriculados(estudiante)
        cursos_matriculados = [
            {
                "nombre": getattr(c, "nombreCurso", ["Desconocido"])[0],
                "semestre": getattr(c, "semestreCurso", [0])[0] if hasattr(c, "semestreCurso") else 0
            }
            for c in matriculados
        ]
        
        # Habilidades deseadas
        habilidades = obtener_habilidades_deseadas(estudiante)
        
        return JsonResponse({
            "estudiante": {
                "nombre": nombre,
                "carrera": carrera_nombre,
                "cursos_aprobados": cursos_aprobados,
                "cursos_matriculados": cursos_matriculados,
                "habilidades_deseadas": [h["nombre"] for h in habilidades],
                "total_creditos_aprobados": len(cursos_aprobados)
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
    GET /health/
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
    



