# ontology_manager.py
from owlready2 import get_ontology
import os

# Cambia la ruta según tu proyecto
ONTO_PATH = os.path.join(os.path.dirname(__file__), "ontologia/AppAcademico.owl")
ONTO_PATH = ONTO_PATH.replace("\\", "/")
onto = get_ontology(f"file://{ONTO_PATH}").load()
print("Ontología cargada correctamente.")

def cargar_ontologia():
    """Retorna la ontología ya cargada."""
    return onto

def obtener_clases_ontologia():
    """Retorna las clases principales de la ontología."""
    onto = cargar_ontologia()
    
    # Namespace correcto según tu ontología
    unti = onto.get_namespace("http://www.semanticweb.org/user/ontologies/2025/10/untitled-ontology-21/")
    acad = onto.get_namespace("http://www.ontologia-academica.org/academic#")
    
    # Buscar clases
    Estudiante = unti.Estudiante
    Curso = unti.Curso
    Carrera = unti.Carrera
    Habilidad = unti.Habilidad
    ObjetivoFinal = acad.ObjetivoFinal
    RutaDeAprendizaje = unti.RutaDeAprendizaje
    RecursoAcademico = acad.RecursoAcademico
    EstiloAprendizaje = acad.EstiloAprendizaje
    
    return {
        "Estudiante": Estudiante,
        "Curso": Curso,
        "Carrera": Carrera,
        "Habilidad": Habilidad,
        "ObjetivoFinal": ObjetivoFinal,
        "RutaDeAprendizaje": RutaDeAprendizaje,
        "RecursoAcademico": RecursoAcademico,
        "EstiloAprendizaje": EstiloAprendizaje,
        "onto": onto
    }

def buscar_estudiante(nombre_persona):
    """Busca un estudiante por su nombre."""
    clases = obtener_clases_ontologia()
    Estudiante = clases["Estudiante"]
    
    if Estudiante is None:
        raise ValueError("No se encontró la clase Estudiante en la ontología")
    
    # Buscar por nombrePersona
    for estudiante in Estudiante.instances():
        nombre = getattr(estudiante, "nombrePersona", [])
        if nombre and len(nombre) > 0 and nombre[0].lower() == nombre_persona.lower():
            return estudiante
    
    raise ValueError(f"No se encontró el estudiante '{nombre_persona}' en la ontología")

def inferir_cursos_aprobados(estudiante):
    """
    Aplica manualmente la regla:
    Estudiante(e) ^ poseeHabilidad(e,h) ^ Curso(c) ^ habilidadCurso(c,h) -> aproboCurso(e,c)
    y retorna la lista de cursos inferidos.
    """

    clases = obtener_clases_ontologia()
    Curso = clases["Curso"]

    # Obtener habilidades del estudiante
    habilidades_est = list(getattr(estudiante, "poseeHabilidad", []))

    cursos_aprobados = set()

    # Para cada curso...
    for curso in Curso.instances():
        # Obtener habilidades requeridas por ese curso
        habilidades_curso = list(getattr(curso, "habilidadCurso", []))

        # Si alguna habilidad coincide, el curso se infiere como aprobado
        if any(h in habilidades_curso for h in habilidades_est):
            cursos_aprobados.add(curso)

    return list(cursos_aprobados)


def obtener_cursos_aprobados(estudiante):
    """
    Retorna cursos aprobados usando la inferencia manual.
    """
    return inferir_cursos_aprobados(estudiante)

def inferir_cursos_recomendados(estudiante):
    """
    Recomienda cursos siguiendo la regla:
    Estudiante -> persigueObjetivo -> ObjetivoFinal -> requiereCursoAprobado -> Curso
    y EXCLUYE los cursos ya aprobados.
    """
    # 1. Objetivo del estudiante
    objetivos = getattr(estudiante, "persigueObjetivo", [])
    if not objetivos:
        return []

    objetivo = objetivos[0]  # Asumimos un objetivo principal

    # 2. Cursos requeridos por ese objetivo
    cursos_requeridos = getattr(objetivo, "requiereCursoAprobado", [])

    # 3. Cursos aprobados por inferencia
    cursos_aprobados = inferir_cursos_aprobados(estudiante)

    # 4. Filtrar: recomendar solo cursos NO aprobados
    cursos_recomendados = [
        c for c in cursos_requeridos if c not in cursos_aprobados
    ]

    return cursos_recomendados

