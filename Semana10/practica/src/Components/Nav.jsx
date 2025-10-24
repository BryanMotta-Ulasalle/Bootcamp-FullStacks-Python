import { NavLi } from "./NavLi"
export function Nav() {
    return (
        <nav className="h-16 items-center flex flex-row justify-between px-8 w-[min(100%,1440px)] border m-auto ">
            <div>LOGO</div>
            <div className="flex flex-row">
                <NavLi label="inicio" />
                <NavLi label="Servicios" />
                <NavLi label="Contacto" />
            </div>
        </nav>
    )
}