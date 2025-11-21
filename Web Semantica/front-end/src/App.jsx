import { BrowserRouter, Routes, Route } from "react-router-dom";

import Header from "./components/Header";
import Main from "./components/Main";
import RegistrarEstudiante from "./components/RegistrarEstudiante";
import VerEstudiante from "./components/VerEstudiante";

function App() {
  return (
    <BrowserRouter>
  {/* CONTENEDOR PRINCIPAL DEL LAYOUT */}
  <div className="h-dvh overflow-hidden flex flex-col">

    <Header />

    {/* El contenedor que tendrá el scroll interno */}
    <div className="flex-1 overflow-y-auto">
      <Routes>
        <Route path="/" element={<Main />} />
        <Route path="/registrar-estudiante" element={<RegistrarEstudiante />} />
        <Route path="/ver-estudiante/:nombre" element={<VerEstudiante />} />
      </Routes>
    </div>

  </div>
</BrowserRouter>

  );
}

export default App;