def obtener_cursos_no_aprobados(estudiante):
    """Retorna los cursos que el estudiante NO ha aprobado."""
    clases = obtener_clases_ontologia()
    Curso = clases["Curso"]

    aprobados = inferir_cursos_aprobados(estudiante)
    todos = list(Curso.instances())

    no_aprobados = [c for c in todos if c not in aprobados]
    return no_aprobados

def obtener_cursos_matriculados(estudiante):
    """Retorna lista de cursos en los que está matriculado."""
    return list(getattr(estudiante, "estaMatriculadoEn", []))

def obtener_habilidades_deseadas(estudiante):
    """Retorna lista de habilidades que desea el estudiante."""
    habilidades = getattr(estudiante, "deseaHabilidad", [])
    return [
        {
            "objeto": h,
            "nombre": getattr(h, "nombreHabilidad", ["Desconocida"])[0] if hasattr(h, "nombreHabilidad") else "Desconocida"
        }
        for h in habilidades
    ]

def obtener_carrera_estudiante(estudiante):
    """Retorna la carrera del estudiante."""
    carreras = getattr(estudiante, "cursaCarrera", [])  # Cambio de estudiaCarrera a cursaCarrera
    if carreras and len(carreras) > 0:
        return carreras[0]
    return None

def obtener_objetivo_estudiante(estudiante):
    """Retorna el objetivo final que persigue el estudiante."""
    objetivos = getattr(estudiante, "persigueObjetivo", [])
    if objetivos and len(objetivos) > 0:
        return objetivos[0]
    return None

def obtener_habilidades_poseidas(estudiante):
    """Retorna lista de habilidades que ya posee el estudiante."""
    habilidades = getattr(estudiante, "poseeHabilidad", [])
    return [
        {
            "objeto": h,
            "nombre": getattr(h, "nombreHabilidad", ["Desconocida"])[0] if hasattr(h, "nombreHabilidad") else "Desconocida"
        }
        for h in habilidades
    ]

def inferir_recursos_recomendados(estudiante):
    """
    Recomienda recursos de cursos NO aprobados,
    cuyo formato coincide con el estilo de aprendizaje del estudiante.
    """

    # 1. Obtener estilo
    estilo = obtener_estilo_aprendizaje(estudiante)
    if not estilo:
        return []

    formatos_estilo = getattr(estilo, "formatoCoincideConEstilo", [])

    # 2. Cursos NO aprobados
    cursos_no_aprobados = obtener_cursos_no_aprobados(estudiante)

    recursos_recomendados = set()

    # 3. Recorrer cursos NO aprobados
    for curso in cursos_no_aprobados:
        recursos = getattr(curso, "cursoUtilizaRecurso", [])
        for recurso in recursos:
            formatos_recurso = getattr(recurso, "recursoTieneFormato", [])

            # 4. Coincidencia de formato
            if any(f in formatos_estilo for f in formatos_recurso):
                recursos_recomendados.add(recurso)

    return list(recursos_recomendados)


def obtener_estilo_aprendizaje(estudiante):
    """Retorna el estilo de aprendizaje preferido del estudiante."""
    estilos = getattr(estudiante, "prefiereEstilo", [])
    if estilos and len(estilos) > 0:
        return estilos[0]
    return None

def obtener_prerequisitos_lineales(curso, visitados=None):
    """
    Obtiene la cadena lineal de prerequisitos de un curso.
    Evita ciclos usando un set de visitados.
    """
    if visitados is None:
        visitados = set()
    
    # Evitar ciclos
    if curso in visitados:
        return []
    
    visitados.add(curso)
    cadena = []
    
    # Obtener prerequisitos
    prereqs = getattr(curso, "tienePreRequisito", [])
    
    # Procesar cada prerequisito recursivamente
    for prereq in prereqs:
        cadena.extend(obtener_prerequisitos_lineales(prereq, visitados))
    
    # Agregar el curso actual
    cadena.append(curso)
    
    return cadena

