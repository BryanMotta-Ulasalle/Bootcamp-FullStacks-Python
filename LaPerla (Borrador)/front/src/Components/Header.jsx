export default function Header() {
  return (
    <header className="flex items-center justify-between mb-6">
      <h1 className="text-2xl font-bold">Panel de Control</h1>
      <div className="flex items-center gap-4">
        <div className="relative">
          <span className="material-symbols-outlined absolute left-3 top-1/2 -translate-y-1/2 text-black/50 dark:text-white/50">
            
          </span>
          <input
            type="text"
            placeholder="Buscar cámaras..."
            className="w-full bg-black/10 dark:bg-white/10 border-0 rounded-lg pl-10 pr-4 py-2 focus:ring-primary focus:ring-2"
          />
        </div>
        <select className="bg-black/10 dark:bg-white/10 border-0 rounded-lg py-2 px-4 appearance-none focus:ring-primary focus:ring-2">
          <option>Todas las Zonas</option>
          <option>Norte</option>
          <option>Centro</option>
          <option>Sur</option>
        </select>
      </div>
    </header>
  );
}
