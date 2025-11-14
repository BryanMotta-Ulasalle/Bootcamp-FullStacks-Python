# advisor/urls.py
from django.urls import path
from .views import (
    recomendaciones,
    crear_estudiante,
    listar_carreras,
    listar_habilidades,
    listar_cursos,
    perfil_estudiante,
    healthcheck
)

urlpatterns = [
    # Healthcheck
    path('health/', healthcheck, name='healthcheck'),
    
    # Estudiantes
    path('recomendaciones/<str:nombre_persona>/', recomendaciones, name='recomendaciones'),
    path('estudiante/<str:nombre_persona>/', perfil_estudiante, name='perfil_estudiante'),
    path('crear_estudiante/', crear_estudiante, name='crear_estudiante'),
    
    # Catálogos
    path('carreras/', listar_carreras, name='listar_carreras'),
    path('habilidades/', listar_habilidades, name='listar_habilidades'),
    path('cursos/', listar_cursos, name='listar_cursos'),
]