def buscar_cursos_por_habilidad(habilidad_obj):
    """Busca todos los cursos que otorgan una habilidad específica."""
    clases = obtener_clases_ontologia()
    Curso = clases["Curso"]
    
    if Curso is None:
        return []
    
    cursos = []
    for curso in Curso.instances():
        habilidades_curso = getattr(curso, "habilidadCurso", [])
        if habilidad_obj in habilidades_curso:
            cursos.append(curso)
    
    return cursos

def recomendar_cursos_para_habilidad(estudiante, habilidad_obj):
    """
    Recomienda cursos necesarios para adquirir una habilidad específica.
    Retorna solo los cursos NO aprobados.
    """
    # Buscar cursos que otorgan la habilidad
    cursos_con_habilidad = buscar_cursos_por_habilidad(habilidad_obj)
    
    if not cursos_con_habilidad:
        return []
    
    # Obtener cursos aprobados
    aprobados = obtener_cursos_aprobados(estudiante)
    
    # Construir cadena completa de prerequisitos para cada curso objetivo
    todos_cursos_necesarios = []
    for curso_objetivo in cursos_con_habilidad:
        cadena = obtener_prerequisitos_lineales(curso_objetivo)
        for curso in cadena:
            if curso not in todos_cursos_necesarios:
                todos_cursos_necesarios.append(curso)
    
    # Filtrar solo los NO aprobados
    cursos_recomendados = [c for c in todos_cursos_necesarios if c not in aprobados]
    
    # Ordenar por semestre
    def obtener_semestre(curso):
        semestre = getattr(curso, "semestreCurso", None)
        if semestre is None:
            return 0
        if isinstance(semestre, list):
            return semestre[0] if semestre else 0
        return semestre
    
    cursos_recomendados.sort(key=obtener_semestre)
    
    return cursos_recomendados

def recomendar_todos_cursos(estudiante):
    """Recomienda cursos para TODAS las habilidades deseadas del estudiante."""
    habilidades = obtener_habilidades_deseadas(estudiante)
    recomendaciones = {}
    
    for hab_info in habilidades:
        habilidad_obj = hab_info["objeto"]
        habilidad_nombre = hab_info["nombre"]
        
        cursos = recomendar_cursos_para_habilidad(estudiante, habilidad_obj)
        
        def obtener_semestre(c):
            semestre = getattr(c, "semestreCurso", None)
            if semestre is None:
                return 0
            if isinstance(semestre, list):
                return semestre[0] if semestre else 0
            return semestre
        
        recomendaciones[habilidad_nombre] = [
            {
                "nombre": getattr(c, "nombreCurso", ["Desconocido"])[0] if isinstance(getattr(c, "nombreCurso", ["Desconocido"]), list) else getattr(c, "nombreCurso", "Desconocido"),
                "semestre": obtener_semestre(c),
                "creditos": getattr(c, "creditos", [0])[0] if hasattr(c, "creditos") and isinstance(getattr(c, "creditos", [0]), list) else getattr(c, "creditos", 0) if hasattr(c, "creditos") else 0,
            }
            for c in cursos
        ]
    
    return recomendaciones

def recomendar_por_objetivo(estudiante):
    """
    Recomienda cursos basados en el objetivo final del estudiante.
    Usa las reglas SWRL inferidas por el razonador.
    """
    objetivo = obtener_objetivo_estudiante(estudiante)
    
    if not objetivo:
        return []
    
    # Obtener cursos requeridos por el objetivo
    cursos_requeridos = getattr(objetivo, "requiereCursoAprobado", [])
    
    # Obtener cursos aprobados
    aprobados = obtener_cursos_aprobados(estudiante)
    
    # Filtrar solo los NO aprobados
    cursos_recomendados = [c for c in cursos_requeridos if c not in aprobados]
    
    # Construir prerequisitos para cada curso no aprobado
    todos_cursos = []
    for curso in cursos_recomendados:
        cadena = obtener_prerequisitos_lineales(curso)
        for c in cadena:
            if c not in aprobados and c not in todos_cursos:
                todos_cursos.append(c)
    
    # Ordenar por semestre
    def obtener_semestre(c):
        semestre = getattr(c, "semestreCurso", None)
        if semestre is None:
            return 0
        if isinstance(semestre, list):
            return semestre[0] if semestre else 0
        return semestre
    
    todos_cursos.sort(key=obtener_semestre)
    
    return todos_cursos

