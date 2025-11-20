import logo from '../assets/main.png';


export default function Main(){
    return(
        <main>
            <div>
                <h1>Descubre tu ruta academica personalizada</h1>
                <h3>Contamos con mas de 1500 perfiles personalizados para llegar a tu perfil</h3>
                <div>
                    <button>Registrar Nuevo Estudiante</button>
                    <button>Consultar Ruta de Aprendizaje</button>
                </div>
            </div>
            <div>
                <img src={logo} alt="" />
            </div>
        </main>
    )
}