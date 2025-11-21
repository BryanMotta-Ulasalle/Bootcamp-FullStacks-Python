import { useNavigate } from "react-router-dom";
export default function Header(){
    const navigate = useNavigate();
     const irAlMenu = () => {
    navigate("/");
  };
    return(
        <header className="w-full h-20 bg-blue-800 flex items-center justify-center" >
            <div className="min-w-[1500px]  flex flex-row justify-between">
                <span className="text-white p-8 text-2xl ">Asistente Ingeligente de Recomendacion Academica</span>
                <button onClick={irAlMenu} className="text-white text-2xl cursor-pointer hover:text-blue-200">Inicio</button>
            </div>
            
        </header>
    )
}