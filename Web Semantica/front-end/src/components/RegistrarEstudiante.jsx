import { useState } from "react";
import { useNavigate } from "react-router-dom";


export default function RegistrarEstudiante() {
  const [nombre, setNombre] = useState("");
  const [carrera, setCarrera] = useState("");
  const [habilidadInput, setHabilidadInput] = useState("");
  const [habilidades, setHabilidades] = useState([]);
  const [objetivo, setObjetivo] = useState("");
  const [estilo, setEstilo] = useState("");
  const navigate = useNavigate();


  const agregarHabilidad = () => {
    if (habilidadInput.trim() !== "") {
      setHabilidades([...habilidades, habilidadInput.trim()]);
      setHabilidadInput(""); // limpiar input
    }
  };

  const eliminarHabilidad = (hab) => {
    setHabilidades(habilidades.filter((h) => h !== hab));
  };

  const enviar = async () => {
  const data = {
    nombre,
    carrera,
    habilidad_poseidas: habilidades,
    objetivo,
    estilo,
  };

  console.log("Enviando:", data);

  try {
    const response = await fetch("http://localhost:8000/api/crear_estudiante/", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(data),
    });

    const result = await response.json();

    if (!response.ok) {
      console.error("Error API:", result);
      alert("Error: " + (result.error || "No se pudo crear el estudiante"));
      return;
    }

    alert("Estudiante registrado con éxito ✔");
    console.log("Respuesta:", result);

    // Opcional: limpiar el formulario
    setNombre("");
    setCarrera("");
    setHabilidadInput("");
    setHabilidades([]);
    setObjetivo("");
    setEstilo("");

     const nombreURL = encodeURIComponent(nombre);

    // ⏳ Espera de 1 segundo y redirige
    setTimeout(() => {
      navigate(`/ver-estudiante/${nombreURL}`);
    }, 800);

  } catch (error) {
    console.error("Error de red:", error);
    alert("Error de conexión con el servidor");
  }
};

const irAlMenu = () => {
    navigate("/");
  };


  return (
    <main className="flex flex-col items-center justify-center bg-gray-50 gap-5 h-full">
      <div className="max-w-3xl mt-8">
        <h1 className="text-5xl font-black text-gray-900 mb-2">
        Registrar Nuevo Estudiante
      </h1>
      <p className="text-gray-600 mb-2 text-xl">
        Complete el siguiente formulario para crear un perfil y recibir
        recomendaciones personalizadas.
      </p>
      </div>
      <div className="min-w-2xl max-w-3xl mx-auto p-8 bg-white rounded-4xl shadow-2xl flex flex-col gap-4">

      <div className="flex flex-col gap-4">
        {/* Nombre */}
      <div className="mb-6">
        <label className="block mb-1 font-medium text-gray-700 text-xl">
          Nombre
        </label>
        <input
          type="text"
          value={nombre}
          onChange={(e) => setNombre(e.target.value)}
          placeholder="Ingrese su nombre completo"
          className="w-full border rounded-lg px-4 py-2"
        />
      </div>

      {/* Carrera */}
      <div className="mb-6">
        <label className="block mb-1 font-medium text-gray-700 text-xl">
          Carrera Actual o de Interés
        </label>
        <select
          value={carrera}
          onChange={(e) => setCarrera(e.target.value)}
          className="w-full border rounded-lg px-4 py-2 text-gray-600"
        >
          <option value="">Seleccione una carrera</option>
          <option value="Ingenieria de Software">Ingenieria de Software</option>
        </select>
      </div>

      {/* Habilidades */}
<div className="mb-6">
  <label className="block mb-1 font-medium text-gray-700 text-xl">
    Habilidades Adquiridas
  </label>

  <div className="flex gap-2 mb-3">
    <select
      value={habilidadInput}
      onChange={(e) => setHabilidadInput(e.target.value)}
      className="flex-1 border rounded-lg px-4 py-2 text-gray-600"
    >
      <option value="">Seleccione una habilidad</option>
      <option value="Pensamiento logico y manejo de estructuras basicas de programacion.">
        Pensamiento logico y manejo de estructuras basicas de programacion.</option>
      <option value="Crear y dar estilo a paginas web estaticas y responsivas.">
        Crear y dar estilo a paginas web estaticas y responsivas.</option>
      <option value="Añadir interactividad y dinamismo a tus sitios web.">
        Añadir interactividad y dinamismo a tus sitios web.</option>
      <option value="Construir interfaces modernas y escalables con componentes reutilizables.">
        Construir interfaces modernas y escalables con componentes reutilizables.</option>
      <option value="Crear APIs y manejar la logica del servidor.">
        Crear APIs y manejar la logica del servidor.</option>
      <option value="Diseñar, almacenar y consultar informacion para tus aplicaciones web.">
        Diseñar, almacenar y consultar informacion para tus aplicaciones web.</option>
      <option value="Integrar frontend y backend en una app completa y publicarla en linea.">
        Integrar frontend y backend en una app completa y publicarla en linea.</option>
    </select>

    <button
      onClick={agregarHabilidad}
      className="px-4 py-2 bg-blue-600 text-white rounded-lg hover:bg-blue-700"
    >
      Añadir
    </button>
  </div>

  {/* Chips */}
  <div className="flex flex-wrap gap-2">
    {habilidades.map((hab) => (
      <span
        key={hab}
        className="px-3 py-1 bg-blue-100 text-blue-700 rounded-full flex items-center gap-2"
      >
        {hab}
        <button
          onClick={() => eliminarHabilidad(hab)}
          className="text-blue-800 font-bold hover:text-red-600"
        >
          ×
        </button>
      </span>
    ))}
  </div>
</div>


      {/* Objetivo */}
      <div className="mb-6">
        <label className="block mb-1 font-medium text-gray-700 text-xl">
          Objetivo Académico o Profesional
        </label>
        <select
          value={objetivo}
          onChange={(e) => setObjetivo(e.target.value)}
          className="w-full border rounded-lg px-4 py-2 text-gray-600"
        >
          <option value="">Seleccione un objetivo</option>
          <option value="Desarrollo Web">Desarrollo Web</option>
        </select>
      </div>

      {/* Estilo */}
      <div className="mb-10">
        <label className="block mb-2 font-medium text-gray-700 text-xl">
          Estilo de Aprendizaje Preferido
        </label>

        <div className="flex gap-4">
          <label className="flex items-center gap-2 border rounded-lg px-4 py-2 cursor-pointer">
            <input
              type="radio"
              name="estilo"
              value="Practico"
              checked={estilo === "Practico"}
              onChange={(e) => setEstilo(e.target.value)}
            />
            Práctico
          </label>

          <label className="flex items-center gap-2 border rounded-lg px-4 py-2 cursor-pointer">
            <input
              type="radio"
              name="estilo"
              value="Teorico"
              checked={estilo === "Teorico"}
              onChange={(e) => setEstilo(e.target.value)}
            />
            Teórico
          </label>
        </div>
      </div>
      </div>

      {/* Botones */}
      <div className="flex justify-end items-center gap-4">
        <button onClick={irAlMenu} className="text-gray-700 hover:underline text-xl">Cancelar</button>

        <button
          onClick={enviar}
          className="flex items-center gap-2 bg-blue-600 text-white px-6 py-3 rounded-lg shadow hover:bg-blue-700"
        >
          Registrar Estudiante
        </button>
      </div>
    </div>
    </main>
    
  );
}
