// src/components/AcademicAdvisor.jsx
import { useState, useEffect } from "react";

export default function AcademicAdvisor() {
  const [nombre, setNombre] = useState("");
  const [carrera, setCarrera] = useState("IngenieriaSoftware"); // Solo la que tienes
  const [habilidades, setHabilidades] = useState(["Desarrollo Web"]); // Solo la que tienes
  const [recomendaciones, setRecomendaciones] = useState([]);
  const [mensaje, setMensaje] = useState("");

  const crearEstudiante = async () => {
    try {
      const res = await fetch("http://localhost:8000/api/crear_estudiante/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          nombre,
          carrera,
          habilidades,
        }),
      });
      const data = await res.json();
      if (res.ok) {
        setMensaje(data.mensaje);
      } else {
        setMensaje(data.error);
      }
    } catch (error) {
      setMensaje(error.message);
    }
  };

  const obtenerRecomendaciones = async () => {
    try {
      const res = await fetch(`http://localhost:8000/api/recomendaciones/${nombre}/`);
      const data = await res.json();
      if (res.ok) {
        setRecomendaciones(data.recomendaciones);
      } else {
        setMensaje(data.error);
      }
    } catch (error) {
      setMensaje(error.message);
    }
  };

  return (
    <div style={{ padding: "2rem", fontFamily: "Arial, sans-serif" }}>
      <h1>Asesor Académico</h1>

      <div style={{ marginBottom: "2rem" }}>
        <h2>Crear Estudiante</h2>
        <input
          type="text"
          placeholder="Nombre del estudiante"
          value={nombre}
          onChange={(e) => setNombre(e.target.value)}
          style={{ marginRight: "1rem" }}
        />
        <button onClick={crearEstudiante}>Crear</button>
      </div>

      <div style={{ marginBottom: "2rem" }}>
        <h2>Consultar Recomendaciones</h2>
        <button onClick={obtenerRecomendaciones}>Obtener Recomendaciones</button>
        {recomendaciones.length > 0 && (
          <ul>
            {recomendaciones.map((curso, i) => (
              <li key={i}>{curso}</li>
            ))}
          </ul>
        )}
      </div>

      {mensaje && <p style={{ color: "red" }}>{mensaje}</p>}
    </div>
  );
}
