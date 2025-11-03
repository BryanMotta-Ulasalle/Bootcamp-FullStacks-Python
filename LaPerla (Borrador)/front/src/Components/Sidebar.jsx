import { useState } from "react";
import { SidebarNav } from "./Sidebar-nav";
import dashboardLogo from "../assets/dashboard-logo.png"
import camaraLogo from "../assets/camaras-logo.png"
export default function Sidebar() {

  const [active, setActive] = useState("dashboard");

  return (
    <aside className="group w-20 hover:w-47 transition-all duration-300 
                 bg-background-light dark:bg-background-dark 
                 border-r border-black/10 dark:border-white/10 
                 flex flex-col items-start py-6 space-y-8 overflow-hidden">

      <div className="w-10 h-10 flex items-center justify-center ml-5">

        <svg className="w-8 h-8 text-black dark:text-white" viewBox="0 0 48 48">
          <path
            d="M24 0.757L47.243 24 24 47.243 0.757 24 24 0.757zM21 35.757V12.243L9.243 24 21 35.757z"
            fill="currentColor"
          />
        </svg>
      </div>

      <nav className="flex flex-col items-center space-y-6">
      <SidebarNav icon={<img src={dashboardLogo} className="w-9 ml-2"/>} 
      label="Dashboard" active={active === "dashboard"}
          onClick={() => setActive("dashboard")}/>

      <SidebarNav icon={<img src={camaraLogo} className="w-10 ml-2"/>} 
      label="Cámaras" active={active === "camaras"}
          onClick={() => setActive("camaras")}/>

      </nav>

    </aside>
  );
}
