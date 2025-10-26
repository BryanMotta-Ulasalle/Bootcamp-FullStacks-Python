export default function DashboardStats() {
  const stats = [
    { title: "Total Cámaras", value: "1,204", color: "", progress: "100%" },
    { title: "Cámaras Activas", value: "1,150", color: "text-primary", detail: "95.5% del total" },
    { title: "Cámaras Inactivas", value: "54", color: "text-red-500", detail: "4.5% del total" },
  ];

  return (
    <div className="grid grid-cols-3 gap-6">
      {stats.map((s, i) => (
        <div key={i} className="bg-black/10 dark:bg-white/10 p-4 rounded-lg flex flex-col justify-between">
          <h3 className="text-sm font-medium text-black/60 dark:text-white/60 mb-2">{s.title}</h3>
          <p className={`text-4xl font-bold ${s.color}`}>{s.value}</p>
          {s.detail && <p className="text-sm text-black/60 dark:text-white/60 mt-2">{s.detail}</p>}
          {s.progress && (
            <div className="w-full bg-black/10 dark:bg-white/10 rounded-full h-2 mt-2">
              <div className={`${s.color} h-2 rounded-full`} style={{ width: s.progress }}></div>
            </div>
          )}
        </div>
      ))}
    </div>
  );
}
