import Sidebar from "./components/Sidebar";
import Header from "./components/Header";
import DashboardStats from "./components/DashboardStats";
import SummaryPanel from "./components/SummaryPanel";

export default function App() {
  return (
    <div className="flex h-screen font-display bg-background-light bg-[#122017]  dark:bg-background-dark text-black dark:text-white">
      <Sidebar />
      <main className="flex-1 flex flex-col p-6">
        <Header />
        <div className="flex-1 grid grid-cols-3 gap-6">
          <div className="col-span-2 flex flex-col gap-6">
            <DashboardStats />
            {/* Aquí podrías agregar MapSection.jsx */}
          </div>
          <SummaryPanel />
        </div>
      </main>
    </div>
  );
}