def obtener_cursos_recomendados_swrl(estudiante):
    """
    Obtiene cursos recomendados por las reglas SWRL (inferidos por el razonador).
    """
    return list(getattr(estudiante, "recomienda", []))

def obtener_cursos_no_recomendados_swrl(estudiante):
    """
    Obtiene cursos que NO deben ser recomendados según reglas SWRL.
    """
    return list(getattr(estudiante, "noRecomendar", []))

def crear_estudiante(nombre, carrera_nombre, habilidad_nombre, objetivo_nombre, estilo_nombre): 
    """Crea un nuevo estudiante en la ontología con todos sus atributos y relaciones."""
    
    clases = obtener_clases_ontologia()
    Estudiante = clases["Estudiante"]
    Carrera = clases["Carrera"]
    Habilidad = clases["Habilidad"]
    ObjetivoFinal = clases["ObjetivoFinal"]
    EstiloAprendizaje = clases["EstiloAprendizaje"]
    onto = clases["onto"]

    # Validar clases necesarias
    if not all([Estudiante, Carrera, Habilidad, ObjetivoFinal, EstiloAprendizaje]):
        raise ValueError("Faltan clases necesarias en la ontología")

    # --------------------------------------
    # Normalizar valores a listas de strings
    # --------------------------------------
    def ensure_list(value):
        if isinstance(value, list):
            return value
        return [value]  # convertir string → lista de 1 elemento

    habilidades_lista = ensure_list(habilidad_nombre)
    objetivos_lista = ensure_list(objetivo_nombre)
    estilos_lista = ensure_list(estilo_nombre)

    # ---------------------------
    # Buscar carrera (solo 1)
    # ---------------------------
    carrera = None
    for c in Carrera.instances():
        nombre_c = getattr(c, "nombreCarrera", [])
        nombre_str = nombre_c[0] if isinstance(nombre_c, list) and nombre_c else str(nombre_c)
        if nombre_str.lower() == carrera_nombre.lower():
            carrera = c
            break
    if not carrera:
        raise ValueError(f"No se encontró la carrera '{carrera_nombre}'")

    # ---------------------------
    # Buscar habilidades (varias)
    # ---------------------------
    habilidades_obj = []
    for habilidad_nombre in habilidades_lista:
        encontrada = None
        for h in Habilidad.instances():
            nombre_h = getattr(h, "nombreHabilidad", [])
            nombre_str = nombre_h[0] if isinstance(nombre_h, list) and nombre_h else str(nombre_h)
            if nombre_str.lower() == habilidad_nombre.lower():
                encontrada = h
                break
        if not encontrada:
            raise ValueError(f"No se encontró la habilidad '{habilidad_nombre}'")
        habilidades_obj.append(encontrada)

    # ---------------------------
    # Buscar objetivo final (solo 1)
    # ---------------------------
    objetivo_nombre = objetivos_lista[0]
    objetivo = None
    for o in ObjetivoFinal.instances():
        nombre_o = getattr(o, "nombreObjetivo", [])
        nombre_str = nombre_o[0] if isinstance(nombre_o, list) and nombre_o else str(nombre_o)
        if nombre_str.lower() == objetivo_nombre.lower():
            objetivo = o
            break
    if not objetivo:
        raise ValueError(f"No se encontró el objetivo final '{objetivo_nombre}'")

    # ---------------------------
    # Buscar estilo de aprendizaje (solo 1)
    # ---------------------------
    estilo_nombre = estilos_lista[0]
    estilo = None
    for e in EstiloAprendizaje.instances():
        nombre_e = getattr(e, "nombreEstilo", [])
        nombre_str = nombre_e[0] if isinstance(nombre_e, list) and nombre_e else str(nombre_e)
        if nombre_str.lower() == estilo_nombre.lower():
            estilo = e
            break
    if not estilo:
        raise ValueError(f"No se encontró el estilo de aprendizaje '{estilo_nombre}'")

    # ---------------------------
    # Crear estudiante
    # ---------------------------
    nuevo_estudiante = Estudiante(f"Est_{nombre.replace(' ', '_')}")
    
    nuevo_estudiante.nombrePersona = [nombre]
    nuevo_estudiante.cursaCarrera = [carrera]
    nuevo_estudiante.poseeHabilidad = habilidades_obj
    nuevo_estudiante.persigueObjetivo = [objetivo]
    nuevo_estudiante.prefiereEstilo = [estilo]

    # Guardar ontología
    onto.save(file=ONTO_PATH)

    return nuevo_estudiante



