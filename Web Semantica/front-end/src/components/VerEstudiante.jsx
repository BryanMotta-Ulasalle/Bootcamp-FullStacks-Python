import { useParams } from "react-router-dom";
import { useEffect, useState } from "react";

export default function VerEstudiante() {
    const { nombre } = useParams();
    const nombreDecodificado = decodeURIComponent(nombre);

    const [estudiante, setEstudiante] = useState(null);
    const [loading, setLoading] = useState(true);

    useEffect(() => {
        const fetchEstudiante = async () => {
            try {
                const response = await fetch(
                    `http://localhost:8000/api/estudiante/${encodeURIComponent(nombreDecodificado)}/`
                );

                const data = await response.json();

                // Aquí el backend devuelve { estudiante: { ... } }
                setEstudiante(data.estudiante);
            } catch (error) {
                console.error("Error cargando estudiante:", error);
            } finally {
                setLoading(false);
            }
        };

        fetchEstudiante();
    }, [nombreDecodificado]);

    if (loading) {
        return <p className="text-center mt-10 text-xl">Cargando información...</p>;
    }

    if (!estudiante) {
        return (
            <p className="text-center mt-10 text-red-600">
                Estudiante no encontrado
            </p>
        );
    }

    return (
        <div className="bg-gray-100 w-full h-full flex items-center justify-center">

            <div className="max-w-3xl mx-auto p-6 bg-white shadow-md rounded-xl flex flex-col gap-7">
                <h1 className="text-4xl font-bold mb-2 text-center ">
                    {estudiante.nombre}
                </h1>

                <div className="space-y-4 text-lg">
                    <p>
                        <span className="font-semibold text-gray-700 text-2xl">Carrera:</span>{" "}
                        {estudiante.carrera}
                    </p>

                    <p>
                        <span className="font-semibold text-gray-700 text-2xl">Objetivo:</span>{" "}
                        {estudiante.objetivo}
                    </p>

                    <p>
                        <span className="font-semibold text-gray-700 text-2xl">
                            Estilo de Aprendizaje:
                        </span>{" "}
                        {estudiante.estilo_aprendizaje}
                    </p>

                    {/* HABILIDADES */}
                    <div>
                        <span className="font-semibold text-gray-700 text-2xl" >
                            Habilidades Poseídas:
                        </span>
                        <ul className="list-disc ml-6 mt-2">
                            {estudiante.habilidades_poseidas.map((hab, i) => (
                                <li key={i}>{hab}</li>
                            ))}
                        </ul>
                    </div>

                    {/* CURSOS APROBADOS */}
                    <div>
                        <span className="font-semibold text-gray-700 text-2xl">
                            Cursos Aprobados ({estudiante.total_cursos_aprobados}):
                        </span>

                        <ul className="mt-2 space-y-2">
                            {estudiante.cursos_aprobados.map((curso, i) => (
                                <li
                                    key={i}
                                    className="p-3 bg-gray-100 rounded-xl border-2 border-blue-800"
                                >
                                    <span className="font-semibold">{curso.nombre}</span> —{" "}
                                    <span className="text-gray-700">
                                        Semestre {curso.semestre}
                                    </span>
                                </li>
                            ))}
                        </ul>
                    </div>
                </div>
                <button className="border-blue-800 border rounded-2xl p-4 bg-blue-800 text-white text-2xl cursor-pointer hover:bg-blue-500 mt-2" >Ver Ruta de Aprendizaje</button>
            </div>
        </div>
    );
}
