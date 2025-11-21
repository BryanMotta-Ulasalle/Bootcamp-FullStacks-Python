import logo from '../assets/main.png';
import { useNavigate } from "react-router-dom";

 

export default function Main(){
     const navigate = useNavigate();
     const irARegistrar = () => {
    navigate("/registrar-estudiante");
  };
    return(
        <main className='h-[800px] flex items-center border justify-center bg-gray-100 '>
            <div className='w-[1200px] flex flex-row bg-white border-none rounded-4xl shadow-xl'>
                <div className='flex p-10 gap-5 flex-col w-[50%] items-start justify-center'>
                    <h1 className='text-5xl font-bold'>Descubre tu ruta academica personalizada</h1>
                <h3 className='text-xl'>Contamos con mas de 1500 perfiles personalizados para llegar a tu perfil</h3>
                <div className='flex flex-col gap-3 '>
                    <button onClick={irARegistrar} className='border rounded-3xl p-4 bg-blue-800 text-white text-xl hover:bg-blue-500 cursor-pointer'>Registrar Nuevo Estudiante</button>
                    <button className='border-3 rounded-3xl p-4 border-blue-800 text-xl hover:bg-blue-50 cursor-pointer'>Consultar Ruta de Aprendizaje</button>
                </div>
                </div>
                <div className='p-5'>
                <img src={logo} alt="" />
                </div>
            </div>
        </main>
    )
}