def listar_todas_carreras():
    """Lista todas las carreras disponibles."""
    clases = obtener_clases_ontologia()
    Carrera = clases["Carrera"]
    
    if not Carrera:
        return []
    
    return [
        getattr(c, "nombreCarrera", ["Desconocida"])[0]
        for c in Carrera.instances()
    ]

def listar_todas_habilidades():
    """Lista todas las habilidades disponibles."""
    clases = obtener_clases_ontologia()
    Habilidad = clases["Habilidad"]
    
    if not Habilidad:
        return []
    
    habilidades = []
    for h in Habilidad.instances():
        nombre = getattr(h, "nombreHabilidad", [])
        nombre_str = nombre[0] if isinstance(nombre, list) and nombre else str(nombre) if nombre else "Desconocida"
        habilidades.append(nombre_str)
    
    return habilidades

def listar_todos_cursos():
    """Lista todos los cursos disponibles."""
    clases = obtener_clases_ontologia()
    Curso = clases["Curso"]
    
    if not Curso:
        return []
    
    cursos = []
    for c in Curso.instances():
        nombre = getattr(c, "nombreCurso", [])
        nombre_str = nombre[0] if isinstance(nombre, list) and nombre else str(nombre) if nombre else "Desconocido"
        
        semestre = getattr(c, "semestreCurso", None)
        semestre_val = semestre[0] if isinstance(semestre, list) and semestre else semestre if semestre else 0
        
        creditos = getattr(c, "creditos", None)
        creditos_val = creditos[0] if isinstance(creditos, list) and creditos else creditos if creditos else 0
        
        cursos.append({
            "nombre": nombre_str,
            "semestre": semestre_val,
            "creditos": creditos_val,
        })
    
    return sorted(cursos, key=lambda x: x["semestre"])

def obtener_recursos_recomendados(estudiante):
    """Obtiene recursos académicos recomendados según el estilo de aprendizaje del estudiante."""
    return list(getattr(estudiante, "recomiendaRecurso", []))

def obtener_ruta_aprendizaje(estudiante):
    """Obtiene la ruta de aprendizaje asignada al estudiante."""
    rutas = getattr(estudiante, "tieneRutaAsignada", [])
    if rutas and len(rutas) > 0:
        return rutas[0]
    return None

def obtener_cursos_en_ruta(ruta):
    """Obtiene los cursos incluidos en una ruta de aprendizaje."""
    if not ruta:
        return []
    return list(getattr(ruta, "incluyeCurso", []))

def obtener_recursos_en_ruta(ruta):
    """Obtiene los recursos prioritarios incluidos en una ruta de aprendizaje."""
    if not ruta:
        return []
    return list(getattr(ruta, "incluyeRecursoPrioritario", []))

