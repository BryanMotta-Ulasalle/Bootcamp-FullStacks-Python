/* import { useState } from "react";

export function Task() {
    const [tasks, setTasks] = useState([]); // Lista de tareas
    const [inputValue, setInputValue] = useState(""); // Valor del input
    const [mostrar, setMostrar] = useState(false);

    const handleAddTask = () => {
        if (inputValue.trim() === "") return; // Evita tareas vacías
        setTasks([...tasks, inputValue]); // Agrega nueva tarea
        setInputValue(""); // Limpia el input
    };

    const deleteTask = (indexDelete) => {
        console.log(indexDelete)
        setTasks(tasks.filter((_, index) => index !== indexDelete))
    }

    const editTaks = (indexEdit) => {
        setMostrar(true)
        setInputValue(tasks[indexEdit])
    }
    const saveEdit = () => {

        setTasks([...tasks, inputValue]);
        setMostrar(false)
    }

    return (
        <section className="relative w-full bg-blue-800 h-[500px] p-8 flex justify-center">
            <div className="w-[300px] h-full bg-white rounded-2xl p-4 flex flex-col gap-4">
                <h1 className="text-center">LISTA DE TAREAS</h1>

                <input
                    id="input"
                    className="border p-1"
                    type="text"
                    value={inputValue}
                    onChange={(e) => setInputValue(e.target.value)} // React maneja el input
                />

                <button
                    id="agregar"
                    className="border w-24 ml-auto rounded-lg bg-blue-500 text-white py-0.5"
                    onClick={handleAddTask} // Evento React, no addEventListener
                >
                    Agregar
                </button>

                <div className="mt-4 flex flex-col gap-2 ">
                    {tasks.map((task, index) => (
                        <h1 key={index} className="flex items-center text-lg font-medium">
                            {task}
                            <button onClick={() => deleteTask(index)} className="ml-auto border px-2 rounded text-red-500">X</button>
                            <button onClick={() => editTaks(index)} className="ml-1 border px-2 rounded text-green-500">!</button>
                        </h1>


                    ))}
                </div>
            </div>
            {mostrar && (
                <div className="absolute w-[300px] h-[450px] border bg-white rounded-2xl p-4 flex flex-col gap-4">
                    <h1>Editar Tarea</h1>
                    <input
                        id="inputEdit"
                        className="border p-1"
                        type="text"
                        value={inputValue}
                        onChange={(e) => setInputValue(e.target.value)} // React maneja el input
                    />
                    <button onClick={() => saveEdit()} className="ml-1 border px-2 rounded">Guardar</button>
                </div>
            )}
        </section>
    );
}
 */

import { useState, useEffect } from "react";

function Task() {
  const [tasks, setTasks] = useState([]);
  const [title, setTitle] = useState("");

  // Obtener tareas
  useEffect(() => {
    fetch("http://127.0.0.1:8000/api/tasks/")
      .then(res => res.json())
      .then(data => setTasks(data))
      .catch(err => console.error("Error cargando tareas:", err));
  }, []);

  // Agregar tarea
  const addTask = async () => {
    if (!title.trim()) return;
    try {
      const response = await fetch("http://127.0.0.1:8000/api/tasks/", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ title }),
      });
      const newTask = await response.json();
      setTasks([...tasks, newTask]);
      setTitle("");
    } catch (error) {
      console.error("Error agregando tarea:", error);
    }
  };

  // Eliminar tarea
  const deleteTask = async (id) => {
    try {
      await fetch(`http://127.0.0.1:8000/api/tasks/${id}/`, {
        method: "DELETE",
      });
      setTasks(tasks.filter(t => t.id !== id));
    } catch (error) {
      console.error("Error eliminando tarea:", error);
    }
  };

  return (
    <div style={{ margin: "2rem", fontFamily: "sans-serif" }}>
      <h1>Lista de Tareas ✅</h1>

      <input
        value={title}
        onChange={(e) => setTitle(e.target.value)}
        placeholder="Nueva tarea..."
      />
      <button onClick={addTask}>Agregar</button>

      <ul>
        {tasks.map(task => (
          <li key={task.id}>
            {task.title}{" "}
            <button onClick={() => deleteTask(task.id)}>❌</button>
          </li>
        ))}
      </ul>
    </div>
  );
}

export default Task;
