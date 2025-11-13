import React, { useState } from "react";

const ConsultaRecomendaciones = () => {
  const [nombre, setNombre] = useState("");
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleSubmit = async (e) => {
    e.preventDefault();
    setLoading(true);
    setError(null);
    setData(null);

    try {
      const response = await fetch(`http://localhost:8000/api/recomendaciones/${nombre}/`);
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const json = await response.json();
      setData(json);
    } catch (err) {
      setError(err.message);
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ padding: "20px", maxWidth: "500px", margin: "0 auto" }}>
      <h2>Consulta de Recomendaciones</h2>
      <form onSubmit={handleSubmit} style={{ marginBottom: "20px" }}>
        <input
          type="text"
          placeholder="Nombre del estudiante"
          value={nombre}
          onChange={(e) => setNombre(e.target.value)}
          style={{ padding: "8px", width: "70%", marginRight: "10px" }}
        />
        <button type="submit" style={{ padding: "8px" }}>Consultar</button>
      </form>

      {loading && <p>Cargando recomendaciones...</p>}
      {error && <p style={{ color: "red" }}>Error: {error}</p>}

      {data && (
        <div>
          <h3>Estudiante: {data.estudiante}</h3>
          <h4>Habilidades deseadas:</h4>
          <ul>
            {data.habilidades_deseadas.map((h, idx) => (
              <li key={idx}>{h}</li>
            ))}
          </ul>
          <h4>Cursos recomendados:</h4>
          <ol>
            {data.recomendaciones.map((c, idx) => (
              <li key={idx}>{c}</li>
            ))}
          </ol>
        </div>
      )}
    </div>
  );
};

export default ConsultaRecomendaciones;