def crear_ruta_aprendizaje(estudiante):
    """
    Crea automáticamente una RutaDeAprendizaje para un estudiante.
    - Crea individuo ruta_<nombre>
    - Asigna cursos recomendados ordenados por semestre
    - Asigna recursos prioritarios según estilo de aprendizaje
    - Relaciona estudiante <-> ruta con propiedades bidireccionales
    """
    try:
        clases = obtener_clases_ontologia()
        RutaDeAprendizaje = clases["RutaDeAprendizaje"]
        onto = clases["onto"]
        
        # Obtener nombre del estudiante
        nombre_persona = getattr(estudiante, "nombrePersona", [])
        if not nombre_persona or len(nombre_persona) == 0:
            print("⚠️ Estudiante sin nombre, usando 'Estudiante' por defecto")
            nombre_estudiante = "Estudiante"
        else:
            nombre_estudiante = nombre_persona[0] if isinstance(nombre_persona, list) else str(nombre_persona)
        
        nombre_ruta = f"ruta_{nombre_estudiante.replace(' ', '_')}"
        
        print(f"🔄 Creando ruta: {nombre_ruta}")
        
        # Crear individuo RutaDeAprendizaje
        nueva_ruta = RutaDeAprendizaje(nombre_ruta)
        print(f"✅ Individuo RutaDeAprendizaje creado: {nueva_ruta.name}")
        
        # 1️⃣ Obtener cursos recomendados (ya calculados)
        print(f"🔄 Calculando cursos recomendados...")
        try:
            cursos_recomendados = inferir_cursos_recomendados(estudiante)
            print(f"✅ Cursos recomendados: {len(cursos_recomendados)}")
        except Exception as e:
            print(f"⚠️ Error al calcular cursos: {e}")
            cursos_recomendados = []
        
        # 2️⃣ Ordenar cursos por semestre
        def obtener_semestre(curso):
            semestre = getattr(curso, "semestreCurso", None)
            if semestre is None:
                return 999  # Cursos sin semestre van al final
            if isinstance(semestre, list):
                return semestre[0] if semestre else 999
            return semestre
        
        cursos_ordenados = sorted(cursos_recomendados, key=obtener_semestre)
        
        if cursos_ordenados:
            print(f"✅ Cursos ordenados: {[c.name for c in cursos_ordenados[:3]]}...")
        else:
            print(f"⚠️ No hay cursos para recomendar (puede que ya haya aprobado todos)")
        
        # 3️⃣ Obtener recursos recomendados
        print(f"🔄 Calculando recursos recomendados...")
        try:
            recursos_recomendados = inferir_recursos_recomendados(estudiante)
            print(f"✅ Recursos recomendados: {len(recursos_recomendados)}")
        except Exception as e:
            print(f"⚠️ Error al calcular recursos: {e}")
            recursos_recomendados = []
        
        # 4️⃣ Asignar cursos a la ruta usando incluyeCurso
        if cursos_ordenados:
            nueva_ruta.incluyeCurso = cursos_ordenados
            print(f"✅ Cursos asignados a la ruta")
        else:
            print(f"ℹ️ No se asignaron cursos (lista vacía)")
        
        # 5️⃣ Asignar recursos prioritarios usando incluyeRecursoPrioritario
        if recursos_recomendados:
            nueva_ruta.incluyeRecursoPrioritario = recursos_recomendados
            print(f"✅ Recursos asignados a la ruta")
        else:
            print(f"ℹ️ No se asignaron recursos (lista vacía)")
        
        # 6️⃣ Relacionar RUTA -> ESTUDIANTE (esRutaDe)
        nueva_ruta.esRutaDe = [estudiante]
        print(f"✅ Relación esRutaDe asignada")
        
        # 7️⃣ Relacionar ESTUDIANTE -> RUTA (tieneRutaAsignada)
        estudiante.tieneRutaAsignada = [nueva_ruta]
        print(f"✅ Relación tieneRutaAsignada asignada")
        
        # 8️⃣ Si el estudiante tiene objetivo, relacionar ruta con objetivo
        objetivo = obtener_objetivo_estudiante(estudiante)
        if objetivo:
            nueva_ruta.satisfaceObjetivo = [objetivo]
            print(f"✅ Objetivo asignado a la ruta: {objetivo.name}")
        else:
            print(f"ℹ️ Estudiante sin objetivo definido")
        
        print(f"✅ Ruta '{nombre_ruta}' creada exitosamente con {len(cursos_ordenados)} cursos y {len(recursos_recomendados)} recursos")

        onto.save(file=ONTO_PATH)

        """  estudiante.tieneRutaAsignada = [nueva_ruta]
        print(f"✅ Relación tieneRutaAsignada asignada x2")

        onto.save(file=ONTO_PATH) """
        
        return nueva_ruta
        
    except Exception as e:
        print(f"❌ Error en crear_ruta_aprendizaje: {e}")
        import traceback
        traceback.print_exc()
        return None
    
def obtener_ruta_aprendizaje(estudiante):
    """Obtiene la ruta de aprendizaje asignada al estudiante."""
    rutas = getattr(estudiante, "tieneRutaAsignada", [])
    if rutas and len(rutas) > 0:
        return rutas[0]
    return None

def obtener_cursos_en_ruta(ruta):
    """Obtiene los cursos incluidos en una ruta de aprendizaje."""
    if not ruta:
        return []
    return list(getattr(ruta, "incluyeCurso", []))