import { Doughnut } from "react-chartjs-2";
import { Chart as ChartJS, ArcElement, Tooltip, Legend } from "chart.js";
ChartJS.register(ArcElement, Tooltip, Legend);

export default function SummaryPanel() {
  const data = {
    labels: ["Norte", "Centro", "Sur"],
    datasets: [
      {
        data: [450, 500, 254],
        backgroundColor: ["#38e07b", "#2ca864", "#1e7043"],
        borderColor: "#122017",
        borderWidth: 4,
      },
    ],
  };

  const options = {
    cutout: "70%",
    plugins: { legend: { position: "bottom", labels: { color: "#fff" } } },
  };

  return (
    <aside className="col-span-1 bg-black/10 dark:bg-white/10 p-6 rounded-lg flex flex-col">
      <h2 className="text-lg font-bold mb-4">Resumen General</h2>
      <div className="h-48 w-48 mx-auto">
        <Doughnut data={data} options={options} />
      </div>
      <div className="mt-6 space-y-3">
        <div className="flex justify-between"><span>Distancia Total</span><span className="font-bold text-primary">125 km</span></div>
        <div className="flex justify-between"><span>Tiempo Aprox.</span><span className="font-bold text-primary">4h 30m</span></div>
        <div className="flex justify-between"><span>Cámaras en Ruta</span><span className="font-bold text-primary">78</span></div>
      </div>
      <div className="mt-auto bg-primary/20 text-primary p-3 rounded-lg flex items-center gap-3 mt-6">
        <span className="material-symbols-outlined">verified</span>
        <p className="text-sm font-medium">Todos los sistemas operativos.</p>
      </div>
    </aside>
  );
}
