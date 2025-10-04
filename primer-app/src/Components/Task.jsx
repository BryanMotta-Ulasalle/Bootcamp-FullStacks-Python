/* export function Task() {

    const input = document.querySelector('#input')
    const agregar = document.querySelector('agregar')

    agregar.addEventListener('click', () => {
        const tarea = document.createElement('h1')
    })

    return (
        <section className="w-full bg-blue-800 h-[500px] p-8 flex justify-center">
            <div className="w-[300px] h-full bg-white rounded-2xl p-4 flex flex-col gap-4">
                <h1 className="text-center">LISTA DE TAREAS</h1>
                <input id="input" className="border p-1" type="text" />
                <button id="agregar" className="border w-24 ml-auto rounded-lg bg-blue-500 text-white py-0.5"> Agregar </button>
            </div>
        </section>

    )
} */

import { useState } from "react";

export function Task() {
    const [tasks, setTasks] = useState([]); // Lista de tareas
    const [inputValue, setInputValue] = useState(""); // Valor del input

    const handleAddTask = () => {
        if (inputValue.trim() === "") return; // Evita tareas vacías
        setTasks([...tasks, inputValue]); // Agrega nueva tarea
        setInputValue(""); // Limpia el input
    };

    const deleteTask = (index) => {
        setTasks(task.filter())
    }

    return (
        <section className="w-full bg-blue-800 h-[500px] p-8 flex justify-center">
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

                {/* Aquí renderizamos la lista de tareas */}
                <div className="mt-4 flex flex-col gap-2 ">
                    {tasks.map((task, index) => (
                        <h1 key={index} className="flex items-center text-lg font-medium">
                            {task}
                            <button className="ml-auto border px-2 rounded text-red-500">X</button>
                        </h1>


                    ))}
                </div>
            </div>
        </section>
    );
}
