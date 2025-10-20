function Header(){
    return(
        <header className="w-[min(90%,1440px)] flex justify-between items-center m-auto h-16 text-primary">
            <div>LOGO</div>
            <div className="flex gap-16">
                <li className="list-none cursor-pointer">Pagina 1</li>
                <li className="list-none cursor-pointer">Pagina 2</li>
                <li className="list-none cursor-pointer">Pagina 3</li>
            </div>
        </header>
    )
}

export default Header