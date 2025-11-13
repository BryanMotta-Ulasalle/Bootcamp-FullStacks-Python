from django.urls import path
from .views import recomendaciones, crear_estudiante

urlpatterns = [
    path('recomendaciones/<str:nombre_persona>/', recomendaciones, name='recomendaciones'),
    path("crear_estudiante/", crear_estudiante, name="crear_estudiante"),
]