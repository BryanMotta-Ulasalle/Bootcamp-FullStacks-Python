import React from "react";
import ConsultaRecomendaciones from "./components/ConsultaRecomendaciones";
import RegistrarEstudiante from "./components/RegistrarEstudiante"

function App() {
  return (
    <div className="App">
      <RegistrarEstudiante/>
      <h1>Prueba de Recomendaciones de Cursos</h1>
      <ConsultaRecomendaciones />
    </div>
  );
}

export default App;
