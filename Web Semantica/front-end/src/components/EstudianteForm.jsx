import React, { useState } from 'react';
import axios from 'axios';

const API_BASE_URL = 'http://localhost:8000/api'; // Asegúrate de que esta URL sea correcta

const EstudianteForm = ({ onStudentCreated }) => {
    // 1. Estado para manejar los campos del formulario
    const [formData, setFormData] = useState({
        nombre: '',
        ciclo: '',
        nivelActual: '',
        tienePromedio: '',
        carrera: '',
        razonar: true, // Por defecto, forzamos el razonador
    });
    const [loading, setLoading] = useState(false);
    const [message, setMessage] = useState('');
    const [error, setError] = useState('');

    // 2. Manejador de cambios en los inputs
    const handleChange = (e) => {
        const { name, value, type, checked } = e.target;
        setFormData(prev => ({
            ...prev,
            [name]: type === 'checkbox' ? checked : value
        }));
        setError(''); // Limpiar errores al escribir
    };

    // 3. Manejador de envío del formulario
    const handleSubmit = async (e) => {
        e.preventDefault();
        setLoading(true);
        setMessage('');
        setError('');

        // Conversión de tipos (necesaria para enviar números)
        const payload = {
            ...formData,
            ciclo: parseInt(formData.ciclo) || null,
            nivelActual: parseInt(formData.nivelActual) || null,
            tienePromedio: parseFloat(formData.tienePromedio) || null,
        };

        // El campo 'carrera' se llama 'carrera_nombre' en el back-end, pero
        // la vista lo recibe como 'carrera', así que usamos 'carrera' en el payload.

        try {
            // El endpoint de tu back-end es /api/estudiantes/ (según urls.py)
            const response = await axios.post(`${API_BASE_URL}/estudiantes/`, payload);
            
            setMessage(`Estudiante ${response.data.estudiante} creado/actualizado con éxito.`);
            
            // 4. Notificar al componente padre (App.js) para recargar las recomendaciones
            if (onStudentCreated) {
                onStudentCreated();
            }

        } catch (err) {
            console.error("Error al crear estudiante:", err);
            setError('Error al conectar con el servidor o datos incompletos.');
            if (err.response && err.response.data && err.response.data.error) {
                setError(`Error: ${err.response.data.error}`);
            }
        } finally {
            setLoading(false);
        }
    };

    return (
        <div className="form-container">
            <h2>Crear/Actualizar Estudiante y Generar Recomendaciones</h2>
            <form onSubmit={handleSubmit}>
                <input type="text" name="nombre" value={formData.nombre} onChange={handleChange} placeholder="Nombre (identificador)" required />
                <input type="number" name="ciclo" value={formData.ciclo} onChange={handleChange} placeholder="Ciclo actual" />
                <input type="number" name="nivelActual" value={formData.nivelActual} onChange={handleChange} placeholder="Nivel actual (ej. 7)" />
                <input type="text" name="tienePromedio" value={formData.tienePromedio} onChange={handleChange} placeholder="Promedio (ej. 15.2)" />
                <input type="text" name="carrera" value={formData.carrera} onChange={handleChange} placeholder="Carrera (ej. Informatica)" />
                
                <label>
                    <input type="checkbox" name="razonar" checked={formData.razonar} onChange={handleChange} />
                    Forzar razonador después de guardar
                </label>

                <button type="submit" disabled={loading}>
                    {loading ? 'Procesando...' : 'Guardar y Recalcular'}
                </button>
            </form>
            {message && <p className="success-message">{message}</p>}
            {error && <p className="error-message">{error}</p>}
        </div>
    );
};

export default EstudianteForm;